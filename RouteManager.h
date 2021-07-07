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
	double	getBusRouteLen(Id bus_id) const;
	double	getBusStopCount(Id bus_id) const;
	double	getBusUniqueStopCount(Id bus_id) const;
	
	// check if 'bus_id' is being tracked by our 'bus_park_'
	bool	isTracking(Id bus_id) const;

private:
	Map&	 map_;
	BusPark& bus_park_;	
};

