#pragma once
#include "Map.h"
#include "BusPark.h"
#include <string_view>
#include <unordered_map>
#include <utility>
#include <functional>		// for std::hash

class RouteManager {
public:
	RouteManager(Map& map, BusPark& bus_park)
		: map_ {map}
		, bus_park_ {bus_park}
	{}

	void	addStop(std::string name, geo::Coordinate location);
	void	addBus(Bus bus);
	Route	buildRoute(const std::vector<std::string>& stop_names, Route::Type type) const;
	double	getBusRouteLen(const Id& bus_id) const;
	size_t	getBusStopCount(const Id& bus_id) const;
	size_t	getBusUniqueStopCount(const Id& bus_id) const;

	const Bus*  getBus(const Id& bus_id) const;
	Bus*		getBus(const Id& bus_id);
	
	// check if 'bus_id' is being tracked by our 'bus_park_'
	bool	isTracking(const Id& bus_id) const;

private:
	Map&	 map_;
	BusPark& bus_park_;	
};

