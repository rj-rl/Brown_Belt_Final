#pragma once
#include "util.h"
#include "geo.h"
#include "BusPark.h"
#include "RouteManager.h"
#include "Response.h"
#include "json.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <optional>
#include <utility>

//======================================= REQUESTS =============================================//

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
    enum class Type {
        ADD_STOP,
        ADD_BUS,
        GET_BUS_INFO,
        GET_STOP_INFO,
    };

    Request(Type type) : type(type) {}
    static RequestHolder create(Type type);
    virtual void parseFrom(std::string_view input) = 0;
    virtual void parseFrom(const Json::Node& input) = 0;
    virtual ~Request() = default;

    const Type type;
};

using TypeTable = std::unordered_map<std::string_view, Request::Type>;

static const TypeTable STR_TO_MODIFYING_REQUEST_TYPE = {
    {"Stop", Request::Type::ADD_STOP},
    {"Bus", Request::Type::ADD_BUS},
};
static const TypeTable STR_TO_QUERY_TYPE = {
    {"Bus", Request::Type::GET_BUS_INFO},
    {"Stop", Request::Type::GET_STOP_INFO},
};


template <typename ResultType>
struct QueryRequest : Request {
    using Request::Request;
    size_t request_id {0u};
    virtual ResultType process(RouteManager& route_mgr) const = 0;
};

struct ModifyRequest : Request {
    using Request::Request;
    virtual void process(RouteManager& route_mgr) = 0;
};

// CONCRETE TYPES BELOW

struct AddBusRequest : ModifyRequest {
    BusId                     bus_id;
    std::vector<std::string>  stop_names;
    Route::Type               route_type;
        
    AddBusRequest()
        : ModifyRequest(Request::Type::ADD_BUS)
        , bus_id {}
        , route_type {}
    {}

    void process(RouteManager& route_mgr) override;
    void parseFrom(std::string_view) override;
    void parseFrom(const Json::Node& input) override;
    void parseRouteType(std::string_view input, std::string* delimiter);
    void parseRouteType(const Json::Node& input);
};

struct AddStopRequest : ModifyRequest {
    std::string     name;
    geo::Coordinate location;
    std::unordered_map<StopId, double> distances;

    AddStopRequest()
        : ModifyRequest(Request::Type::ADD_STOP)
    {}

    void process(RouteManager& route_mgr) override;
    void parseFrom(std::string_view) override;
    void parseFrom(const Json::Node& input) override;
};


struct GetBusInfo : QueryRequest<ResponseHolder> {
    BusId bus_id;

    GetBusInfo()
        : QueryRequest(Request::Type::GET_BUS_INFO)
        , bus_id {}
    {}

    ResponseHolder process(RouteManager& route_mgr) const override;
    void parseFrom(std::string_view) override;
    void parseFrom(const Json::Node& input) override;
};


struct GetStopInfo : QueryRequest<ResponseHolder> {
    StopId stop_id;

    GetStopInfo()
        : QueryRequest(Request::Type::GET_STOP_INFO)
        , stop_id {}
    {}

    ResponseHolder process(RouteManager& route_mgr) const override;
    void parseFrom(std::string_view) override;
    void parseFrom(const Json::Node& input) override;
};

//================================== REQUESTS CONTAINER ========================================//

struct RequestsContainer {
    std::vector<RequestHolder> fill_map_requests;
    std::vector<RequestHolder> fill_bus_park_requests;
    std::vector<RequestHolder> queries;

    RequestsContainer() = default;
};

//======================================= UTILITY ==============================================//
// Determines if a request is a query to DB
enum class RequestCategory {
    MODIFY,
    READ,
};

std::optional<Request::Type> convertRequestTypeFromString(
    std::string_view type_str, const TypeTable& table);

RequestHolder parseRequest(std::string_view request_str, RequestCategory category);
RequestHolder parseRequestFromJson(const Json::Node& node, RequestCategory category);

template <RequestCategory category>
RequestsContainer readRequests(std::istream& in = std::cin)
{
    const size_t request_count = readNumberOnLine<size_t>(in);
    RequestsContainer requests;

    for (size_t i = 0; i < request_count; ++i) {
        std::string request_str;
        std::getline(in, request_str);
        if (auto request = parseRequest(request_str, category)) {
            if (request->type == Request::Type::ADD_BUS) {
                requests.fill_bus_park_requests.push_back(std::move(request));
            }
            else if (request->type == Request::Type::ADD_STOP) {
                requests.fill_map_requests.push_back(std::move(request));
            }
            else {
                requests.queries.push_back(std::move(request));
            }
        }
    }
    return requests;
}

RequestsContainer readRequestsJson(std::istream& input = std::cin);

std::vector<ResponseHolder> processRequests(
    const RequestsContainer& modify_requests,
    const RequestsContainer& query_requests);

void printResponses(const std::vector<ResponseHolder>&, std::ostream& stream = std::cout);
void printResponsesJSON(const std::vector<ResponseHolder>&, std::ostream& stream = std::cout);