#pragma once
#include <iostream>
#include <string_view>

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
	
	void parseFromStr(std::string_view input);
	//static_assert(std::is_arithmetic_v<Number>, "T is not arithmetic type (in Coordinate<T>)");
};


inline double degToRad(double degree) 
{
	return degree * p;
}

double distance(Coordinate punkt_a, Coordinate punkt_b);

}	// ns ::geo