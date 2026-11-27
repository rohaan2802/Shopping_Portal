// Analytics suite: statistics, filters, restock planning, quotes and exports.
// Uses an in-memory catalog plus the maintained sample seed (path from argv).
#include "Analytics.h"
#include <iostream>

namespace
{
int passed = 0;
void check(bool ok, const char *label)
{
    if (!ok)
        throw std::runtime_error(std::string("FAILED ") + label);
    std::cout << "PASS " << label << '\n';
    ++passed;
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
shop::Product item(const std::string &category, const std::string &name, shop::Cents price,
                   int qty)
{
    shop::Product p;
    p.category = category;
    p.name = name;
    p.price = price;
    p.priceLabel = shop::formatMoney(price);
    p.quantity = qty;
    return p;
}
} // namespace

int main(int argc, char **argv)
{
    using namespace shop::analytics;
    try
    {
        const std::vector<shop::Product> demo = {
            item("Groceries", "Apples", 15000, 20), item("Groceries", "Milk", 18000, 4),
            item("Groceries", "Bread", 10000, 2),   item("Electronics", "Mouse", 200000, 15),
            item("Electronics", "Webcam", 600000, 8),
        };

        auto stats = priceStatistics(demo);
        check(stats.size() == 3 && stats.back().category == "All categories",
              "statistics include one row per category plus a total row");
        check(stats[1].category == "Groceries" && stats[1].medianPrice == 15000 &&
                  stats[1].minPrice == 10000 && stats[1].maxPrice == 18000,
              "odd-sized median, min and max are exact");
        check(stats[0].medianPrice == 400000, "even-sized median averages the middle pair");
        check(stats[1].meanPrice == 14333, "mean price rounds half up in paisa");
        check(stats.back().inventoryValue == 15000 * 20 + 18000 * 4 + 10000 * 2 +
                                                 200000 * 15 + 600000 * 8,
              "inventory value equals sum of price x stock");

        Filter f;
        f.minPrice = 10000;
        f.maxPrice = 18000;
        check(select(demo, f).size() == 3, "inclusive price-range filter");
        f = {};
        f.maxStock = 5;
        check(select(demo, f).size() == 2, "configurable low-stock threshold");
        f = {};
        f.query = "APP";
        f.category = "groceries";
        check(select(demo, f).size() == 1, "case-insensitive search and category filter");
        rejects(
            [&] {
                Filter bad;
                bad.minPrice = 500;
                bad.maxPrice = 100;
                select(demo, bad);
            },
            "inverted price range rejected");

        auto sorted = demo;
        sortProducts(sorted, "value", true);
        check(sorted.front().name == "Webcam" && sorted.back().name == "Bread",
              "descending stock-value sort");
        sortProducts(sorted, "name");
        check(sorted.front().name == "Apples", "alphabetical sort is case-insensitive");
        rejects([&] { sortProducts(sorted, "colour"); }, "unknown sort key rejected");

        auto plan = restockPlan(demo, 10);
        check(plan.lines.size() == 3 && plan.lines.front().product.name == "Bread" &&
                  plan.units == 8 + 6 + 2,
              "restock plan targets only products below target, lowest stock first");
        check(plan.cost == 10000 * 8 + 18000 * 6 + 600000 * 2, "restock cost uses unit prices");
        rejects([&] { restockPlan(demo, 0); }, "restock target must be positive");

        auto q = buildQuote(demo, "Apples:1, apples:1");
        check(q.lines.size() == 1 && q.lines[0].quantity == 2, "repeated quote items merge");
        check(q.totals.subtotal == 30000 && q.totals.tax == 1500 && q.totals.total == 33000,
              "two Apples quote matches the native PKR 330.00 checkout");
        auto discounted = buildQuote(demo, "Apples:3,Milk:2", parsePercent("10"));
        check(discounted.gross == 81000 && discounted.discount == 8100 &&
                  discounted.totals.total == 80190,
              "10% discount is applied before tax and delivery");
        check(parsePercent("12.5") == 1250, "fractional discount percent parsed exactly");
        rejects([&] { parsePercent("75"); }, "discount above 50% rejected");
        rejects([&] { buildQuote(demo, "Apples:21"); }, "quote beyond available stock rejected");
        rejects([&] { buildQuote(demo, "Pears:1"); }, "unknown quote product rejected");
        rejects([&] { buildQuote(demo, "Apples:0"); }, "zero quantity rejected");
        rejects([&] { buildQuote(demo, "Apples"); }, "quote item without quantity rejected");
        auto ambiguous = demo;
        ambiguous.push_back(item("Electronics", "Apples", 1, 1));
        rejects([&] { buildQuote(ambiguous, "Apples:1"); },
                "ambiguous names across categories rejected");

        check(csvField("Pens, blue") == "\"Pens, blue\"" && csvField("6\" ruler") == "\"6\"\" ruler\"",
              "CSV fields quote commas and escape quotes");
        check(toCsv({demo[0]}) ==
                  "category,name,price_pkr,stock,stock_value_pkr\nGroceries,Apples,150.00,20,3000.00\n",
              "CSV export has a header and exact money columns");
        check(formatMoneyGrouped(597536000) == "5,975,360.00" && withThousands(4651) == "4,651",
              "thousands separators for reports");

        if (argc > 1)
        {
            const auto seed = shop::loadCatalog(argv[1]);
            const auto all = priceStatistics(seed).back();
            check(all.products == 351 && all.units == 4651, "sample seed statistics: 351 products, 4,651 units");
            check(buildQuote(seed, "Apples:2").totals.total == 33000,
                  "sample seed quote: 2 Apples = PKR 330.00");
        }
        std::cout << passed << " analytics checks passed\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
