"""Render the 30 README/webpage screenshots from real outputs, source code and browser captures.

    python tools/render_figures.py            # report + code screenshots
    python tools/render_figures.py --browser  # also recapture the two browser screenshots (Playwright)

Every image is 1600 x 1000: a dark terminal/editor window on a saturated,
category-coloured backdrop so it stays readable in light and dark themes.
Text is auto-sized as large as the excerpt allows. Requires Pillow.
"""

from pathlib import Path
from collections import Counter
import json, re, sys, threading, functools, http.server
from PIL import Image, ImageDraw, ImageFilter, ImageFont

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
OUT = ROOT / "assets/screenshots"
W, H = 1600, 1000

FONT_CANDIDATES = {
    "mono": ["C:/Windows/Fonts/consola.ttf", "/usr/share/fonts/truetype/sand-box/google/IBM Plex Mono/IBMPlexMono-Regular.ttf",
             "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", "DejaVuSansMono.ttf"],
    "mono_bold": ["C:/Windows/Fonts/consolab.ttf", "/usr/share/fonts/truetype/sand-box/google/IBM Plex Mono/IBMPlexMono-Bold.ttf",
                  "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", "DejaVuSansMono-Bold.ttf"],
    "sans": ["C:/Windows/Fonts/segoeui.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", "DejaVuSans.ttf"],
    "sans_bold": ["C:/Windows/Fonts/segoeuib.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", "DejaVuSans-Bold.ttf"],
}


@functools.lru_cache(None)
def font(kind, size):
    for path in FONT_CANDIDATES[kind]:
        try:
            return ImageFont.truetype(path, size)
        except OSError:
            continue
    return ImageFont.load_default()


# Backdrop gradients per category (top-left -> bottom-right).
THEMES = {
    "Catalog": ("#0ea5e9", "#1d4ed8"),
    "Analytics": ("#8b5cf6", "#db2777"),
    "Native": ("#f59e0b", "#dc2626"),
    "Tests": ("#10b981", "#0f766e"),
    "Source": ("#6366f1", "#0891b2"),
    "Browser": ("#f43f5e", "#7c3aed"),
}
INK = {"text": "#e6edf3", "dim": "#7d8590", "head": "#79c0ff", "ok": "#56d364", "bad": "#ff7b72",
       "money": "#ffa657", "info": "#a5d6ff", "kw": "#ff7bdb", "type": "#7ee787", "str": "#ffd479",
       "num": "#79c0ff", "com": "#8b949e", "fn": "#d2a8ff"}

CPP_KW = r"\b(if|else|for|while|return|const|auto|try|catch|throw|using|namespace|struct|inline|void|bool|int|std|string|vector|size_t|template|class|public|private|static|true|false|break|continue|new|delete)\b"
TOKEN = re.compile(r'(?P<com>//.*$)|(?P<str>"(?:\\.|[^"\\])*")|(?P<kw>' + CPP_KW + r')|(?P<num>\b\d+(?:\.\d+)?\b)|(?P<fn>\b[A-Za-z_]\w*(?=\())')


def read(name):
    return (ROOT / name).read_text(encoding="utf-8", errors="replace")


def clean(text):
    """Strip console centring, squeeze blank runs and trailing spaces."""
    lines = [l.rstrip() for l in text.replace("\t", "    ").splitlines()]
    pad = min((len(l) - len(l.lstrip()) for l in lines if l.strip()), default=0)
    out = []
    for l in lines:
        l = l[pad:] if len(l) >= pad else l.lstrip()
        if not l.strip() and (not out or not out[-1].strip()):
            continue
        out.append(l)
    return out


def excerpt(name, start, stop=None, count=40, skip=0):
    lines = clean(read(name))
    hits = [i for i, l in enumerate(lines) if start in l]
    i = hits[min(skip, len(hits) - 1)]
    chunk = lines[i : i + count]
    if stop:
        for j, l in enumerate(chunk[1:], 1):
            if stop in l:
                chunk = chunk[: j + 1]
                break
    while chunk and not chunk[-1].strip():
        chunk.pop()
    return chunk


