#include "geo.h"
#include "util.h"

#include <cmath>
#include <sstream>
using namespace std;

namespace geo {

Coordinate Coordinate::parseFromStr(string_view& input)
{
    auto lat = strToNum<double>(readToken(input, ", "));
    auto lon = strToNum<double>(readToken(input, ", "));
    return {lat, lon};
}

Coordinate Coordinate::parseFromJson(const Json::Node& input)
{
    const auto& lon = input.AsMap().at("longitude").AsDouble();
    const auto& lat = input.AsMap().at("latitude").AsDouble();
    return {lat, lon};
}

// uses the haversine formula
double distance(Coordinate punkt_a, Coordinate punkt_b)
{
    // recommended version:
    auto a_lat = degToRad(punkt_a.lat);
    auto a_lon = degToRad(punkt_a.lon);

    auto b_lat = degToRad(punkt_b.lat);
    auto b_lon = degToRad(punkt_b.lon);

    double a = sin(a_lat) * sin(b_lat) +
        cos(a_lat) * cos(b_lat) * cos(a_lon - b_lon);
    double d = earth_r * acos(a);

    return d;
}

}    // ns ::geo
