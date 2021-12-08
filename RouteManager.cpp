#include "RouteManager.h"
#include "graph.h"
#include <utility>
#include <set>
#include <stdexcept>
#include <string_view>
using namespace std;
using namespace Graph;

void RouteManager::addStop(StopId name, geo::Coordinate location, DistanceList distances)
{
    map_.addStop(move(name), location, move(distances));
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
    Bus* bus = bus_park_.getBus(bus_id);
    if (not bus) throw runtime_error("BusPark::getBus returned nullptr");
    return bus->routeCurvature(map_);
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

const BusSet* RouteManager::getStopBusList(const StopId& stop_id) const
{
    return getStop(stop_id)->getBuses();
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

optional<Router<double>::RouteInfo> RouteManager::shortestPath(const StopId& from, const StopId& to) const
{
    optional<Router<double>::RouteInfo> route_info 
        = router_->BuildRoute(getVertexId(from), getVertexId(to));
    return route_info;
}

bool RouteManager::isMapping(const StopId& stop_id) const
{
    return map_.hasStop(stop_id);
}

DirectedWeightedGraph<double> RouteManager::buildGraph() const
{
    using EdgeType = Edge<double>;
    DirectedWeightedGraph<double> graph(2 * stopCount());  // 2 vertices per stop 
    // one vertex is to represent starting to wait for a bus, another for boarding the bus
    mapStopsToVertices();

    for (auto* bus : getBusList()) {
        const auto& stops = bus->getRoute().stops;
        for (size_t i = 0; i < stops.size() - 1; ++i) {  // 'size - 1' cuz last stop == first stop
            size_t vertex_start_waiting = getVertexId(stops[i]->id());
            size_t vertex_board_bus = vertex_start_waiting + 1;
            size_t span_count = 0;
            double travel_time = 0.0;
            EdgeType wait_on_stop = {
                vertex_start_waiting, /* from 'begin holding!' */
                vertex_board_bus,     /* to 'board bus'  */
                waitTime(),           /* wait time in seconds  */
                bus->id(),            /* bus_id */
                span_count            /* span_count */
            };
            graph.AddEdge(wait_on_stop);

            for (size_t j = i + 1; j < stops.size(); ++j) {
                size_t vertex_ride_and_start_waiting = getVertexId(stops[j]->id());
                travel_time += 
                    map_.distance(stops[j - 1]->name, stops[j]->name) / busSpeed();
                EdgeType ride_a_few_stops = {
                    vertex_board_bus,               /* from 'begin holding!' */
                    vertex_ride_and_start_waiting,  /* to 'board bus'  */
                    travel_time,                    /* wait time in seconds  */
                    bus->id(),                      /* bus_id */
                    ++span_count                    /* span_count */
                };
                graph.AddEdge(ride_a_few_stops);
            }
        }
    }
    return graph;
}

void RouteManager::mapStopsToVertices() const
{
    int i = 0;
    for (auto* stop : getStopList()) {
        // all even indices are for the "start_waiting" stops
        // all 'i + 1' indices are for "board_bus" stops

        stop_names_to_vertex_ids_[stop->id()] = i;
        vertex_ids_to_stop_names_[i] = stop->id();
        vertex_ids_to_stop_names_[i + 1] = stop->id();
        i += 2;
    }
}
