#pragma once
// Inventory analytics built on the shared Commerce.h domain model.
// Standard library only: used by the portable catalog CLI, the Windows admin
// dashboard and the analytics test suite. Every function is read-only.
#include "Commerce.h"
#include <numeric>

namespace shop
{
namespace analytics
{
// ---------------------------------------------------------------- statistics
struct PriceStats
{
    std::string category;
    int products{};
    std::int64_t units{};
    Cents minPrice{}, maxPrice{}, meanPrice{}, medianPrice{}, inventoryValue{};
};

// Integer division rounded half up; inputs are non-negative paisa amounts.
inline Cents roundedDivide(Cents numerator, std::int64_t denominator)
{
    if (denominator <= 0)
        throw std::invalid_argument("Division by a non-positive count");
    return (numerator + denominator / 2) / denominator;
}

inline PriceStats describe(const std::string &label, std::vector<Product> items)
{
    PriceStats s;
    s.category = label;
    s.products = static_cast<int>(items.size());
    if (items.empty())
        return s;
    std::sort(items.begin(), items.end(),
              [](const Product &a, const Product &b) { return a.price < b.price; });
    Cents sum = 0;
    for (const auto &p : items)
    {
        sum += p.price;
        s.units += p.quantity;
        s.inventoryValue += p.price * p.quantity;
    }
    s.minPrice = items.front().price;
    s.maxPrice = items.back().price;
    s.meanPrice = roundedDivide(sum, static_cast<std::int64_t>(items.size()));
    const auto mid = items.size() / 2;
    s.medianPrice = items.size() % 2 ? items[mid].price
                                     : roundedDivide(items[mid - 1].price + items[mid].price, 2);
    return s;
}

// One row per category (alphabetical) followed by an "All categories" row.
inline std::vector<PriceStats> priceStatistics(const std::vector<Product> &catalog)
{
    std::map<std::string, std::vector<Product>> grouped;
    for (const auto &p : catalog)
        grouped[p.category].push_back(p);
    std::vector<PriceStats> rows;
    for (const auto &[name, items] : grouped)
        rows.push_back(describe(name, items));
    rows.push_back(describe("All categories", catalog));
    return rows;
}

// ------------------------------------------------------------------ filters
struct Filter
{
    std::string query;    // case-insensitive substring of the product name
    std::string category; // case-insensitive exact category
    Cents minPrice = -1;  // inclusive; -1 disables
    Cents maxPrice = -1;  // inclusive; -1 disables
    int maxStock = -1;    // inclusive; -1 disables (low-stock view)
};

inline bool matches(const Product &p, const Filter &f)
{
    if (!f.query.empty() && lower(p.name).find(lower(f.query)) == std::string::npos)
        return false;
    if (!f.category.empty() && lower(p.category) != lower(f.category))
        return false;
    if (f.minPrice >= 0 && p.price < f.minPrice)
        return false;
    if (f.maxPrice >= 0 && p.price > f.maxPrice)
        return false;
    if (f.maxStock >= 0 && p.quantity > f.maxStock)
        return false;
    return true;
}

inline std::vector<Product> select(const std::vector<Product> &catalog, const Filter &f)
{
    if (f.minPrice >= 0 && f.maxPrice >= 0 && f.minPrice > f.maxPrice)
        throw std::invalid_argument("Minimum price is greater than maximum price");
    std::vector<Product> out;
    std::copy_if(catalog.begin(), catalog.end(), std::back_inserter(out),
                 [&](const Product &p) { return matches(p, f); });
    return out;
}

// Deterministic sorting with a name tie-breaker so reports never reorder.
inline void sortProducts(std::vector<Product> &items, const std::string &key,
                         bool descending = false)
{
    if (key != "name" && key != "price" && key != "stock" && key != "value")
        throw std::invalid_argument("Unsupported sort: use name, price, stock or value");
    std::stable_sort(items.begin(), items.end(), [&](const Product &a, const Product &b) {
        auto keyOf = [&](const Product &p) -> std::int64_t {
            if (key == "price")
                return p.price;
            if (key == "stock")
                return p.quantity;
            return p.price * p.quantity;
        };
        if (key != "name" && keyOf(a) != keyOf(b))
            return descending ? keyOf(a) > keyOf(b) : keyOf(a) < keyOf(b);
        const auto an = lower(a.name), bn = lower(b.name);
        if (key == "name" && descending)
            return an > bn;
        return an < bn;
    });
}

// ------------------------------------------------------------ restock plan
struct RestockLine
{
    Product product;
    int reorderUnits{};
    Cents reorderCost{};
};
struct RestockPlan
{
    int target{};
    std::vector<RestockLine> lines;
    std::int64_t units{};
    Cents cost{};
};

// Bring every product below `target` units back up to `target`.
// Cost uses the catalog unit price as a conservative purchasing estimate.
inline RestockPlan restockPlan(const std::vector<Product> &catalog, int target)
{
    if (target < 1 || target > 100000)
        throw std::invalid_argument("Restock target must be between 1 and 100000");
    RestockPlan plan;
    plan.target = target;
    for (const auto &p : catalog)
        if (p.quantity < target)
        {
            RestockLine line{p, target - p.quantity, 0};
            line.reorderCost = p.price * line.reorderUnits;
            plan.units += line.reorderUnits;
            plan.cost += line.reorderCost;
            plan.lines.push_back(line);
        }
    std::stable_sort(plan.lines.begin(), plan.lines.end(),
                     [](const RestockLine &a, const RestockLine &b) {
                         if (a.product.quantity != b.product.quantity)
                             return a.product.quantity < b.product.quantity;
                         return lower(a.product.name) < lower(b.product.name);
                     });
    return plan;
}

// ------------------------------------------------------------ price quotes
struct QuoteLine
{
    Product product;
    int quantity{};
    Cents lineTotal{};
};
struct Quote
{
    std::vector<QuoteLine> lines;
    Cents gross{};          // before discount
    int discountBasisPoints{};
    Cents discount{};       // rounded half up
    Totals totals;          // subtotal after discount, plus tax and delivery
};

inline int parseQuantity(const std::string &text)
{
    const auto t = trim(text);
    if (!std::regex_match(t, std::regex("[0-9]{1,6}")) || std::stoi(t) < 1)
        throw std::invalid_argument("Quantity must be a whole number from 1 to 999999");
    return std::stoi(t);
}

// Percent with up to two decimals, e.g. "10" or "12.5" -> basis points.
inline int parsePercent(const std::string &text)
{
    const auto t = trim(text);
    if (!std::regex_match(t, std::regex("[0-9]{1,3}(\\.[0-9]{1,2})?")))
        throw std::invalid_argument("Discount must be a percentage such as 10 or 12.5");
    const auto bp = static_cast<int>(money(t)); // reuse exact two-decimal parsing
    if (bp > 5000)
        throw std::invalid_argument("Discount cannot exceed 50%");
    return bp;
}

// Spec: "Apples:2,Milk:1". Each name must match exactly one product
// (case-insensitive); quantities are merged and checked against stock.
inline Quote buildQuote(const std::vector<Product> &catalog, const std::string &spec,
                        int discountBasisPoints = 0)
{
    if (discountBasisPoints < 0 || discountBasisPoints > 5000)
        throw std::invalid_argument("Discount cannot exceed 50%");
    Quote q;
    q.discountBasisPoints = discountBasisPoints;
    std::stringstream items(spec);
    std::string part;
    while (std::getline(items, part, ','))
    {
        if (trim(part).empty())
            continue;
        const auto colon = part.rfind(':');
        if (colon == std::string::npos)
            throw std::invalid_argument("Quote items use Name:quantity, got '" + trim(part) + "'");
        const auto name = trim(part.substr(0, colon));
        const int qty = parseQuantity(part.substr(colon + 1));
        std::vector<const Product *> hits;
        for (const auto &p : catalog)
            if (lower(p.name) == lower(name))
                hits.push_back(&p);
        if (hits.empty())
            throw std::invalid_argument("Unknown product: " + name);
        if (hits.size() > 1)
            throw std::invalid_argument("Ambiguous product name across categories: " + name);
        std::size_t index = 0;
        while (index < q.lines.size() && lower(q.lines[index].product.name) != lower(name))
            ++index;
        if (index == q.lines.size())
            q.lines.push_back({*hits.front(), 0, 0});
        auto &line = q.lines[index];
        line.quantity += qty;
        if (line.quantity > line.product.quantity)
            throw std::invalid_argument("Insufficient stock for " + line.product.name +
                                        ": requested " + std::to_string(line.quantity) +
                                        ", available " + std::to_string(line.product.quantity));
    }
    if (q.lines.empty())
        throw std::invalid_argument("Quote needs at least one Name:quantity item");
    for (auto &line : q.lines)
    {
        line.lineTotal = line.product.price * line.quantity;
        q.gross += line.lineTotal;
    }
    q.discount = (q.gross / 10000) * discountBasisPoints +
                 ((q.gross % 10000) * discountBasisPoints + 5000) / 10000;
    q.totals = calculate(q.gross - q.discount);
    return q;
}

// ---------------------------------------------------------------- exports
inline std::string csvField(const std::string &value)
{
    const bool quote = value.find_first_of(",\"\r\n") != std::string::npos;
    std::string out = quote ? "\"" : "";
    for (char c : value)
    {
        if (c == '"')
            out += '"';
        out += c;
    }
    return quote ? out + '"' : out;
}

inline std::string toCsv(const std::vector<Product> &items)
{
    std::ostringstream out;
    out << "category,name,price_pkr,stock,stock_value_pkr\n";
    for (const auto &p : items)
        out << csvField(p.category) << ',' << csvField(p.name) << ',' << formatMoney(p.price)
            << ',' << p.quantity << ',' << formatMoney(p.price * p.quantity) << '\n';
    return out.str();
}

inline std::string withThousands(std::int64_t value)
{
    std::string digits = std::to_string(value < 0 ? -value : value), out;
    for (std::size_t i = 0; i < digits.size(); ++i)
    {
        if (i && (digits.size() - i) % 3 == 0)
            out += ',';
        out += digits[i];
    }
    return value < 0 ? "-" + out : out;
}

// "5975360.00" -> "5,975,360.00" for human-readable reports.
inline std::string formatMoneyGrouped(Cents value)
{
    return withThousands(value / 100) + formatMoney(value).substr(formatMoney(value).find('.'));
}
} // namespace analytics
} // namespace shop
