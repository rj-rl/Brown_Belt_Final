#include "Request.h"
#include "util.h"
#include <stdexcept>
using namespace std;

RequestHolder Request::create(Request::Type type)
{
    switch (type) {
    case Request::Type::ADD_BUS:
        return make_unique<AddBusRequest>();
    case Request::Type::ADD_STOP:
        return make_unique<AddStopRequest>();
    case Request::Type::GET_BUS_INFO:
        return make_unique<GetBusInfo>();
    default:
        return nullptr;
    }
}

optional<Request::Type> convertRequestTypeFromString(string_view type_str, const TypeTable& table)
{
    if (const auto it = table.find(type_str);
        it != table.end()) {
        return it->second;
    }
    else {
        return nullopt;
    }
}

RequestHolder parseRequest(string_view request_str, QueryType is_query)
{
    const TypeTable& table = (is_query == QueryType::READ)
        ? STR_TO_QUERY_TYPE
        : STR_TO_REQUEST_TYPE;
    const auto request_type = convertRequestTypeFromString(readToken(request_str), table);
    if (!request_type) {
        return nullptr;
    }
    RequestHolder request = Request::create(*request_type);
    if (request) {
        request->parseFrom(request_str);
    };
    return request;
}

vector<Response> processRequests(const vector<RequestHolder>& requests)
{
    Map map;
    BusPark bus_park;
    RouteManager manager(map, bus_park);
    vector<Response> result;

    for (const auto& request_holder : requests) {
        if (request_holder->type == Request::Type::GET_BUS_INFO) {
            const auto& request = static_cast<const GetBusInfo&>(*request_holder);
            result.push_back(request.process(manager));
        }
        // requests without return values (aka ModifyRequests) go here
        else {  
            auto& request = static_cast<ModifyRequest&>(*request_holder);
            request.process(manager);
        }
    }
    return result;
}

// formats and prints data for a single request response
void print_data(const Response& response, ostream& out)
{
    out << response.data->stop_cnt << " stops on route, "
        << response.data->unique_stop_cnt << " unique stops, "
        << response.data->route_len << " route length";
}

void printResponses(const vector<Response>& responses, ostream& out)
{
    for (const auto& response : responses) {
        out << "Bus " << response.bus_id << ": ";
        if (response.data) {
            out << response.data->stop_cnt << "stops on route, "
                << response.data->unique_stop_cnt << "unique stops, "
                << response.data->route_len << "route length";
            out << '\n';
        }
        else {
            out << "not found\n";
        }
    }
}

void AddBusRequest::process(RouteManager& route_mgr)
{
    Bus bus {bus_id, route_mgr.buildRoute(stop_names, route_type)};
    route_mgr.addBus(move(bus));
}

Route::Type AddBusRequest::parseRouteType(string_view input)
{
    if (input.empty() || input[0] == '-') {
        return Route::Type::LINE;
    }
    else if (input[0] == '>') {
        return Route::Type::CIRCLE;
    }
    else {
        throw runtime_error("Invalid route delimiter format: " + input[0]);
    }
}

void AddBusRequest::parseFrom(string_view input)
{
    bus_id = strToNum<Id>(readToken(input, ":"));
    input = strip_ws(input);     // note: this assumes leading whitespace, might not be there
                                 // but FOR NOW we're fine even then

    stop_names.emplace_back(readToken(input));    // assumes there's at least one stop
    route_type = parseRouteType(input);

    while (not input.empty()) {
        input.remove_prefix(2);
        stop_names.emplace_back(readToken(input));
    }
    if (route_type == Route::Type::CIRCLE 
        && stop_names.front() != stop_names.back()) {
        throw runtime_error("Invalid circle route, first and last must match");
    }
}


void AddStopRequest::process(RouteManager& route_mgr)
{
    route_mgr.addStop(move(name), location);
}

void AddStopRequest::parseFrom(string_view input)
{
    name = readToken(input , ":");
    location = geo::Coordinate::parseFromStr(input);
}


Response GetBusInfo::process(RouteManager& route_mgr) const
{
    Response response {bus_id};
    if (route_mgr.isTracking(bus_id)) {
        response.data->route_len = route_mgr.getBusRouteLen(bus_id);
        response.data->stop_cnt = route_mgr.getBusStopCount(bus_id);
        response.data->unique_stop_cnt = route_mgr.getBusUniqueStopCount(bus_id);
    }
    return response;
}

void GetBusInfo::parseFrom(string_view input)
{
    bus_id = strToNum<Id>(readToken(input));
}
