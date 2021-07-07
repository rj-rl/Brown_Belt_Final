#pragma once

#include "geo.h"
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <unordered_map>

struct Stop {
	std::string_view name;		// string_view makes sure Stops are cheap to copy
	geo::Coordinate location;

	Stop(std::string_view nm = {}, geo::Coordinate loc = {})
		: name {nm}, location {loc}
	{}

	bool operator < (const Stop& that) const { return name < that.name; }
	bool operator == (const Stop& that) const { return name == that.name; }
	struct Hasher {
		size_t operator() (const Stop& stop) const
		{
			// POTENTIAL ERRORS down the line if we'll need to compare by coordinate too
			return std::hash<std::string_view> {}(stop.name);
		}
	};
};

class Map;

struct Route {
	enum class Type {
		CIRCLE,
		LINE,
	};

	std::vector<Stop> stops;
	Type			  type;

	void addStop(Stop);
	size_t getLength(const Map& map) const;
	size_t getStopCount() const;
	size_t getUniqueStopCount() const;
};


class Map {
public:
	Map()
	{}

	void addStop(std::string name, geo::Coordinate location);
	Route buildRouteLine(const std::vector<std::string>& stop_names) const;
	Route buildRouteCirc(const std::vector<std::string>& stop_names) const;

private:
	std::unordered_map<std::string_view, Stop> stops_;

// utility
private:
	using Segment = std::pair<geo::Coordinate, geo::Coordinate>;
	struct SegmentHasher {
		size_t operator() (const Segment& seg) const
		{
			constexpr size_t x = 514'229;
			size_t a = geo::Coordinate::Hasher {}(seg.first);
			size_t b = geo::Coordinate::Hasher {}(seg.second);
			return a * x * x + b * x;
		}
	};
	mutable std::unordered_map<Segment, double, SegmentHasher> distance_cache_;

	std::deque<std::string> owned_names_;	// to make life easier with all the str_views
	size_t distance(std::string_view a, std::string_view b) const;
};
