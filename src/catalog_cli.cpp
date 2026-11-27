// catalog_cli: portable, read-only inventory audit for Shopping_Portal.
// It validates the same text catalog used by the Windows portal and offers
// search, filters, sorting, statistics, restock planning, price quotes and
// JSON/CSV export. It never writes account, cart, order or stock files.
#include "Analytics.h"
#include <iostream>

namespace
{
using namespace shop;
using namespace shop::analytics;

struct CategorySummary
{
    int products{};
    std::int64_t units{};
    Cents value{};
};

struct Options
{
    std::filesystem::path folder = "data/sample";
    Filter filter;
    std::string sort = "name", quote;
    bool descending = false, asJson = false, asCsv = false, low = false;
    bool summary = false, stats = false, listCategories = false;
    int threshold = 5, restock = 0, discount = 0;
    std::size_t limit = 0;
};

const char *HELP = R"(catalog_cli - read-only Shopping_Portal catalog audit

Usage: catalog_cli [--data DIR] [filters] [view] [output]

Filters
  --search TEXT        Case-insensitive product-name search
  --category NAME      Exact category (case-insensitive)
  --min-price PKR      Inclusive minimum unit price, e.g. 500 or 99.50
  --max-price PKR      Inclusive maximum unit price
  --low-stock          Only products at or below the low-stock threshold
  --threshold N        Low-stock threshold (default 5)

Ordering
  --sort KEY           name | price | stock | value (price x stock)
  --desc               Reverse the sort order
  --limit N            Show at most N matching products

Views
  --summary            Product count, units and value per category
  --stats              Min / median / mean / max price per category
  --categories         List registered categories with product counts
  --restock TARGET     Units and cost to bring products up to TARGET stock
  --quote SPEC         Price a basket, e.g. "Apples:2,Milk:1" (stock-checked)
  --discount PCT       Discount for --quote, 0-50 percent (applied before tax)

Output
  --json               Machine-readable JSON for the matching products
  --csv                CSV export for the matching products
  --help               Show this help

Exit codes: 0 success, 2 invalid input or catalog data.
)";

std::string json(const std::string &value)
{
    std::string out = "\"";
    for (unsigned char c : value)
    {
        if (c == '"' || c == '\\')
        {
            out += '\\';
            out += char(c);
        }
        else if (c == '\n')
            out += "\\n";
        else if (c < 32)
            out += ' ';
        else
            out += char(c);
    }
    return out + '"';
}

int parseCount(const std::string &text, const char *what, int lo, int hi)
{
    const auto t = trim(text);
    if (!std::regex_match(t, std::regex("[0-9]{1,9}")) || std::stoll(t) < lo || std::stoll(t) > hi)
        throw std::invalid_argument(std::string(what) + " must be a whole number from " +
                                    std::to_string(lo) + " to " + std::to_string(hi));
    return std::stoi(t);
}

Options parse(int argc, char **argv, bool &help)
{
    Options o;
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        auto value = [&]() {
            if (++i >= argc)
                throw std::invalid_argument("Missing value for " + arg);
            return std::string(argv[i]);
        };
        if (arg == "--data")
            o.folder = value();
        else if (arg == "--search")
            o.filter.query = value();
        else if (arg == "--category")
            o.filter.category = value();
        else if (arg == "--min-price")
            o.filter.minPrice = money(value());
        else if (arg == "--max-price")
            o.filter.maxPrice = money(value());
        else if (arg == "--low-stock")
            o.low = true;
        else if (arg == "--threshold")
            o.threshold = parseCount(value(), "Threshold", 0, 100000);
        else if (arg == "--sort")
            o.sort = value();
        else if (arg == "--desc")
            o.descending = true;
        else if (arg == "--limit")
            o.limit = static_cast<std::size_t>(parseCount(value(), "Limit", 1, 100000));
        else if (arg == "--summary")
            o.summary = true;
        else if (arg == "--stats")
            o.stats = true;
        else if (arg == "--categories")
            o.listCategories = true;
        else if (arg == "--restock")
            o.restock = parseCount(value(), "Restock target", 1, 100000);
        else if (arg == "--quote")
            o.quote = value();
        else if (arg == "--discount")
            o.discount = parsePercent(value());
        else if (arg == "--json")
            o.asJson = true;
        else if (arg == "--csv")
            o.asCsv = true;
        else if (arg == "--help" || arg == "-h")
            help = true;
        else
            throw std::invalid_argument("Unknown option: " + arg + " (see --help)");
    }
    if (o.asJson && o.asCsv)
        throw std::invalid_argument("Choose either --json or --csv");
    if (o.discount && o.quote.empty())
        throw std::invalid_argument("--discount applies only to --quote");
    if (o.low)
        o.filter.maxStock = o.threshold;
    std::vector<Product> probe;
    sortProducts(probe, o.sort); // validates the sort key before loading data
    return o;
}

