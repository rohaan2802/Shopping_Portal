#pragma once
// Shared, standard-library-only domain logic: exact money, catalog parsing and
// rollback-capable file commits. The application is single-process, not a DB.
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace shop
{
inline std::string trim(std::string value)
{
    const auto start = value.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return {};
    return value.substr(start, value.find_last_not_of(" \t\r\n") - start + 1);
}
inline std::string lower(std::string value)
{
    for (char &c : value)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return value;
}
inline bool safeField(const std::string &value)
{
    return !trim(value).empty() && value.size() <= 100 &&
           value.find_first_of("|\r\n;:") == std::string::npos;
}
inline bool safeCategory(const std::string &value)
{
    if (!safeField(value) || value.size() > 60)
        return false;
    for (unsigned char c : value)
        if (!(std::isalnum(c) || c == ' ' || c == '_' || c == '-'))
            return false;
    const auto name = lower(trim(value));
    if (name == "con" || name == "prn" || name == "aux" || name == "nul")
        return false;
    if (name.size() == 4 &&
        (name.substr(0, 3) == "com" || name.substr(0, 3) == "lpt") && name[3] >= '1' &&
        name[3] <= '9')
        return false;
    return true;
}
using Cents = std::int64_t;
inline Cents money(std::string value)
{
    value = trim(value);
    if (lower(value).size() >= 3 && lower(value).substr(value.size() - 3) == "pkr")
        value = trim(value.substr(0, value.size() - 3));
    if (lower(value).rfind("rs.", 0) == 0)
        value = trim(value.substr(3));
    if (!std::regex_match(value, std::regex("[0-9]+(\\.[0-9]{1,2})?")))
        throw std::invalid_argument(
            "Price must be a nonnegative PKR amount with at most two decimals");
    auto dot = value.find('.');
    const auto whole = std::stoll(value.substr(0, dot));
    if (whole > 1000000000LL)
        throw std::invalid_argument("Price exceeds the supported limit");
    std::string fraction = dot == std::string::npos ? "00" : value.substr(dot + 1);
    if (fraction.size() == 1)
        fraction += '0';
    return whole * 100 + std::stoll(fraction);
}
inline std::string formatMoney(Cents value)
{
    std::ostringstream out;
    out << value / 100 << '.' << std::setw(2) << std::setfill('0') << value % 100;
    return out.str();
}
struct Totals
{
    Cents subtotal{}, tax{}, delivery{}, total{};
};
inline Totals calculate(Cents subtotal, int taxBasisPoints = 500,
                        int deliveryBasisPoints = 500)
{
    if (subtotal < 0 || subtotal > 1000000000000000LL || taxBasisPoints < 0 ||
        taxBasisPoints > 10000 || deliveryBasisPoints < 0 ||
        deliveryBasisPoints > 10000)
        throw std::invalid_argument("Invalid bill amount or rate");
    Totals t;
    t.subtotal = subtotal;
    t.tax = (subtotal / 10000) * taxBasisPoints +
            ((subtotal % 10000) * taxBasisPoints + 5000) / 10000;
    t.delivery = (subtotal / 10000) * deliveryBasisPoints +
                 ((subtotal % 10000) * deliveryBasisPoints + 5000) / 10000;
    t.total = t.subtotal + t.tax + t.delivery;
    return t;
}
struct Product
{
    std::string category, name, priceLabel;
    Cents price{};
    int quantity{};
};
inline Product parseProduct(const std::string &line, const std::string &category = {})
{
    const auto comma = line.rfind(',');
    const auto dash = line.rfind(" - ");
    if (comma == std::string::npos || dash == std::string::npos || dash >= comma)
        throw std::invalid_argument(
            "Malformed product line: expected Name - price PKR, quantity");
    Product p;
    p.category = category;
    p.name = trim(line.substr(0, dash));
    p.priceLabel = trim(line.substr(dash + 3, comma - dash - 3));
    p.price = money(p.priceLabel);
    const auto qty = trim(line.substr(comma + 1));
    if (!safeField(p.name) || !std::regex_match(qty, std::regex("[0-9]{1,9}")))
        throw std::invalid_argument("Invalid product name or quantity");
    p.quantity = std::stoi(qty);
    return p;
}
inline bool readRecord(std::istream &input, std::string &name, std::string &price,
                       std::string &quantity)
{
    std::string line;
    while (std::getline(input, line))
    {
        if (trim(line).empty())
            continue;
        auto product = parseProduct(line);
        name = product.name;
        price = product.priceLabel;
        quantity = std::to_string(product.quantity);
        return true;
    }
    return false;
}
inline std::string readFile(const std::filesystem::path &path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
        throw std::runtime_error("Cannot read " + path.filename().string());
    std::ostringstream text;
    text << in.rdbuf();
    return text.str();
}
inline std::vector<Product> loadCatalog(const std::filesystem::path &folder)
{
    std::ifstream index(folder / "ItemsCategory.txt");
    if (!index)
        throw std::runtime_error(
            "ItemsCategory.txt missing in the selected data directory");
    std::vector<Product> products;
    std::map<std::string, bool> categories;
    std::string category;
    while (std::getline(index, category))
    {
        category = trim(category);
        if (category.empty())
            continue;
        if (!safeCategory(category) || categories.count(lower(category)))
            throw std::invalid_argument("Invalid or duplicate category");
        categories[lower(category)] = true;
        std::ifstream input(folder / (category + ".txt"));
        if (!input)
            throw std::runtime_error("Missing category file: " + category);
        std::map<std::string, bool> names;
        std::string line;
        while (std::getline(input, line))
        {
            if (trim(line).empty())
                continue;
            auto p = parseProduct(line, category);
            if (names.count(lower(p.name)))
                throw std::invalid_argument("Duplicate product in " + category + ": " +
                                            p.name);
            names[lower(p.name)] = true;
            products.push_back(p);
        }
    }
    return products;
}
inline void commitFiles(const std::map<std::filesystem::path, std::string> &updates)
{
    namespace fs = std::filesystem;
    std::vector<fs::path> prepared, backed, installed;
    try
    {
        for (const auto &entry : updates)
        {
            const auto temp = fs::path(entry.first.string() + ".shop.tmp");
            const auto backup = fs::path(entry.first.string() + ".shop.bak");
            if (fs::exists(temp) || fs::exists(backup))
                throw std::runtime_error("Pending transaction files require recovery");
            prepared.push_back(entry.first);
            std::ofstream out(temp, std::ios::binary);
            if (!out)
                throw std::runtime_error("Cannot stage file update");
            out << entry.second;
            out.flush();
            if (!out)
                throw std::runtime_error("Could not flush staged update");
        }
        for (const auto &entry : updates)
        {
            if (fs::exists(entry.first))
            {
                fs::rename(entry.first, entry.first.string() + ".shop.bak");
                backed.push_back(entry.first);
            }
            fs::rename(entry.first.string() + ".shop.tmp", entry.first);
            installed.push_back(entry.first);
        }
    }
    catch (...)
    {
        for (const auto &path : installed)
        {
            std::error_code ec;
            fs::remove(path, ec);
        }
        for (auto it = backed.rbegin(); it != backed.rend(); ++it)
        {
            std::error_code ec;
            fs::rename(it->string() + ".shop.bak", *it, ec);
        }
        for (const auto &path : prepared)
        {
            std::error_code ec;
            fs::remove(path.string() + ".shop.tmp", ec);
        }
        throw;
    }
    for (const auto &path : backed)
    {
        std::error_code ec;
        fs::remove(path.string() + ".shop.bak", ec);
    }
}
} // namespace shop
