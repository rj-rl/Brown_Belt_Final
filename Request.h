#pragma once
#include "util.h"
#include "geo.h"
#include "BusPark.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <optional>

//======================================= RESPONSE =============================================//

struct Response {
    struct Data {
        int     stop_cnt = 0;
        int     unique_stop_cnt = 0;
        double  route_len = 0.0;
    };

    Id id = 0;
    std::optional<Data> data = std::nullopt;

    Response(Id id = -1)
        : id {id}
    {}
};

//======================================= REQUESTS =============================================//

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
    enum class Type {
        ADD_STOP,
        ADD_BUS,
        GET_BUS_INFO,
    };

    Request(Type type) : type(type) {}
    static RequestHolder create(Type type);
    virtual void parseFrom(std::string_view input) = 0;
    virtual ~Request() = default;

    const Type type;
};

using TypeTable = std::unordered_map<std::string_view, Request::Type>;

static const TypeTable STR_TO_REQUEST_TYPE = {
    {"Stop", Request::Type::ADD_STOP},
    {"Bus", Request::Type::ADD_BUS},
};

static const TypeTable STR_TO_QUERY_TYPE = {
    {"Bus", Request::Type::GET_BUS_INFO},
};



template <typename ResultType>
struct QueryRequest : Request {
    using Request::Request;
    virtual ResultType process() const = 0;
};

struct ModifyRequest : Request {
    using Request::Request;
    virtual void process() const = 0;
};

// CONCRETE TYPES BELOW

struct AddBusRequest : ModifyRequest {
    Id              bus_id;
    geo::Coordinate coordinates;

    AddBusRequest()
        : ModifyRequest(Request::Type::ADD_BUS)
        , bus_id {}
        , coordinates {}
    {}

    void process() const override;
    void parseFrom(std::string_view) override;
};

struct AddStopRequest : ModifyRequest {
    AddStopRequest()
        : ModifyRequest(Request::Type::ADD_STOP)
    {}

    void process() const override;
    void parseFrom(std::string_view) override;
};


struct GetBusInfo : QueryRequest<Response> {
    Id bus_id;

    GetBusInfo()
        : QueryRequest(Request::Type::GET_BUS_INFO)
        , bus_id {}
    {}

    Response process() const override;
    void parseFrom(std::string_view) override;
};

//======================================= UTILITY ==============================================//

enum class IS_QUERY {
    NO,
    YES,
};

std::optional<Request::Type> convertRequestTypeFromString(
    std::string_view type_str, const TypeTable& table);

RequestHolder parseRequest(std::string_view request_str, IS_QUERY is_query);

template <IS_QUERY is_query>
std::vector<RequestHolder> readRequests(std::istream& in = std::cin)
{
    const size_t request_count = readNumberOnLine<size_t>(in);

    std::vector<RequestHolder> requests;
    requests.reserve(request_count);

    for (size_t i = 0; i < request_count; ++i) {
        std::string request_str;
        std::getline(in, request_str);
        if (auto request = parseRequest(request_str, is_query)) {
            requests.push_back(move(request));
        }
    }
    return requests;
}

std::vector<Response> processRequests(const std::vector<RequestHolder>& requests);

void printResponses(const std::vector<Response>&, std::ostream& stream = std::cout);