void printQuote(const Quote &q)
{
    std::cout << "PRICE QUOTE (read-only, stock not reserved)\n"
              << std::left << std::setw(28) << "Item" << std::right << std::setw(6) << "Qty"
              << std::setw(14) << "Unit PKR" << std::setw(14) << "Line PKR" << "\n"
              << std::string(62, '-') << "\n";
    for (const auto &l : q.lines)
        std::cout << std::left << std::setw(28) << l.product.name.substr(0, 27) << std::right
                  << std::setw(6) << l.quantity << std::setw(14) << formatMoney(l.product.price)
                  << std::setw(14) << formatMoney(l.lineTotal) << "\n";
    std::cout << std::string(62, '-') << "\n";
    auto row = [](const std::string &label, Cents v) {
        std::cout << std::left << std::setw(48) << label << std::right << std::setw(14)
                  << formatMoney(v) << "\n";
    };
    row("Gross", q.gross);
    if (q.discountBasisPoints)
        row("Discount (" + formatMoney(q.discountBasisPoints) + "%)", -q.discount);
    row("Subtotal", q.totals.subtotal);
    row("Tax (5%)", q.totals.tax);
    row("Delivery (5%)", q.totals.delivery);
    row("TOTAL PKR", q.totals.total);
}

void printQuoteJson(const Quote &q)
{
    std::cout << "{\"lines\":[";
    for (std::size_t i = 0; i < q.lines.size(); ++i)
    {
        const auto &l = q.lines[i];
        std::cout << (i ? "," : "") << "{\"name\":" << json(l.product.name)
                  << ",\"category\":" << json(l.product.category)
                  << ",\"quantity\":" << l.quantity << ",\"unit_paisa\":" << l.product.price
                  << ",\"line_paisa\":" << l.lineTotal << '}';
    }
    std::cout << "],\"gross_paisa\":" << q.gross << ",\"discount_basis_points\":"
              << q.discountBasisPoints << ",\"discount_paisa\":" << q.discount
              << ",\"subtotal_paisa\":" << q.totals.subtotal << ",\"tax_paisa\":" << q.totals.tax
              << ",\"delivery_paisa\":" << q.totals.delivery
              << ",\"total_paisa\":" << q.totals.total << "}\n";
}

void printRestock(const RestockPlan &plan)
{
    std::cout << "RESTOCK PLAN (target " << plan.target << " units per product)\n"
              << std::left << std::setw(28) << "Product" << std::setw(28) << "Category"
              << std::right << std::setw(6) << "Now" << std::setw(8) << "Order"
              << std::setw(14) << "Cost PKR" << "\n"
              << std::string(84, '-') << "\n";
    for (const auto &l : plan.lines)
        std::cout << std::left << std::setw(28) << l.product.name.substr(0, 27) << std::setw(28)
                  << l.product.category << std::right << std::setw(6)
                  << l.product.quantity << std::setw(8) << l.reorderUnits << std::setw(14)
                  << formatMoney(l.reorderCost) << "\n";
    std::cout << std::string(84, '-') << "\n"
              << plan.lines.size() << " products | " << withThousands(plan.units)
              << " units to order | estimated cost PKR " << formatMoneyGrouped(plan.cost) << "\n";
}

