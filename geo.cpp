#include "geo.h"
#include "util.h"

#include <cmath>
#include <charconv>
#include <sstream>
using namespace std;
using std::from_chars;

namespace geo {

Coordinate Coordinate::parseFromStr(string_view input)
{
    input = strip_ws(input);
    double lat = 0.0;
    double lon = 0.0;

    auto token = readToken(input, ", ");
    auto err = from_chars(token.data(), token.data() + token.size(), lat);
    if (err.ec != errc {}) {
        stringstream msg;
        msg << "Invalid cordinate format: " << token;
        throw invalid_argument(msg.str());
    }

    token = readToken(input);
    err = from_chars(token.data(), token.data() + token.size(), lon);
    if (err.ec != errc {}) {
        stringstream msg;
        msg << "Invalid cordinate format: " << token;
        throw invalid_argument(msg.str());
    }
    return {lat, lon};
}

// haversine formula
double distance(Coordinate punkt_a, Coordinate punkt_b)
{
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