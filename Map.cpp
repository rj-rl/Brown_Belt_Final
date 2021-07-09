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

double Route::getLength(const Map& map) const
{
	if (cached_length.has_value()) return *cached_length;

	double total_length = 0.0;
	Stop* first_stop = stops.front();
	for (size_t i = 1; i < stops.size(); ++i) {
		total_length += map.distance(first_stop->name, stops[i]->name);
		first_stop = stops[i];
	}
	cached_length = total_length;
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

void Map::addStop(string name, geo::Coordinate location)
{
	owned_names_.push_back(move(name));
	auto& str = owned_names_.back();
	stops_[str] = Stop(str, location);
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

double Map::distance(string_view a, string_view b) const
{
	Stop stop_a = stops_.at(a);
	Stop stop_b = stops_.at(b);
	Segment a_b_segment = {stop_a.location, stop_b.location};

	if (distance_cache_.count(a_b_segment) == 0) {
		distance_cache_[a_b_segment] = geo::distance(stop_a.location, stop_b.location);
	}
	return distance_cache_[a_b_segment];
}

//======================================== UTILITY  ============================================//

void unfold(Route& route)
{
	if (route.stops.size() < 2) return;
	for (size_t i = route.stops.size() - 1; i --> 0;) {
		route.stops.push_back(route.stops[i]);
	}
}
