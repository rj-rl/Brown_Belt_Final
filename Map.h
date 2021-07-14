#pragma once
#include "geo.h"
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>

using StopId = std::string;
using BusSet = std::set<std::string_view>;
using DistanceList = std::unordered_map<StopId, double>;

// fwd declarations
class Bus;
class Map;

struct Stop {
	StopId				name;
	geo::Coordinate		location;
	BusSet				buses;
	DistanceList		distances;

	Stop(std::string_view nm = {}, geo::Coordinate loc = {}, DistanceList distances = {})
		: name {nm}, location {loc}, buses {}, distances {move(distances)}
	{}

	std::string_view id() const { return name; }
	void			addBus(std::string_view bus_name);
	const BusSet*	getBuses() const { return &buses; }

	bool operator < (const Stop& that) const { return name < that.name; }
	bool operator == (const Stop& that) const { return name == that.name; }
};


struct Route {
	enum class Type {
		CIRCLE,
		LINE,
	};
	struct DistanceCache {
		std::optional<double> by_road;
		std::optional<double> crow_flies;
	};

	Type								 type;
	std::vector<Stop*>					 stops;
	std::unordered_set<std::string_view> unique_stop_names;
	mutable DistanceCache				 cached_length;

	void	addStop				(Stop*);
	double	getLengthCrowFlies	(const Map& map) const;
	double	getLength			(const Map& map) const;
	size_t	getStopCount		() const;
	size_t	getUniqueStopCount	() const;
};


class Map {
public:
	Map()
	{}

	void addStop(StopId name, geo::Coordinate location, DistanceList distances);
	bool hasStop(const StopId& stop_id) const;
	auto stopCount() const { return stops_.size(); }
	Stop*		getStop(const StopId& stop_id) { return &stops_.at(stop_id); }
	const Stop* getStop(const StopId& stop_id) const { return &stops_.at(stop_id); }

	Route buildRouteLine(const std::vector<std::string>& stop_names);
	Route buildRouteCirc(const std::vector<std::string>& stop_names);

	double distanceCrowFlies(const StopId& a, const StopId& b) const;
	double distance(const StopId& a, const StopId& b) const;

	const auto& getStopList() const { return stop_list_; }

private:
	std::unordered_map<StopId, Stop> stops_;
	std::vector<Stop*>				 stop_list_;

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
};
