#include "RouteManager.h"
#include <utility>
using namespace std;

void RouteManager::addStop(string name, geo::Coordinate location)
{
	map_.addStop(move(name), location);
}

void RouteManager::addBus(Bus bus)
{
	bus_park_.addBus(move(bus));
}

Route RouteManager::buildRoute(const vector<string>& stops, Route::Type type) const
{
	if (type == Route::Type::LINE) {
		return map_.buildRouteLine(stops);
	}
	return map_.buildRouteCirc(stops);
}

double RouteManager::getBusRouteLen(Id bus_id) const
{
	Bus* bus = bus_park_.getBus(bus_id);
		// this should never happen, we only call this function when 'bus_id' is tracked
	if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
	return bus->routeLen(map_);
}

size_t RouteManager::getBusStopCount(Id bus_id) const
{
	Bus* bus = bus_park_.getBus(bus_id);
		// this should never happen, we only call this function when 'bus_id' is tracked
	if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
	return bus->stopCount();
}

size_t RouteManager::getBusUniqueStopCount(Id bus_id) const
{
	Bus* bus = bus_park_.getBus(bus_id);
			// this should never happen, we only call this function when 'bus_id' is tracked
	if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
	return bus->uniqueStopCount();
}

const Bus* RouteManager::getBus(Id bus_id) const
{
	return bus_park_.getBus(bus_id);
}

Bus* RouteManager::getBus(Id bus_id)
{
	return bus_park_.getBus(bus_id);
}

bool RouteManager::isTracking(Id bus_id) const
{
	return bus_park_.contains(bus_id);
}

