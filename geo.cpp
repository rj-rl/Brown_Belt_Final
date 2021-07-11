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

// haversine formula
double distance(Coordinate punkt_a, Coordinate punkt_b)
{
    // recommended version:
    //auto a_lat = degToRad(punkt_a.lat);
    //auto a_lon = degToRad(punkt_a.lon);

    //auto b_lat = degToRad(punkt_b.lat);
    //auto b_lon = degToRad(punkt_b.lon);

    //double a = sin(a_lat) * sin(b_lat) +
    //    cos(a_lat) * cos(b_lat) * cos(a_lon - b_lon);
    //double d = earth_r * acos(a);

	auto d_lat = degToRad(punkt_a.lat - punkt_b.lat);   // latitude distance 
	auto d_lon = degToRad(punkt_a.lon - punkt_b.lon);   // longitude distance

    double a = sin(d_lat / 2) * sin(d_lat / 2) +
        cos(degToRad(punkt_a.lat)) * cos(degToRad(punkt_b.lat)) *
        sin(d_lon / 2) * sin(d_lon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = earth_r * c;

    return d;
}

}	// ns ::geo