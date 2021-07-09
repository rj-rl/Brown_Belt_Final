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

	Route	buildRoute(const std::vector<StopId>& stop_names, Route::Type type) const;

	// bus query logic
	void	addBus(Bus bus);
	double	getBusRouteLen			(const BusId& bus_id) const;
	double	getBusRouteCurvature	(const BusId& bus_id) const;
	size_t	getBusStopCount			(const BusId& bus_id) const;
	size_t	getBusUniqueStopCount	(const BusId& bus_id) const;
	const Bus*  getBus				(const BusId& bus_id) const;
	Bus*		getBus				(const BusId& bus_id);
	// check if 'bus_id' is being tracked by our 'bus_park_'
	bool	isTracking				(const BusId& bus_id) const;

	// stop query logic
	void			addStop			(StopId name, geo::Coordinate location);
	const BusList*	getStopBusList	(const StopId& stop_id) const;
	const Stop*		getStop			(const StopId& stop_id) const;
	Stop*			getStop			(const StopId& stop_id);
	// check if 'stop_id' is on our 'map_'
	bool			isMapping		(const StopId& stop_id) const;

private:
	Map&	 map_;
	BusPark& bus_park_;	
};

