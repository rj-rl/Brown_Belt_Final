#pragma once
#include "util.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <optional>

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
    enum class Type {
        ADD_STOP,
        ADD_BUS,
    };

    Request(Type type) : type(type) {}
    static RequestHolder create(Type type);
    virtual void parseFrom(std::string_view input) = 0;
    virtual ~Request() = default;

    const Type type;
};

static const std::unordered_map<std::string_view, Request::Type> STR_TO_REQUEST_TYPE = {
    {"Stop", Request::Type::ADD_STOP},
    {"Bus", Request::Type::ADD_BUS},
};

template <typename ResultType>
struct ReadRequest : Request {
    using Request::Request;
    virtual ResultType process() const = 0;
};

struct ModifyRequest : Request {
    using Request::Request;
    virtual void process() const = 0;
};

struct AddBusRequest : ModifyRequest {
    AddBusRequest(Request::Type type)
        : ModifyRequest(type)
    {}

    void process() const override;
};

struct AddStopRequest : ModifyRequest {
    AddStopRequest(Request::Type type)
        : ModifyRequest(type)
    {}

    void process() const override;
};

//======================================= UTILITY ==============================================//

std::optional<Request::Type> convertRequestTypeFromString(std::string_view type_str);

RequestHolder parseRequest(std::string_view request_str);

std::vector<RequestHolder> readRequests(std::istream& in_stream = std::cin);

void processRequests(const std::vector<RequestHolder>& requests);

void printResponses(std::ostream& stream = std::cout);