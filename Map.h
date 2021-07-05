#pragma once

#include "geo.h"
#include <string>
#include <vector>
#include <iostream>

struct Stop {
	std::string name;
	geo::Coordinate location;

	Stop(std::string nm = {}, geo::Coordinate loc = {})
		: name {nm}, location {loc}
	{}
};


class Map {
public:
	Map()
	{}

private:
	std::vector<Stop> stops_;
};