void printStats(const std::vector<PriceStats> &rows)
{
    std::cout << "PRICE STATISTICS (PKR)\n"
              << std::left << std::setw(28) << "Category" << std::right << std::setw(6) << "Items"
              << std::setw(11) << "Min" << std::setw(11) << "Median" << std::setw(11) << "Mean"
              << std::setw(12) << "Max" << "\n"
              << std::string(79, '-') << "\n";
    for (std::size_t i = 0; i < rows.size(); ++i)
    {
        const auto &r = rows[i];
        if (i + 1 == rows.size())
            std::cout << std::string(79, '-') << "\n";
        std::cout << std::left << std::setw(28) << r.category << std::right
                  << std::setw(6) << r.products << std::setw(11) << formatMoney(r.minPrice)
                  << std::setw(11) << formatMoney(r.medianPrice) << std::setw(11)
                  << formatMoney(r.meanPrice) << std::setw(12) << formatMoney(r.maxPrice) << "\n";
    }
}
} // namespace

int main(int argc, char **argv)
{
    try
    {
        bool help = false;
        const Options o = parse(argc, argv, help);
        if (help)
        {
            std::cout << HELP;
            return 0;
        }
        const auto all = loadCatalog(o.folder);

        if (!o.quote.empty())
        {
            const auto q = buildQuote(all, o.quote, o.discount);
            o.asJson ? printQuoteJson(q) : printQuote(q);
            return 0;
        }

        auto products = select(all, o.filter);
        sortProducts(products, o.sort, o.descending);
        const auto matched = products.size();
        if (o.limit && products.size() > o.limit)
            products.resize(o.limit);

        std::map<std::string, CategorySummary> categories;
        std::int64_t units = 0;
        Cents value = 0;
        for (const auto &p : all)
        {
            auto &entry = categories[p.category];
            entry.products++;
            entry.units += p.quantity;
            entry.value += p.price * p.quantity;
            units += p.quantity;
            value += p.price * p.quantity;
        }

        if (o.asCsv)
        {
            std::cout << toCsv(products);
            return 0;
        }
        if (o.asJson)
        {
            std::cout << "{\"product_count\":" << all.size()
                      << ",\"category_count\":" << categories.size()
                      << ",\"stock_units\":" << units << ",\"inventory_value_paisa\":" << value
                      << ",\"matched\":" << matched << ",\"products\":[";
            for (std::size_t i = 0; i < products.size(); ++i)
            {
                const auto &p = products[i];
                std::cout << (i ? "," : "") << "{\"category\":" << json(p.category)
                          << ",\"name\":" << json(p.name) << ",\"price_paisa\":" << p.price
                          << ",\"stock\":" << p.quantity << '}';
            }
            std::cout << "]}\n";
            return 0;
        }

        std::cout << "SHOPPING PORTAL | CATALOG AUDIT\n"
                  << all.size() << " products | " << categories.size() << " categories | "
                  << units << " stock units\nInventory value: PKR " << formatMoney(value)
                  << "\nMatched: " << matched << "\n\n";
        if (o.listCategories)
        {
            std::cout << "REGISTERED CATEGORIES\n";
            int n = 0;
            for (const auto &[name, entry] : categories)
                std::cout << std::right << std::setw(2) << ++n << ". " << std::left
                          << std::setw(30) << name << std::right << std::setw(4)
                          << entry.products << " products\n";
            return 0;
        }
        if (o.restock)
        {
            printRestock(restockPlan(products, o.restock));
            return 0;
        }
        if (o.stats)
        {
            printStats(priceStatistics(products));
            return 0;
        }
        if (o.summary)
        {
            std::cout << "CATEGORY SUMMARY\n"
                      << std::left << std::setw(30) << "Category" << std::right << std::setw(11)
                      << "Products" << std::setw(11) << "Units" << std::setw(20) << "Value (PKR)"
                      << "\n"
                      << std::string(72, '-') << "\n";
            for (const auto &[name, entry] : categories)
                std::cout << std::left << std::setw(30) << name << std::right << std::setw(11)
                          << entry.products << std::setw(11) << entry.units << std::setw(20)
                          << formatMoney(entry.value) << "\n";
            std::cout << "\n";
        }
        for (const auto &p : products)
            std::cout << std::left << std::setw(31) << p.name << std::setw(30) << p.category
                      << "PKR " << std::setw(12) << formatMoney(p.price) << " stock=" << p.quantity
                      << '\n';
        if (o.limit && matched > products.size())
            std::cout << "... " << matched - products.size() << " more (raise --limit)\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Catalog error: " << e.what() << '\n';
        return 2;
    }
}
