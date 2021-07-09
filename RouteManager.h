#pragma once
#include "Map.h"
#include "BusPark.h"
#include <string>
#include <vector>

class RouteManager {
public:
	RouteManager(Map& map, BusPark& bus_park)
		: map_ {map}
		, bus_park_ {bus_park}
	{}

	void	addStop(std::string name, geo::Coordinate location);
	void	addBus(Bus bus);
	Route	buildRoute(const std::vector<std::string>& stop_names, Route::Type type) const;
	// bus query logic
	double	getBusRouteLen(const BusId& bus_id) const;
	size_t	getBusStopCount(const BusId& bus_id) const;
	size_t	getBusUniqueStopCount(const BusId& bus_id) const;
	// stop query logic
	const BusList* getStopBusList(const StopId& stop_id) const;

	const Bus*  getBus(const BusId& bus_id) const;
	Bus*		getBus(const BusId& bus_id);
	const Stop* getStop(const StopId& stop_id) const;
	Stop*		getStop(const StopId& stop_id);
	
	// check if 'bus_id' is being tracked by our 'bus_park_'
	bool	isTracking(const BusId& bus_id) const;
	// check if 'stop_id' is on our 'map_'
	bool	isMapping(const StopId& stop_id) const;

private:
	Map&	 map_;
	BusPark& bus_park_;	
};