def code(name, start, count):
    lines = read(name).splitlines()
    i = next(n for n, l in enumerate(lines) if start in l)
    chunk = [l.rstrip().replace("\t", "    ") for l in lines[i : i + count]]
    pad = min((len(l) - len(l.lstrip()) for l in chunk if l.strip()), default=0)
    return [l[pad:] for l in chunk]


def gradient(size, a, b, diagonal=True):
    w, h = size
    ca = Image.new("RGB", (1, 1), a).getpixel((0, 0))
    cb = Image.new("RGB", (1, 1), b).getpixel((0, 0))
    horizontal = Image.linear_gradient("L").rotate(90).resize((w, h))
    horizontal = horizontal.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    mask = Image.blend(horizontal, Image.linear_gradient("L").resize((w, h)), 0.5) if diagonal else horizontal
    return Image.composite(Image.new("RGB", size, cb), Image.new("RGB", size, ca), mask)


def frame(number, title, category, source):
    """Backdrop + title + dark window. Returns image, draw, body box."""
    a, b = THEMES[category]
    im = gradient((W, H), a, b)
    d = ImageDraw.Draw(im)
    d.text((60, 30), f"{number}", font=font("sans_bold", 46), fill=(255, 255, 255, 255))
    nw = d.textlength(f"{number}", font=font("sans_bold", 46))
    d.text((60 + nw + 18, 34), title, font=font("sans_bold", 42), fill="#ffffff")
    tag = category.upper()
    tw = d.textlength(tag, font=font("sans_bold", 20))
    d.rounded_rectangle((W - 60 - tw - 36, 42, W - 60, 82), radius=20, fill=(255, 255, 255))
    d.text((W - 60 - tw - 18, 49), tag, font=font("sans_bold", 20), fill=a)
    box = (48, 112, W - 48, H - 36)
    shadow = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    ImageDraw.Draw(shadow).rounded_rectangle((box[0] + 4, box[1] + 14, box[2] + 4, box[3] + 14), radius=22, fill=(0, 0, 0, 120))
    im.paste(shadow.filter(ImageFilter.GaussianBlur(16)), (0, 0), shadow.filter(ImageFilter.GaussianBlur(16)))
    d = ImageDraw.Draw(im)
    d.rounded_rectangle(box, radius=20, fill="#0d1117", outline="#30363d", width=2)
    d.rounded_rectangle((box[0], box[1], box[2], box[1] + 54), radius=20, fill="#161b22")
    d.rectangle((box[0] + 2, box[1] + 30, box[2] - 2, box[1] + 54), fill="#161b22")
    d.line((box[0], box[1] + 54, box[2], box[1] + 54), fill="#30363d", width=2)
    for k, c in enumerate(("#ff5f57", "#febc2e", "#28c840")):
        d.ellipse((box[0] + 24 + k * 30, box[1] + 18, box[0] + 42 + k * 30, box[1] + 36), fill=c)
    label = f"Shopping_Portal  ›  {source}"
    lw = d.textlength(label, font=font("mono", 21))
    d.text(((box[0] + box[2] - lw) / 2, box[1] + 15), label, font=font("mono", 21), fill="#9da7b3")
    return im, d, (box[0] + 40, box[1] + 80, box[2] - 40, box[3] - 28)


def line_colour(line):
    s = line.strip()
    if re.match(r"^(\[?PASS\]?|\[OK\]|\d+/\d+ Test .*Passed|100% tests passed|.*checks passed$)", s):
        return INK["ok"]
    if re.search(r"\bFAIL|error|rejected with|Catalog error", s):
        return INK["bad"]
    if re.match(r"^[-=]{6,}$", s):
        return INK["dim"]
    if s.startswith("[INFO]") or s.startswith("Test project") or s.startswith("Start "):
        return INK["info"]
    if re.match(r"^(TOTAL|Total|Estimated|Average|\d+ products \|)", s) or "TOTAL AMOUNT" in s:
        return INK["money"]
    if "####" in s or (s.isupper() and len(s) > 6 and not re.search(r"\d", s)) or re.match(r"^[A-Z][A-Z &|/()\-]{5,}[A-Z)](\s.*)?$", s):
        return INK["head"]
    return INK["text"]


