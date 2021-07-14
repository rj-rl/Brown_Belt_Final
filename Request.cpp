#include "Request.h"
#include "Response.h"
#include "RouteManager.h"
#include "util.h"
#include "router.h"
#include <stdexcept>
#include <sstream>
#include <vector>
#include <utility>
#include <unordered_map>
using namespace std;
using namespace Graph;

//======================================== UTILITY =============================================//

void printResponses(const vector<ResponseHolder>& responses, ostream& out)
{
    for (const auto& response : responses) {
        response->print(out);
        out << '\n';
    }
}

void printResponsesJSON(const vector<ResponseHolder>& responses, ostream& out)
{
    out << "[\n";
    bool first = true;
    for (const auto& response : responses) {
        if (first) {
            response->printJson(out);
            first = false;
        }
        else {
            out << ",\n";
            response->printJson(out);
        }
    }
    out << "\n]";
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

optional<Request::Type> getRequestTypeFromJson(const Json::Node& node, const TypeTable& table)
{
    if (const auto it = table.find(node.AsMap().at("type").AsString());
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
    case Request::Type::ADD_ROUTING_SETTINGS:
        return make_unique<AddRoutingSettings>();
    case Request::Type::GET_BUS_INFO:
        return make_unique<GetBusInfo>();
    case Request::Type::GET_STOP_INFO:
        return make_unique<GetStopInfo>();
    case Request::Type::GET_ROUTE_INFO:
        return make_unique<GetRouteInfo>();
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

RequestHolder parseRequestFromJson(const Json::Node& node, RequestCategory category)
{
    const auto& details = node.AsMap();
    const TypeTable& table = (category == RequestCategory::READ)
        ? STR_TO_QUERY_TYPE
        : STR_TO_MODIFYING_REQUEST_TYPE;
    const auto request_type = getRequestTypeFromJson(node, table);
    if (!request_type) {
        return nullptr;
    }
    RequestHolder request = Request::create(*request_type);
    if (request) {
        request->parseFrom(node);
    };
    return request;
}

RequestsContainer readRequestsJson(std::istream& input)
{
    RequestsContainer requests;
    Json::Document doc {Json::Load(input)};
    const auto& modify_requests = doc.GetRoot().AsMap().at("base_requests").AsArray();
    const auto& read_requests = doc.GetRoot().AsMap().at("stat_requests").AsArray();

    for (size_t i = 0; i < modify_requests.size(); ++i) {
        if (auto request = parseRequestFromJson(modify_requests[i], RequestCategory::MODIFY)) {
            if (request->type == Request::Type::ADD_BUS) {
                requests.fill_bus_park_requests.push_back(move(request));
            }
            else if (request->type == Request::Type::ADD_STOP) {
                requests.fill_map_requests.push_back(move(request));
            }
        }
    }
    for (size_t i = 0; i < read_requests.size(); ++i) {
        if (auto request = parseRequestFromJson(read_requests[i], RequestCategory::READ)) {
            requests.queries.push_back(move(request));
        }
    }

    if (doc.GetRoot().AsMap().count("routing_settings") > 0) {  // to make tests A to D work
        const auto& routing_settings_json = doc.GetRoot().AsMap().at("routing_settings");
        requests.routing_settings_request = Request::create(Request::Type::ADD_ROUTING_SETTINGS);
        requests.routing_settings_request->parseFrom(routing_settings_json);
    }    
    return requests;
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
    if (modify_requests.routing_settings_request) {
        static_cast<ModifyRequest&>(*modify_requests.routing_settings_request).process(manager);
    }
    for (const auto& request_holder : modify_requests.fill_map_requests) {
        auto& request = static_cast<ModifyRequest&>(*request_holder);
        request.process(manager);
    }
    for (const auto& request_holder : modify_requests.fill_bus_park_requests) {
        auto& request = static_cast<ModifyRequest&>(*request_holder);
        request.process(manager);
    }

        // Router setup
    auto graph = manager.buildGraph();
    Router router(graph);
    // EXPERIMENT
    //auto path = router.BuildRoute(
    //    manager.getVertexId("Biryulyovo Zapadnoye"), 
    //    manager.getVertexId("Universam")
    //);
    //for (size_t i = 0; i < path->edge_count; ++i) {
    //    cout << graph.GetEdge(router.GetRouteEdge(path->id, i)).bus_id << '\n';
    //    cout << graph.GetEdge(router.GetRouteEdge(path->id, i)).from << " - "
    //         << graph.GetEdge(router.GetRouteEdge(path->id, i)).to << " | weight: "
    //         << graph.GetEdge(router.GetRouteEdge(path->id, i)).weight << '\n';
    //}
    // END EXPERIMENT
    manager.setRouter(&router);

        // requests with return values (aka QueryRequest) go here
    for (const auto& request_holder : query_requests.queries) {
        if (request_holder->type == Request::Type::GET_BUS_INFO) {
            const auto& request = static_cast<const GetBusInfo&>(*request_holder);
            result.push_back(request.process(manager));
        }
        if (request_holder->type == Request::Type::GET_STOP_INFO) {
            const auto& request = static_cast<const GetStopInfo&>(*request_holder);
            result.push_back(request.process(manager));
        }
        if (request_holder->type == Request::Type::GET_ROUTE_INFO) {
            const auto& request = static_cast<const GetRouteInfo&>(*request_holder);
            result.push_back(request.process(manager));
        }
    }
    return result;
}

//=================================== AddRoutingSettings =======================================//

void AddRoutingSettings::process(RouteManager& route_mgr)
{
    route_mgr.setRouterSettings({bus_wait_time, bus_velocity});
}

void AddRoutingSettings::parseFrom(const Json::Node& input)
{
    bus_wait_time = (int)minutesToSeconds(input.AsMap().at("bus_wait_time").AsInt());
    bus_velocity  = kphToMps(input.AsMap().at("bus_velocity").AsDouble());
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

void AddBusRequest::parseRouteType(const Json::Node& input)
{
    if (input.AsMap().at("is_roundtrip").AsBool()) {
        route_type = Route::Type::CIRCLE;
        return;
    }
    else {
        route_type = Route::Type::LINE;
        return;
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

void AddBusRequest::parseFrom(const Json::Node& input)
{
    const auto& details = input.AsMap();
    bus_id = details.at("name").AsString();
    parseRouteType(input);

    for (const auto& stop : details.at("stops").AsArray()) {
        stop_names.emplace_back(stop.AsString());
    }

    if (route_type == Route::Type::CIRCLE
        && stop_names.front() != stop_names.back()) {
        throw runtime_error("Invalid circle route, first and last must match");
    }
}

//===================================== AddStopRequest =========================================//

void AddStopRequest::process(RouteManager& route_mgr)
{
    route_mgr.addStop(move(name), location, move(distances));
}

void AddStopRequest::parseFrom(string_view input)
{
    name = readToken(input , ": ");
    location = geo::Coordinate::parseFromStr(input);

    while (!input.empty()) {
        string stop_name;
        double distance = strToNum<double>(readToken(input, "m to "));
        stop_name = readToken(input, ", ");
        distances[move(stop_name)] = distance;
    }
}

void AddStopRequest::parseFrom(const Json::Node& input)
{
    const auto& details = input.AsMap();
    name = details.at("name").AsString();
    location = geo::Coordinate::parseFromJson(input);

    for (const auto& stop : details.at("road_distances").AsMap()) {
        distances[stop.first] = stop.second.AsInt();
    }
}

//======================================= GetBusInfo ===========================================//

ResponseHolder GetBusInfo::process(RouteManager& route_mgr) const
{
    ResponseHolder response = Response::create(Response::Type::BUS_INFO, request_id);
    auto& concrete_response = static_cast<BusInfoResponse&>(*response);

    concrete_response.set_bus_id(bus_id);    
    if (route_mgr.isTracking(bus_id)) {
        auto data = BusInfoResponse::Data();
        data.stop_cnt = route_mgr.getBusStopCount(bus_id);
        data.unique_stop_cnt = route_mgr.getBusUniqueStopCount(bus_id);
        data.route_len = route_mgr.getBusRouteLen(bus_id);
        data.curvature = route_mgr.getBusRouteCurvature(bus_id);
        concrete_response.set_data(data);
    }
    return response;
}

void GetBusInfo::parseFrom(string_view input)
{
    bus_id = input;
}

void GetBusInfo::parseFrom(const Json::Node& input)
{
    bus_id = input.AsMap().at("name").AsString();
    request_id = input.AsMap().at("id").AsInt();
}

//======================================= GetStopInfo ===========================================//

ResponseHolder GetStopInfo::process(RouteManager& route_mgr) const
{
    ResponseHolder response = Response::create(Response::Type::STOP_INFO, request_id);
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

void GetStopInfo::parseFrom(const Json::Node& input)
{
    stop_id = input.AsMap().at("name").AsString();
    request_id = input.AsMap().at("id").AsInt();
}

//====================================== GetRouteInfo ==========================================//

ResponseHolder GetRouteInfo::process(RouteManager& route_mgr) const
{
    ResponseHolder response = Response::create(Response::Type::ROUTE_INFO, request_id);
    auto& concrete_response = static_cast<RouteInfoResponse&>(*response);
    
    concrete_response.set_wait_time((int)route_mgr.waitTime());
    auto path = route_mgr.shortestPath(from, to);
    if (path) {
        auto route_description = RouteInfoResponse::Data();
        route_description.total_time = path->weight;

        const auto* router = route_mgr.getRouter();
        if (!router) throw runtime_error("No router attached to RouteManager");
        const auto& graph = router->getGraph();

        for (size_t i = 0; i < path->edge_count; ++i) {
            auto edge_id = router->GetRouteEdge(path->id, i);
            const auto& edge = graph.GetEdge(edge_id);
            if (edge.span_count == 0) { // waiting for a bus
                RouteInfoResponse::WaitInfo info {};
                info.stop_id = route_mgr.vertexIdToStopId(edge.from);
                route_description.segments.push_back(move(info));
            }
            else {  // taking a ride
                RouteInfoResponse::RideInfo info {};
                info.bus_id = edge.bus_id;
                info.span_count = edge.span_count;
                info.time = edge.weight;
                route_description.segments.push_back(move(info));
            }
        }
        concrete_response.set_data(move(route_description));
    }
    return response;
}

void GetRouteInfo::parseFrom(const Json::Node& input)
{
    from = input.AsMap().at("from").AsString();
    to = input.AsMap().at("to").AsString();
    request_id = input.AsMap().at("id").AsInt();
}
