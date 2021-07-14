#pragma once
#include "Map.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <utility>
#include <optional>

using BusId = std::string;

class Bus {
public:
	Bus(BusId id = {}, Route route = {})	// take stops by value to store them
		: id_ {std::move(id)}
		, route_ {std::move(route)}
	{}

	std::string_view id() const { return id_; }
	size_t			 stopCount() const;
	size_t			 uniqueStopCount() const;
	double			 routeLen(const Map& map) const;
	double			 routeCurvature(const Map& map) const;
	const Route&     getRoute() const;

	bool operator < (const Bus& that) const
	{
		return this->id_ < that.id_;
	}

private:
	BusId	id_;
	Route	route_;
};


class BusPark {
public:
	BusPark() = default;

	const Bus* addBus(Bus bus)
	{
		auto res = buses_.emplace(bus.id(), std::move(bus));
		bus_list_.push_back(&res.first->second);
		return &res.first->second;
	}

	bool	   contains(const BusId& bus_id) const;
	Bus*       getBus(const BusId& bus_id);
	const Bus* getBus(const BusId& bus_id) const;
	const auto& getBusList() const { return bus_list_; }

private:
	std::unordered_map<BusId, Bus> buses_;
	std::vector<Bus*>			   bus_list_;
};