def fit(draw, lines, box, kind):
    width, height = box[2] - box[0], box[3] - box[1]
    for size in range(40, 15, -1):
        f = font(kind, size)
        step = int(size * 1.42)
        if len(lines) * step <= height and all(draw.textlength(l, font=f) <= width for l in lines):
            return size, step
    return 16, int(16 * 1.42)


def draw_text(d, lines, box, is_code=False):
    size, step = fit(d, lines, box, "mono")
    width = max(d.textlength(l, font=font("mono", size)) for l in lines)
    total = len(lines) * step
    x0 = box[0] + max(0, (box[2] - box[0] - width) / 2) if not is_code else box[0]
    y = box[1] + max(0, (box[3] - box[1] - total) / 2)
    for line in lines:
        if is_code:
            x, pos = x0, 0
            for m in TOKEN.finditer(line):
                if m.start() > pos:
                    d.text((x, y), line[pos:m.start()], font=font("mono", size), fill=INK["text"])
                    x += d.textlength(line[pos:m.start()], font=font("mono", size))
                kind = m.lastgroup
                fnt = font("mono_bold" if kind == "kw" else "mono", size)
                d.text((x, y), m.group(), font=fnt, fill=INK[kind])
                x += d.textlength(m.group(), font=fnt)
                pos = m.end()
            d.text((x, y), line[pos:], font=font("mono", size), fill=INK["text"])
        else:
            colour = line_colour(line)
            bold = colour in (INK["head"], INK["money"], INK["ok"])
            d.text((x0, y), line, font=font("mono_bold" if bold else "mono", size), fill=colour)
        y += step


def draw_chart(d, box, counts):
    items = sorted(counts.items(), key=lambda kv: -kv[1]["value"])
    peak = max(v["value"] for _, v in items)
    rows = len(items)
    step = (box[3] - box[1]) / rows
    label_w, value_w = 430, 270
    d.text((box[0], box[1] - 6), "", font=font("mono", 10))
    for r, (name, v) in enumerate(items):
        y = box[1] + r * step
        d.text((box[0], y + 6), name, font=font("mono_bold", 27), fill=INK["text"])
        x1 = box[0] + label_w
        span = box[2] - value_w - x1
        d.rounded_rectangle((x1, y + 8, x1 + span, y + step - 14), radius=10, fill="#21262d")
        grad = gradient((max(12, int(span * v["value"] / peak)), int(step - 22)), "#22d3ee", "#a78bfa", diagonal=False)
        mask = Image.new("L", grad.size, 0)
        ImageDraw.Draw(mask).rounded_rectangle((0, 0, grad.size[0] - 1, grad.size[1] - 1), radius=10, fill=255)
        d._image.paste(grad, (int(x1), int(y + 8)), mask)
        txt = f"{v['value'] / 100:>12,.0f}  ({v['count']})"
        d.text((box[2] - value_w + 16, y + 6), txt, font=font("mono", 26), fill=INK["money"])


