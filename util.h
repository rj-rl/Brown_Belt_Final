#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>
#include <optional>
#include <utility>
#include <charconv>

template <typename It>
class Range {
public:
    Range(It begin, It end) : begin_(begin), end_(end) {}
    It begin() const { return begin_; }
    It end() const { return end_; }

private:
    It begin_;
    It end_;
};


inline void setOutPrecision(size_t n = 6)
{
    std::cout << std::setprecision(n);
}

//======================================= PARSING ==============================================//

std::pair<std::string_view, std::optional<std::string_view>> 
    splitTwoStrict (std::string_view s, std::string_view delimiter = " ");

std::pair<std::string_view, std::string_view> 
    splitTwo(std::string_view s, std::string_view delimiter = " ");

std::string_view readToken(std::string_view& s, std::string_view delimiter = " ");

// strips all leading and trailing ws
std::string_view strip_ws(std::string_view s);

template <typename Number>
Number strToNum(std::string_view input)
{
    Number num {};
    auto token = readToken(input);
    auto err = std::from_chars(token.data(), token.data() + token.size(), num);
    if (err.ec != std::errc {}) {
        std::stringstream msg;
        msg << "Invalid bus number format: " << token;
        throw std::invalid_argument(msg.str());
    }
    return num;
}

template <typename Number>
Number readNumberOnLine(std::istream& stream)
{
    Number number {};
    stream >> number;
    std::string dummy;
    std::getline(stream, dummy);
    return number;
}
