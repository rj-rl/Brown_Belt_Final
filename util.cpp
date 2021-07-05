#include "util.h"

using namespace std;

pair<string_view, optional<string_view>> 
    splitTwoStrict(string_view s, string_view delimiter)
{
    const size_t pos = s.find(delimiter);
    if (pos == s.npos) {
        return {s, nullopt};
    }
    else {
        return {s.substr(0, pos), s.substr(pos + delimiter.length())};
    }
}

pair<string_view, string_view> 
    splitTwo(string_view s, string_view delimiter)
{
    const auto [lhs, rhs_opt] = splitTwoStrict(s, delimiter);
    return {lhs, rhs_opt.value_or("")};
}

string_view readToken(string_view& s, string_view delimiter)
{
    const auto [lhs, rhs] = splitTwo(s, delimiter);
    s = rhs;
    return lhs;
}
