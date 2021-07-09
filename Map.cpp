#include "Map.h"
#include <algorithm>
using namespace std;

//======================================== STOP ================================================//

void Stop::addBus(string_view bus_name)
{
	buses.insert(bus_name);
}

//======================================== ROUTE  ==============================================//

void Route::addStop(Stop* stop)
{
	stops.push_back(stop);
	unique_stop_names.emplace(stops.back()->name);
}

double Route::getLengthCrowFlies(const Map& map) const
{
	if (cached_length.crow_flies.has_value()) return *cached_length.crow_flies;

	double total_length = 0.0;
	Stop* first_stop = stops.front();
	for (size_t i = 1; i < stops.size(); ++i) {
		total_length += map.distanceCrowFlies(first_stop->name, stops[i]->name);
		first_stop = stops[i];
	}
	cached_length.crow_flies = total_length;
	return total_length;
}

double Route::getLength(const Map& map) const
{
	if (cached_length.by_road.has_value()) return *cached_length.by_road;

	double total_length = 0.0;
	Stop* first_stop = stops.front();
	for (size_t i = 1; i < stops.size(); ++i) {
		total_length += map.distance(first_stop->name, stops[i]->name);
		first_stop = stops[i];
	}
	cached_length.by_road = total_length;
	return total_length;
}

size_t Route::getStopCount() const
{
	return stops.size();
}

size_t Route::getUniqueStopCount() const
{
	return unique_stop_names.size();
}

//========================================== MAP  ==============================================//

void Map::addStop(StopId name, geo::Coordinate location)
{
	auto name_copy = name;	// to be able to move otherwise it's 
	// "stops_[move(name)] = Stop(name, location);" and who knows what's evaluated first
	stops_[move(name)] = Stop(move(name_copy), location);
}

bool Map::hasStop(const StopId& stop_id) const
{
	return stops_.count(stop_id);
}

// expands a linear route 'a - b - c' into circular representation 'a - b - c - b - a'
void unfold(Route& route);

Route Map::buildRouteLine(const vector<string>& stop_names)
{
	Route route = buildRouteCirc(stop_names);
	route.type = Route::Type::LINE;
	unfold(route);
	return route;
}

Route Map::buildRouteCirc(const vector<string>& stop_names)
{
	// unlike the name suggests, just builds route through every stop in 'stops'
	// in a linear fashion, what makes it circular is that last stop == first stop
	Route route;
	route.type = Route::Type::CIRCLE;
	for (const auto& stop_name : stop_names) {
		route.addStop(&stops_[stop_name]);
	}
	return route;
}

double Map::distanceCrowFlies(const StopId& a, const StopId& b) const
{
	const Stop& stop_a = stops_.at(a);
	const Stop& stop_b = stops_.at(b);
	Segment a_b_segment = {stop_a.location, stop_b.location};

	if (distance_cache_.count(a_b_segment) == 0) {
		distance_cache_[a_b_segment] = geo::distance(stop_a.location, stop_b.location);
	}
	return distance_cache_[a_b_segment];
}

double Map::distance(const StopId& a, const StopId& b) const
{
	double distance = .0;
	const Stop& stop_a = stops_.at(a);
	const Stop& stop_b = stops_.at(b);
	if (stop_a.distances.count(stop_b.name) > 0) {
		return stop_a.distances.at(stop_b.name);
	}
	else if (stop_b.distances.count(stop_a.name)) {
		return stop_b.distances.at(stop_a.name);
	}
	else {
		return distanceCrowFlies(a, b);
	}
}

//======================================== UTILITY  ============================================//

void unfold(Route& route)
{
	if (route.stops.size() < 2) return;
	for (size_t i = route.stops.size() - 1; i --> 0;) {
		route.stops.push_back(route.stops[i]);
	}
}
