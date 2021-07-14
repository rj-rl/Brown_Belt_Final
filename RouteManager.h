#pragma once
#include "Map.h"
#include "BusPark.h"
#include "router.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

class RouteManager {
public:
	RouteManager(Map& map, BusPark& bus_park)
		: map_ {map}
		, bus_park_ {bus_park}
		, router_settings_ {}
		, router_ {nullptr}
	{}

	Route	buildRoute(const std::vector<StopId>& stop_names, Route::Type type) const;

	// bus query related
	void	addBus(Bus bus);
	double	getBusRouteLen(const BusId& bus_id) const;
	double	getBusRouteCurvature(const BusId& bus_id) const;
	size_t	getBusStopCount(const BusId& bus_id) const;
	size_t	getBusUniqueStopCount(const BusId& bus_id) const;
	const Bus*	getBus(const BusId& bus_id) const;
	Bus*		getBus(const BusId& bus_id);
	// check if 'bus_id' is being tracked by our 'bus_park_'
	bool	isTracking(const BusId& bus_id) const;

	// stop query related
	void			addStop(StopId name, geo::Coordinate location,
							DistanceList distances);
	const BusSet*	getStopBusList(const StopId& stop_id) const;
	const Stop*		getStop(const StopId& stop_id) const;
	Stop*			getStop(const StopId& stop_id);
	size_t			stopCount() const { return map_.stopCount(); }

	// router related
	using DRouter = Graph::Router<double>;
	using DGraph =  Graph::DirectedWeightedGraph<double>;

	struct RouterSettings {
		int		bus_wait_time = 0;	
		double	bus_velocity = 0.0;
	};
	void		setRouterSettings(RouterSettings settings) { router_settings_ = settings; }
	DGraph		buildGraph() const;
	void		setRouter(DRouter* router) { router_ = router; }
	const auto* getRouter() const { return router_; }
	std::optional<DRouter::RouteInfo> 
				shortestPath(const StopId& from, const StopId& to) const;

	// check if 'stop_id' is on our 'map_'
	bool			isMapping(const StopId& stop_id) const;

	const auto&		getBusList() const { return bus_park_.getBusList(); }
	const auto&		getStopList() const { return map_.getStopList(); }

	// in seconds
	double	waitTime() const { return router_settings_.bus_wait_time; }
	// in seconds
	double	busSpeed() const { return router_settings_.bus_velocity; }

	int  getVertexId(std::string_view name) const { return stop_names_to_vertex_ids_[name]; }
	auto vertexIdToStopId(int vertex_id) const { return vertex_ids_to_stop_names_[vertex_id]; }

private:
	Map&			map_;
	BusPark&		bus_park_;
	RouterSettings	router_settings_;
	DRouter*		router_;

	mutable std::unordered_map<std::string_view, int> stop_names_to_vertex_ids_;
	mutable std::unordered_map<int, std::string_view> vertex_ids_to_stop_names_;
	void mapStopsToVertices() const;
};

