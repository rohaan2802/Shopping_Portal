"""Headless browser checks for index.html (Playwright).

    python -m pip install playwright      # uses installed Chrome/Edge if present
    python tests/site_check.py

Opens the page twice - straight from disk (file://, like double-clicking
index.html) and from a local HTTP server (like GitHub Pages) - and checks
desktop and mobile layouts in both themes: right-aligned navbar, hamburger and
theme icons, back-to-top button, all 30 screenshots, every content section,
insights tables, gallery filters, sandbox billing and console errors.
"""

from pathlib import Path
import functools, http.server, json, sys, threading
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]


class Quiet(http.server.SimpleHTTPRequestHandler):
    def log_message(self, *args):
        pass


def launch(p):
    for kwargs in ({}, {"channel": "chrome"}, {"channel": "msedge"}):
        try:
            return p.chromium.launch(**kwargs)
        except Exception:
            continue
    raise RuntimeError("No Chromium, Chrome or Edge available for Playwright")


def main():
    server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), functools.partial(Quiet, directory=str(ROOT)))
    threading.Thread(target=server.serve_forever, daemon=True).start()
    http_url = f"http://127.0.0.1:{server.server_port}/index.html"
    expected = len(json.loads((ROOT / "assets/screenshots/manifest.json").read_text()))
    results, errors = [], []

    def check(name, ok, detail=""):
        results.append((name, bool(ok), detail))

    def watch(page, tag):
        page.on("pageerror", lambda e: errors.append(f"{tag}: {e}"))
        page.on("console", lambda m: m.type == "error" and errors.append(f"{tag}: {m.text}"))

    nav_links = 12
    with sync_playwright() as p:
        browser = launch(p)
        for mode, url in (("file", (ROOT / "index.html").as_uri()), ("http", http_url)):
            for theme in ("light", "dark"):
                for layout, viewport in (("desktop", {"width": 1600, "height": 900}), ("mobile", {"width": 390, "height": 844})):
                    tag = f"{mode}/{layout}/{theme}"
                    page = browser.new_page(viewport=viewport)
                    watch(page, tag)
                    page.add_init_script(f"localStorage.clear(); localStorage.setItem('shop-theme', JSON.stringify('{theme}'))")
                    page.goto(url)
                    page.wait_for_selector("#products .product", timeout=10000)
                    check(f"{tag}: theme applied", page.evaluate("document.documentElement.dataset.theme") == theme)
                    icon = "#theme .i-sun" if theme == "dark" else "#theme .i-moon"
                    check(f"{tag}: theme toggle icon visible", page.locator(icon).is_visible())
                    tops = page.evaluate("[...document.querySelectorAll('#features .feature')].map(c => { const s = getComputedStyle(c); return s.borderTopWidth + ' ' + s.borderTopColor; })")
                    check(f"{tag}: all {len(tops)} feature cards share one top accent line", len(tops) == 9 and len(set(tops)) == 1 and tops[0].startswith("3px"), str(set(tops)))
                    check(f"{tag}: no horizontal overflow", page.evaluate("document.documentElement.scrollWidth <= innerWidth"))
                    links = page.locator("#navigation a:visible").count()
                    if layout == "desktop":
                        nav = page.locator("#navigation").bounding_box()
                        actions = page.locator(".nav-actions").bounding_box()
                        check(f"{tag}: {nav_links} navbar links visible", links == nav_links, str(links))
                        check(f"{tag}: navbar aligned right", 0 <= actions["x"] - (nav["x"] + nav["width"]) <= 24 and not page.locator("#menu").is_visible())
                    else:
                        check(f"{tag}: hamburger icon visible", page.locator("#menu .i-bars").is_visible() and links == 0)
                        page.click("#menu")
                        check(f"{tag}: menu opens with close icon", page.locator("#navigation a:visible").count() == nav_links and page.locator("#menu .i-close").is_visible())
                        page.click("#navigation a[href='#screenshots']")
                        check(f"{tag}: menu closes after navigation", page.locator("#navigation a:visible").count() == 0)
                    loaded = page.evaluate("Promise.all([...document.querySelectorAll('#gallery img')].map(i => { i.loading = 'eager'; return i.decode().then(() => i.naturalWidth).catch(() => 0); }))")
                    check(f"{tag}: {expected} screenshots load at 1600px", len(loaded) == expected and all(w == 1600 for w in loaded), str(loaded.count(1600)))
                    page.evaluate("window.scrollTo(0, document.querySelector('#screenshots').offsetTop)")
                    page.wait_for_timeout(300)
                    check(f"{tag}: back-to-top button visible", page.locator("#top").is_visible())
                    inside = page.evaluate("""() => { const b = document.querySelector('#top').getBoundingClientRect(), r = document.querySelector('#top .fill').getBoundingClientRect();
                        return r.left >= b.left && r.top >= b.top && r.right <= b.right && r.bottom <= b.bottom; }""")
                    check(f"{tag}: back-to-top progress ring stays inside the button", inside)
                    check(f"{tag}: active link follows scroll", page.evaluate("document.querySelector('#navigation a.active').hash") == "#screenshots")
                    page.close()

            # content of every section (desktop, light theme)
            page = browser.new_page(viewport={"width": 1600, "height": 900})
            watch(page, f"{mode}/content")
            page.add_init_script("localStorage.clear()")
            page.goto(url + "#demo")
            page.wait_for_selector("#products .product", timeout=10000)
            empty = page.evaluate("[...document.querySelectorAll('main section')].filter(s => s.innerText.trim().length < 200).map(s => s.id)")
            check(f"{mode}: all {page.locator('main section').count()} sections have content", not empty, str(empty))
            check(f"{mode}: insights KPIs show 351 products", page.locator("#kpis .kpi").count() == 5 and "351" in page.inner_text("#kpis"))
            check(f"{mode}: price statistics table has 12 categories + total", page.locator("#priceStats tr").count() == 13)
            check(f"{mode}: value bars and low-stock table filled", page.locator("#valueBars .bar").count() == 12 and page.locator("#lowStock tr").count() == 10)
            page.click("#filters .chip:has-text('Tests')")
            shown = page.locator("#gallery figure.shot:visible").count()
            check(f"{mode}: gallery category filter", 0 < shown < expected, str(shown))
            page.click("#filters .chip:has-text('All')")
            page.locator("#gallery figure.shot button").first.click()
            check(f"{mode}: screenshot viewer opens", page.locator("#viewer").is_visible() and page.evaluate("document.querySelector('#viewerImage').naturalWidth") == 1600)
            first = page.inner_text("#viewerTitle")
            page.click("#next")
            moved = page.inner_text("#viewerTitle") != first
            page.click("#previous")
            back = page.inner_text("#viewerTitle") == first
            page.click("#zoom")
            zoomed = page.get_attribute("#zoom", "aria-pressed") == "true"
            no_download = page.locator("#viewer a[download], #imageDownload").count() == 0
            check(f"{mode}: viewer prev/next/zoom work, no download link", moved and back and zoomed and no_download, f"{moved} {back} {zoomed} {no_download}")
            page.click("#close")
            closed = not page.locator("#viewer").is_visible()
            check(f"{mode}: viewer closes", closed)
            page.fill("#productSearch", "Apples")
            page.locator("#products .product").first.locator("input").fill("2")
            page.locator("#products .product").first.locator("button").click()
            check(f"{mode}: sandbox 2 Apples total PKR 330.00", "PKR 330.00" in page.inner_text("#bill .total"))
            page.fill("#coupon", "ROHAAN10")
            page.click("#couponForm button")
            check(f"{mode}: sandbox ROHAAN10 total PKR 297.00", "PKR 297.00" in page.inner_text("#bill .total"))
            page.click("#checkout")
            check(f"{mode}: sandbox receipt printed", "SIMULATED RECEIPT" in page.inner_text("#receipt"))
            page.close()

            # touch gestures in the screenshot viewer (mobile emulation with real touch input)
            ctx = browser.new_context(viewport={"width": 390, "height": 844}, device_scale_factor=2, is_mobile=True, has_touch=True)
            page = ctx.new_page()
            watch(page, f"{mode}/touch")
            page.goto(url)
            page.locator("#gallery figure.shot button").nth(4).tap()
            page.wait_for_selector("#viewer[open]")
            page.wait_for_function("document.querySelector('#viewerImage').complete")
            cdp = ctx.new_cdp_session(page)

            def drag(x1, y1, x2, y2):
                cdp.send("Input.dispatchTouchEvent", {"type": "touchStart", "touchPoints": [{"x": x1, "y": y1}]})
                for i in range(1, 9):
                    cdp.send("Input.dispatchTouchEvent", {"type": "touchMove", "touchPoints": [{"x": x1 + (x2 - x1) * i / 8, "y": y1 + (y2 - y1) * i / 8}]})
                cdp.send("Input.dispatchTouchEvent", {"type": "touchEnd", "touchPoints": []})
                page.wait_for_timeout(250)

            box = page.locator("#viewerImage").bounding_box()
            cx, cy = box["x"] + box["width"] / 2, box["y"] + box["height"] / 2
            title = lambda: page.inner_text("#viewerTitle")
            zoomed = lambda: page.evaluate("document.querySelector('#viewerImage').classList.contains('enlarged')") and page.get_attribute("#zoom", "aria-pressed") == "true"
            start = title()
            drag(cx, cy - 60, cx + 10, cy + 60)
            check(f"{mode}: touch - vertical swipe never changes screenshot", title() == start)
            drag(cx + 120, cy, cx - 120, cy + 10)
            moved = title()
            check(f"{mode}: touch - horizontal swipe changes screenshot when not zoomed", moved != start, f"{start} -> {moved}")
            page.locator("#zoom").tap()
            page.wait_for_timeout(200)
            box = page.locator(".imagewrap").bounding_box()
            cx, cy = box["x"] + box["width"] / 2, box["y"] + box["height"] / 2
            drag(cx + 140, cy, cx - 140, cy)
            sx = page.evaluate("document.querySelector('.imagewrap').scrollLeft")
            check(f"{mode}: touch - zoomed horizontal pan scrolls the image, same screenshot, still zoomed", title() == moved and zoomed() and sx > 0, f"{title()} zoomed={zoomed()} scrollLeft={sx}")
            drag(cx - 140, cy, cx + 100, cy)
            check(f"{mode}: touch - zoomed pan back does not navigate", title() == moved and zoomed(), title())
            drag(cx, cy + 80, cx, cy - 80)
            check(f"{mode}: touch - zoomed vertical pan stays on the same zoomed screenshot", title() == moved and zoomed())
            ctx.close()
        browser.close()
    server.shutdown()
    check("no JavaScript or console errors (file:// and http://)", not errors, "; ".join(errors))
    for name, ok, detail in results:
        print(f"{'PASS' if ok else 'FAIL'} {name}" + (f" ({detail})" if detail and not ok else ""))
    failed = sum(not ok for _, ok, _ in results)
    print(f"{len(results) - failed}/{len(results)} site checks passed")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
