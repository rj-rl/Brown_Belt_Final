#include "RouteManager.h"
#include <utility>
#include <set>
#include <stdexcept>
#include <string_view>
using namespace std;

void RouteManager::addStop(StopId name, geo::Coordinate location)
{
	map_.addStop(move(name), location);
}

void RouteManager::addBus(Bus bus)
{
	const Bus* added = bus_park_.addBus(move(bus));
	const auto& route = added->getRoute();
	for (auto stop : route.stops) {
		stop->addBus(added->id());
	}
}

Route RouteManager::buildRoute(const vector<StopId>& stops, Route::Type type) const
{
	if (type == Route::Type::LINE) {
		return map_.buildRouteLine(stops);
	}
	return map_.buildRouteCirc(stops);
}

double RouteManager::getBusRouteLen(const BusId& bus_id) const
{
	Bus* bus = bus_park_.getBus(bus_id);
		// this should never happen, we only call this function when 'bus_id' is tracked
	if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
	return bus->routeLen(map_);
}

double RouteManager::getBusRouteCurvature(const BusId& bus_id) const
{
	return 1.0;	// TO DO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

size_t RouteManager::getBusStopCount(const BusId& bus_id) const
{
	Bus* bus = bus_park_.getBus(bus_id);
	if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
	return bus->stopCount();
}

size_t RouteManager::getBusUniqueStopCount(const BusId& bus_id) const
{
	Bus* bus = bus_park_.getBus(bus_id);
	if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
	return bus->uniqueStopCount();
}

const BusList* RouteManager::getStopBusList(const StopId& stop_id) const
{
	return getStop(stop_id)->getBusList();
}

const Bus* RouteManager::getBus(const BusId& bus_id) const
{
	return bus_park_.getBus(bus_id);
}

Bus* RouteManager::getBus(const BusId& bus_id)
{
	return bus_park_.getBus(bus_id);
}

const Stop* RouteManager::getStop(const StopId& stop_id) const
{
	return map_.getStop(stop_id);
}

Stop* RouteManager::getStop(const StopId& stop_id)
{
	return map_.getStop(stop_id);
}

bool RouteManager::isTracking(const BusId& bus_id) const
{
	return bus_park_.contains(bus_id);
}

bool RouteManager::isMapping(const StopId& stop_id) const
{
	return map_.hasStop(stop_id);
}