def definitions():
    data = json.loads(read("reports/catalog.json"))
    counts = {}
    for p in data["products"]:
        c = counts.setdefault(p["category"], {"count": 0, "value": 0})
        c["count"] += 1
        c["value"] += p["price_paisa"] * p["stock"]
    insights = clean(read("reports/native_insights.txt"))
    i = next(n for n, l in enumerate(insights) if "INVENTORY INSIGHTS" in l)
    j = next(n for n, l in enumerate(insights) if "Estimated purchase cost" in l)
    smoke = clean(read("reports/smoke_tests.txt"))
    return [
        # number, title, category, caption, source, body lines | "CHART" | "BROWSER:*"
        ("Catalog audit summary", "Catalog", "catalog_cli --summary validates the seed and totals products, units and value for every category.",
         "reports/summary.txt", [l for l in clean(read("reports/summary.txt")) if "stock=" not in l]),
        ("Inventory value by category", "Catalog", "Stock value (price x units) per category from the exported catalog; product counts in brackets.",
         "reports/catalog.json", "CHART"),
        ("Product search", "Catalog", "A case-insensitive search for 'oil' finds matches across several categories with full names, prices and stock.",
         "reports/search.txt", clean(read("reports/search.txt"))),
        ("Electronics sorted by price", "Catalog", "Category filter plus ascending price sort, with a deterministic name tie-breaker.",
         "reports/price_sort.txt", clean(read("reports/price_sort.txt"))[4:]),
        ("Low-stock review", "Catalog", "Products at or below the low-stock threshold (default 5), lowest stock first.",
         "reports/low_stock.txt", clean(read("reports/low_stock.txt"))[4:30]),
        ("Price statistics per category", "Analytics", "New --stats view: min, median, mean and max unit price for each category and overall.",
         "reports/price_stats.txt", clean(read("reports/price_stats.txt"))[4:]),
        ("Restock plan with cost", "Analytics", "New --restock view: units to order and estimated cost to bring Electronics up to 8 units each.",
         "reports/restock_plan.txt", clean(read("reports/restock_plan.txt"))[4:]),
        ("Discounted price quote", "Analytics", "New --quote view: a stock-checked basket with 10% off applied before 5% tax and 5% delivery.",
         "reports/quote.txt", clean(read("reports/quote.txt"))),
        ("CSV export of top stock value", "Analytics", "--sort value --desc --limit 12 --csv exports the most valuable stock lines for spreadsheets.",
         "reports/top_value.csv", clean(read("reports/top_value.csv"))),
        ("Native role selection", "Native", "The Windows console portal opens with Admin, Vendor and Customer entry points.",
         "reports/native_welcome.txt", excerpt("reports/native_welcome.txt", "=====", "Enter Your Choice", 30)),
        ("Admin dashboard", "Native", "Admin login reaches the dashboard, now with the Inventory Insights option.",
         "reports/native_admin.txt", excerpt("reports/native_admin.txt", "ADMIN LOGIN SUCCESSFUL", "Enter choice", 30)),
        ("Admin inventory insights", "Native", "New native report: category medians and values, low-stock alerts and a restock estimate.",
         "reports/native_insights.txt", [l for l in insights[i : j + 1] if not l.startswith("  ") or "####" in l]),
        ("Vendor product ownership", "Native", "A vendor sees only the products it owns, with price and stock.",
         "reports/native_vendor.txt", excerpt("reports/native_vendor.txt", "VENDOR LOGIN SUCCESSFUL", None, 40)[:4] + [""] + excerpt("reports/native_vendor.txt", "MY PRODUCTS", None, 4)),
        ("Browsing the grocery catalog", "Native", "Customers browse a category with live prices and stock read from the data files.",
         "reports/native_browse.txt", excerpt("reports/native_browse.txt", "####  Groceries  ####", None, 18)),
        ("Merged cart and line totals", "Native", "Adding Apples twice merges into one line (qty 3); Milk is a second line with its own total.",
         "reports/native_cart.txt", excerpt("reports/native_cart.txt", "YOUR CART", "Milk", 14)),
        ("Confirmed checkout bill", "Native", "2 Apples: PKR 300.00 subtotal + 15.00 tax + 15.00 delivery = PKR 330.00, then the order is saved.",
         "reports/native_checkout.txt", excerpt("reports/native_checkout.txt", "CHECKOUT & BILL", "ORDER PLACED", 30)),
        ("Saved order and updated stock", "Native", "The order log gains the new order and Apples stock drops from 20 to 18.",
         "reports/checkout_orders.txt", ["orders.txt"] + ["-" * 60] + clean(read("reports/checkout_orders.txt"))[-3:] + ["", "Groceries.txt (first lines)", "-" * 60] + clean(read("reports/checkout_stock.txt"))[:6]),
        ("Persistent wishlist", "Native", "Wishlist items are saved per customer and survive logout.",
         "reports/native_wishlist.txt", excerpt("reports/native_wishlist.txt", "YOUR WISHLIST", "Headphones", 10)),
        ("Domain correctness checks", "Tests", "17 checks: money parsing, rounding, malformed rows, unsafe names and rollback cleanup.",
         "reports/domain_tests.txt", clean(read("reports/domain_tests.txt"))),
        ("Analytics test suite", "Tests", "30 new checks for statistics, filters, sorting, restock plans, quotes, discounts and CSV.",
         "reports/analytics_tests.txt", clean(read("reports/analytics_tests.txt"))[:19] + ["...  11 more PASS lines in the report"] + clean(read("reports/analytics_tests.txt"))[-1:]),
        ("Native cart and CRUD checks", "Tests", "Unit checks for cart merging, stock rules and category-gated product operations.",
         "reports/console_tests.txt", [l for l in clean(read("reports/console_tests.txt")) if l.startswith("PASS")][:19] + ["...  full list in reports/console_tests.txt"]),
        ("Integration scenarios", "Tests", "Nine piped-input scenarios run the real executable in temporary fixtures.",
         "reports/smoke_tests.txt", [re.sub(r"\s*\(.*\)$", "", l) for l in smoke]),
        ("CMake and CTest run", "Tests", "All CTest targets pass, including CLI quote, statistics and overselling checks.",
         "reports/ctest.txt", clean(read("reports/ctest.txt"))),
        ("Complete-record parsing", "Source", "Parsing uses the final separators, so names like T-shirts survive, and validates every field.",
         "include/Commerce.h", code("include/Commerce.h", "inline Product parseProduct", 18)),
        ("Exact PKR charge calculation", "Source", "Integer-paisa arithmetic: tax and delivery are rounded half up without overflow.",
         "include/Commerce.h", code("include/Commerce.h", "inline Totals calculate", 16)),
        ("Rollback-capable file commit", "Source", "Checkout stages every file, keeps backups and restores them if any step fails.",
         "include/Commerce.h", code("include/Commerce.h", "    catch (...)", 18)),
        ("Quote builder with stock checks", "Source", "Analytics.h merges repeated items, rejects unknown or ambiguous names and checks stock.",
         "include/Analytics.h", code("include/Analytics.h", "        const auto colon = part.rfind", 19)),
        ("Inventory insights source", "Source", "The admin report reuses the analytics layer and never writes to data files.",
         "src/Admin.cpp", code("src/Admin.cpp", "    const int threshold = 5, target = 10;", 19)),
        ("Interactive shopping sandbox", "Browser", "The webpage loads the C++ catalog export: search, filter, sort and add items to the cart.",
         "index.html  (browser capture)", "BROWSER:catalog"),
        ("Simulated receipt with discount", "Browser", "The ROHAAN10 code takes 10% off before tax and delivery, matching the CLI quote rules.",
         "index.html  (browser capture)", "BROWSER:receipt"),
    ], counts


