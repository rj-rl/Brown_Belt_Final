#include "BusPark.h"
#include "util.h"
#include <string_view>
using namespace std;

size_t Bus::stopCount() const
{
    return route_.getStopCount();
}

size_t Bus::uniqueStopCount() const
{
    return route_.getUniqueStopCount();
}

double Bus::routeLen(const Map& map) const
{
    return route_.getLength(map);
}

bool BusPark::contains(const Id& bus_id) const
{
    return buses_.count(bus_id) > 0;
}

Bus* BusPark::getBus(const Id& bus_id)
{
    return contains(bus_id)
        ? &buses_[bus_id]
        : nullptr;
}

const Bus* BusPark::getBus(const Id& bus_id) const
{
    return contains(bus_id)
        ? &buses_.at(bus_id)
        : nullptr;
}
