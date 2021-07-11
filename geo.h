#pragma once
#include <iostream>
#include <string_view>
#include <tuple>		// for std::tie
#include <functional>	// for std::hash
#include "json.h"

namespace geo {

constexpr double pi = 3.1415926535;
constexpr double p = pi / 180;
constexpr double earth_r = 6'371'000;	// Earth radius in meters

struct Coordinate {
	double lat = 0.0;	// latitude (that's what they call SHIROTA, mate)
	double lon = 0.0;	// longitude

	Coordinate(double lat = 0.0, double lon = 0.0)
		: lat {lat}
		, lon {lon}
	{}
	
	static Coordinate parseFromStr(std::string_view& input);
	static Coordinate parseFromJson(const Json::Node& input);

	bool operator == (const Coordinate& that) const
	{
		return std::tie(lat, lon) == std::tie(that.lat, that.lon);
	}
	struct Hasher {
		size_t operator() (const Coordinate& c) const
		{
			constexpr size_t x = 2'946'901;
			size_t a = std::hash<double> {}(c.lat);
			size_t b = std::hash<double> {}(c.lon);
			return a*x*x + b*x;
		}
	}; 
	
	friend std::ostream& operator << (std::ostream& out, const Coordinate& c)
	{
		return out << "(lat = " << c.lat << ", lon = " << c.lon << ")";
	}
};


inline double degToRad(double degree) 
{
	return degree * p;
}

double distance(Coordinate punkt_a, Coordinate punkt_b);

}	// ns ::geo