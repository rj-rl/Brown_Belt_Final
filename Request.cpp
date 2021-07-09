#include "Request.h"
#include "Response.h"
#include "util.h"
#include <stdexcept>
#include <sstream>
#include <vector>
#include <utility>
using namespace std;

//======================================== UTILITY =============================================//

void printResponses(const vector<ResponseHolder>& responses, ostream& out)
{
    for (const auto& response : responses) {
        response->print(out);
        out << '\n';
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

//=================================== PARSE AND PROCESS ========================================//

RequestHolder Request::create(Request::Type type)
{
    switch (type) {
    case Request::Type::ADD_BUS:
        return make_unique<AddBusRequest>();
    case Request::Type::ADD_STOP:
        return make_unique<AddStopRequest>();
    case Request::Type::GET_BUS_INFO:
        return make_unique<GetBusInfo>();
    case Request::Type::GET_STOP_INFO:
        return make_unique<GetStopInfo>();
    default:
        return nullptr;
    }
}

RequestHolder parseRequest(string_view request_str, RequestCategory category)
{
    const TypeTable& table = (category == RequestCategory::READ)
        ? STR_TO_QUERY_TYPE
        : STR_TO_MODIFYING_REQUEST_TYPE;
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

vector<ResponseHolder> processRequests(
    const RequestsContainer& modify_requests,
    const RequestsContainer& query_requests)
{
    Map map;
    BusPark bus_park;
    RouteManager manager(map, bus_park);
    vector<ResponseHolder> result;

        // requests without return values (aka ModifyRequests) go here
    for (const auto& request_holder : modify_requests.fill_map_requests) {
        auto& request = static_cast<ModifyRequest&>(*request_holder);
        request.process(manager);
    }
    for (const auto& request_holder : modify_requests.fill_bus_park_requests) {
        auto& request = static_cast<ModifyRequest&>(*request_holder);
        request.process(manager);
    }

    for (const auto& request_holder : query_requests.queries) {
        if (request_holder->type == Request::Type::GET_BUS_INFO) {
            const auto& request = static_cast<const GetBusInfo&>(*request_holder);
            result.push_back(request.process(manager));
        }
        if (request_holder->type == Request::Type::GET_STOP_INFO) {
            const auto& request = static_cast<const GetStopInfo&>(*request_holder);
            result.push_back(request.process(manager));
        }
    }
    return result;
}

//===================================== AddBusRequest ==========================================//

void AddBusRequest::process(RouteManager& route_mgr)
{
    Bus bus {move(bus_id), route_mgr.buildRoute(stop_names, route_type)};
    route_mgr.addBus(move(bus));
}

void AddBusRequest::parseRouteType(string_view input, string* delimiter)
{
    size_t pos = 0u;
    pos = input.find(" > ");
    if (pos < input.npos) {
        route_type = Route::Type::CIRCLE;
        *delimiter = " > ";
        return;
    }
    else if (pos = input.find(" - "); pos < input.npos) {
        route_type = Route::Type::LINE;
        *delimiter = " - ";
        return;
    }
    else {
        stringstream msg;
        msg << "Invalid route delimiter format: " << input;
        throw runtime_error(msg.str());
    }
}

void AddBusRequest::parseFrom(string_view input)
{
    bus_id = readToken(input, ": ");
    string delimiter;
    parseRouteType(input, &delimiter);

    while (not input.empty()) {
        stop_names.emplace_back(readToken(input, delimiter));
    }

    if (route_type == Route::Type::CIRCLE
        && stop_names.front() != stop_names.back()) {
        throw runtime_error("Invalid circle route, first and last must match");
    }
}

//===================================== AddStopRequest =========================================//

void AddStopRequest::process(RouteManager& route_mgr)
{
    route_mgr.addStop(move(name), location);
}

void AddStopRequest::parseFrom(string_view input)
{
    name = readToken(input , ": ");
    location = geo::Coordinate::parseFromStr(input);
}

//======================================= GetBusInfo ===========================================//

ResponseHolder GetBusInfo::process(RouteManager& route_mgr) const
{
    ResponseHolder response = Response::create(Response::Type::BUS_INFO);
    auto& concrete_response = static_cast<BusInfoResponse&>(*response);

    concrete_response.set_bus_id(bus_id);
    if (route_mgr.isTracking(bus_id)) {
        auto data = BusInfoResponse::Data();
        data.stop_cnt = route_mgr.getBusStopCount(bus_id);
        data.unique_stop_cnt = route_mgr.getBusUniqueStopCount(bus_id);
        data.route_len = route_mgr.getBusRouteLen(bus_id);
        concrete_response.set_data(data);
    }
    return response;
}

void GetBusInfo::parseFrom(string_view input)
{
    bus_id = input;
}

//======================================= GetStopInfo ===========================================//

ResponseHolder GetStopInfo::process(RouteManager& route_mgr) const
{
    ResponseHolder response = Response::create(Response::Type::STOP_INFO);
    auto& concrete_response = static_cast<StopInfoResponse&>(*response);

    concrete_response.set_stop_id(stop_id);
    if (route_mgr.isMapping(stop_id)) {
        vector<string> bus_list (
            route_mgr.getStopBusList(stop_id)->begin(),
            route_mgr.getStopBusList(stop_id)->end()
        );
        concrete_response.set_data(move(bus_list));
    }
    return response;
}

void GetStopInfo::parseFrom(string_view input)
{
    stop_id = input;
}
