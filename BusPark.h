#pragma once
#include "Map.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>
#include <utility>
#include <optional>

using Id = size_t;	// for bus ids

class Bus {
public:
	Bus(Id id = 0, Route route = {})	// take stops by value to store them
		: id_ {id}
		, route_ {std::move(route)}
	{}

	Id		id() const { return id_; }
	size_t	stopCount() const;
	size_t	uniqueStopCount() const;
	double	routeLen(const Map& map) const;

	bool operator < (const Bus& that) const
	{
		return this->id_ < that.id_;
	}

private:
	Id		id_;
	Route	route_;
};


class BusPark {
public:
	BusPark() = default;

	void addBus(Bus bus)
	{
		buses_.emplace(bus.id(), std::move(bus));
	}

	bool	   contains(Id bus_id) const;
	Bus*       getBus(Id bus_id);
	const Bus* getBus(Id bus_id) const;

private:
	std::unordered_map<Id, Bus> buses_;
};

