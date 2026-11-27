#include "Commerce.h"
#include <iostream>
#include <chrono>
void check(bool ok, const char *label)
{
    if (!ok)
        throw std::runtime_error(label);
    std::cout << "PASS " << label << '\n';
}
template <class F> void rejects(F action, const char *label)
{
    bool caught = false;
    try
    {
        action();
    }
    catch (const std::exception &)
    {
        caught = true;
    }
    check(caught, label);
}
int main()
{
    try
    {
        check(shop::money("150 PKR") == 15000, "integer PKR parsing");
        check(shop::money("Rs. 12.50") == 1250, "fractional price precision");
        rejects([] { shop::money("-5"); }, "negative price rejected");
        rejects([] { shop::money("1.234"); }, "excess decimal precision rejected");
        check(shop::parseProduct("T-shirts - 899.95 PKR, 12").name == "T-shirts",
              "hyphenated product name preserved");
        rejects([] { shop::parseProduct("Apples - 10 PKR, -2"); },
                "negative stock rejected");
        rejects([] { shop::parseProduct("malformed row"); },
                "malformed records rejected");
        check(!shop::safeCategory("../outside") && !shop::safeCategory("CON") &&
                  !shop::safeCategory("bad*name"),
              "category traversal and invalid filenames rejected");
        check(!shop::safeField("name|record"), "record delimiter rejected");
        check(shop::formatMoney(150) == "1.50" && shop::formatMoney(0) == "0.00",
              "money formatting preserves two decimal places");
        const auto bill = shop::calculate(1999);
        check(bill.tax == 100 && bill.delivery == 100 && bill.total == 2199,
              "independent half-up charge rounding");
        check(shop::calculate(1000000000000000LL, 10000, 10000).total ==
                  3000000000000000LL,
              "large supported bill avoids overflow");
        rejects([] { shop::calculate(-1); }, "negative subtotal rejected");
        auto dir = std::filesystem::temp_directory_path() /
                   ("shopping_domain_" +
                    std::to_string(
                        std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directories(dir);
        struct Cleanup
        {
            std::filesystem::path p;
            ~Cleanup()
            {
                std::error_code e;
                std::filesystem::remove_all(p, e);
            }
        } cleanup{dir};
        auto a = dir / "a.txt", b = dir / "b.txt";
        {
            std::ofstream(a) << "before";
            std::ofstream(b) << "order0";
        }
        shop::commitFiles({{a, "after"}, {b, "order1"}});
        check(shop::readFile(a) == "after" && shop::readFile(b) == "order1",
              "multi-file commit saves both records");
        std::ofstream(b.string() + ".shop.tmp") << "occupied";
        rejects([&] { shop::commitFiles({{a, "bad"}, {b, "bad"}}); },
                "existing staging file blocks transaction");
        check(shop::readFile(a) == "after" && shop::readFile(b) == "order1",
              "failed staging leaves original files unchanged");
        check(!std::filesystem::exists(a.string() + ".shop.tmp"),
              "owned temporary files cleaned after failure");
        std::cout << "17 domain checks passed\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "FAIL " << e.what() << '\n';
        return 1;
    }
}
