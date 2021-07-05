#pragma once
#include "Map.h"
#include <vector>
#include <string>
#include <utility>

using Id = int;

class Bus {
public:
	Bus(Id id, std::vector<Stop> stops)	// take stops by value to store them
		: id_ {id}
		, stops_ {std::move(stops)}
	{}

	int		stopCount() const { return stops_.size(); }
	int		uniqueStopCount() const;
	double	routeLen() const;

private:
	Id					id_;
	std::vector<Stop>	stops_;
};


class BusPark {
public:
	BusPark() = default;

	void addBus(Bus bus)
	{
		buses_.push_back(std::move(bus));
	}

	Bus*		getBus(Id id);
	const Bus*	getBus(Id id) const;

private:
	std::vector<Bus> buses_;
};

