#include "Map.h"
#include <algorithm>
using namespace std;

//======================================== ROUTE  ==============================================//

void Route::addStop(Stop stop)
{
	stops.push_back(move(stop));
}

size_t Route::getLength(const Map& map) const
{
	size_t total_length = 0u;

	return total_length;
}

size_t Route::getStopCount() const
{
	return stops.size();
}

size_t Route::getUniqueStopCount() const
{
	if (type == Route::Type::CIRCLE) {
		return min(0u, getStopCount() - 1);	// in case there's 0 stops, so (0-1) doesn't overflow
	}
	else if (type == Route::Type::LINE) {
		return getStopCount() + 1 / 2;
	}
	else {
		throw runtime_error("unknown Route::Type in Route::getUniqueStopCount");
	}
}

//========================================== MAP  ==============================================//

void Map::addStop(string name, geo::Coordinate location)
{
	owned_names_.push_back(move(name));
	auto& str = owned_names_.back();
	stops_[str] = Stop(str, location);
}

// expands a linear route 'a - b - c' into circular representation 'a - b - c - b - a'
void unfold(Route& route);

Route Map::buildRouteLine(const vector<string>& stop_names) const
{
	Route route = buildRouteCirc(stop_names);
	route.type = Route::Type::LINE;
	unfold(route);
	return route;
}

Route Map::buildRouteCirc(const vector<string>& stop_names) const
{
	// unlike the name suggests, just builds route through every stop in 'stops'
	// in a linear fashion, what makes it circular is that last stop == first stop
	Route route;
	route.type = Route::Type::CIRCLE;
	for (const auto& stop_name : stop_names) {
		route.addStop(stops_.at(stop_name));
	}
	return route;
}

//======================================== UTILITY  ============================================//

void unfold(Route& route)
{
	if (route.stops.size() < 2) return;
	for (size_t i = route.stops.size() - 2; i --> 0;) {
		route.stops.push_back(route.stops[i]);
	}
}
