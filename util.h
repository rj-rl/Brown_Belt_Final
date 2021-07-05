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

template <typename Number>
Number strToNum(std::string_view str)
{
// use from_chars when available to git rid of string copy
    size_t pos;
    Number result;
    if constexpr (std::is_integral_v<Number>) {
        result = std::stoi(std::string(str), &pos);
    }
    else {
        result = std::stod(std::string(str), &pos);
    }
    if (pos != str.length()) {
        std::stringstream error;
        error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
        throw std::invalid_argument(error.str());
    }
    return result;
}

template <typename Number>
Number readNumberOnLine(std::istream& stream)
{
    Number number;
    stream >> number;
    std::string dummy;
    std::getline(stream, dummy);
    return number;
}