def capture_browser():
    """Capture the two sandbox screenshots with Playwright from a local HTTP server."""
    from playwright.sync_api import sync_playwright

    class Quiet(http.server.SimpleHTTPRequestHandler):
        def log_message(self, *args):
            pass

    handler = functools.partial(Quiet, directory=str(ROOT))
    server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), handler)
    threading.Thread(target=server.serve_forever, daemon=True).start()
    url = f"http://127.0.0.1:{server.server_port}/index.html#demo"
    shots = {}
    try:
        with sync_playwright() as p:
            try:
                browser = p.chromium.launch()
            except Exception:  # fall back to an installed Chrome / Edge
                try:
                    browser = p.chromium.launch(channel="chrome")
                except Exception:
                    browser = p.chromium.launch(channel="msedge")
            page = browser.new_page(viewport={"width": 1120, "height": 700}, device_scale_factor=2)
            page.add_init_script("localStorage.setItem('shop-theme', JSON.stringify('light'))")
            page.goto(url)
            page.add_style_tag(content=".nav{position:static!important}.to-top{display:none!important}")
            page.wait_for_selector("#products .product")
            page.fill("#productSearch", "a")
            page.select_option("#productCategory", "Groceries")
            page.wait_for_timeout(200)
            page.evaluate("document.querySelector('#demo').scrollIntoView()")
            page.locator("#products .product").nth(0).locator("input").fill("3")
            page.locator("#products .product").nth(0).locator("button").click()
            page.locator("#products .product").nth(1).locator("button").click()
            page.wait_for_timeout(200)
            top = page.evaluate("document.querySelector('#productCount').getBoundingClientRect().top + scrollY")
            page.evaluate(f"window.scrollTo(0, {top} - 16)")
            page.wait_for_timeout(300)
            shots["catalog"] = page.screenshot(clip={"x": 0, "y": 0, "width": 1120, "height": 700})
            page.fill("#coupon", "ROHAAN10")
            page.click("#couponForm button")
            page.click("#checkout")
            page.wait_for_timeout(300)
            r = page.evaluate("(() => { const h = document.querySelector('#receipt').previousElementSibling.getBoundingClientRect(); const p = document.querySelector('#receipt').getBoundingClientRect(); return {x: p.left, y: h.top + scrollY, w: p.width, b: p.bottom + scrollY}; })()")
            width = r["w"] + 24
            height = width / 1.6
            shots["receipt"] = page.screenshot(full_page=True, clip={"x": r["x"] - 12, "y": r["b"] + 12 - height, "width": width, "height": height})
            browser.close()
    finally:
        server.shutdown()
    return shots


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    defs, counts = definitions()
    shots = capture_browser() if "--browser" in sys.argv else {}
    manifest = []
    for n, (title, category, caption, source, body) in enumerate(defs, 1):
        number = f"{n:02d}"
        slug = number + "_" + re.sub(r"[^a-z0-9]+", "_", title.lower()).strip("_") + ".png"
        manifest.append(dict(number=number, title=title, category=category, caption=caption, source=source, image=f"assets/screenshots/{slug}"))
        if isinstance(body, str) and body.startswith("BROWSER:"):
            key = body.split(":")[1]
            if key not in shots:
                if not (OUT / slug).exists():
                    print(f"warning: {slug} missing; run with --browser")
                continue
            import io
            im, d, box = frame(number, title, category, "http://127.0.0.1:8080/#demo")
            shot = Image.open(io.BytesIO(shots[key])).convert("RGB")
            inner = (box[0] - 38, box[1] - 24, box[2] + 38, box[3] + 26)
            target = (inner[2] - inner[0], inner[3] - inner[1])
            scale = min(target[0] / shot.width, target[1] / shot.height)
            fitted = shot.resize((round(shot.width * scale), round(shot.height * scale)), Image.Resampling.LANCZOS)
            shot = Image.new("RGB", target, shot.getpixel((2, 2)))
            shot.paste(fitted, ((target[0] - fitted.width) // 2, (target[1] - fitted.height) // 2))
            mask = Image.new("L", target, 0)
            ImageDraw.Draw(mask).rounded_rectangle((0, 0, target[0] - 1, target[1] - 1), radius=14, fill=255)
            im.paste(shot, inner[:2], mask)
        else:
            im, d, box = frame(number, title, category, source)
            if body == "CHART":
                draw_chart(d, box, counts)
            else:
                draw_text(d, body, box, is_code=category == "Source")
        im.save(OUT / slug, optimize=True)
    (OUT / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"{len(manifest)} screenshots in {OUT.relative_to(ROOT)}")
    sys.dont_write_bytecode = True  # no tools/__pycache__ in the repo
    import build_site_data  # keep data.js and the static gallery in sync
    build_site_data.main()


if __name__ == "__main__":
    main()
