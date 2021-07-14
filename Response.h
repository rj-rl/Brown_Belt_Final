#pragma once
#include <iostream>
#include <memory>
#include <utility>
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <string_view>

//======================================= RESPONSE =============================================//

using BusId = std::string;
using StopId = std::string;

struct Response;
using ResponseHolder = std::unique_ptr<Response>;

struct Response {
    enum class Type {
        BUS_INFO,
        STOP_INFO,
        ROUTE_INFO,
    };
    const Type type;
    const size_t request_id;

    Response(Type type, size_t request_id = 0) : type {type}, request_id {request_id} {}
    static ResponseHolder create(Type type, size_t request_id = 0);
    virtual void print(std::ostream& out) const = 0;
    virtual void printJson(std::ostream& out) const = 0;
    virtual ~Response() = default;
};


struct BusInfoResponse : Response {
    struct Data {
        size_t  stop_cnt = 0;
        size_t  unique_stop_cnt = 0;
        double  route_len = 0.0;
        double  curvature = 1.0;

        Data() = default;
    };
    BusId bus_id = {};
    std::optional<Data> data = std::nullopt;

    BusInfoResponse(size_t request_id)
        : Response(Response::Type::BUS_INFO, request_id)
    {
    }
    void set_bus_id(BusId bus_id) 
    {
        this->bus_id = std::move(bus_id);
    }
    void set_data(Data data) 
    { 
        this->data = data; 
    }
    void print(std::ostream& out) const override;
    void printJson(std::ostream& out) const override;
};


struct StopInfoResponse : Response {
    StopId stop_id = {};
    std::optional<std::vector<std::string>> data = std::nullopt;

    StopInfoResponse(size_t request_id)
        : Response(Response::Type::STOP_INFO, request_id)
    {
    }
    void set_stop_id(StopId stop_id) 
    { 
        this->stop_id = std::move(stop_id); 
    }
    void set_data(std::vector<std::string> data)
    {
        this->data = std::move(data); 
    }
    void print(std::ostream& out) const override;
    void printJson(std::ostream& out) const override;
};


struct RouteInfoResponse : Response {
    struct WaitInfo {
        StopId  stop_id = {};
    }; 
    struct RideInfo {
        BusId   bus_id = {};
        int     span_count = 0;
        double  time = 0.0;
    };
    struct Data {
        double total_time = 0.0;
        std::vector<std::variant<WaitInfo, RideInfo>> segments;
    };

    int                 wait_time = 0;
    std::optional<Data> data = std::nullopt;

    RouteInfoResponse(size_t request_id)
        : Response(Response::Type::ROUTE_INFO, request_id)
    {
    }

    void set_wait_time(int wait_time)
    {
        this->wait_time = wait_time;
    }
    void set_data(Data data)
    {
        this->data = std::move(data);
    }
    void print(std::ostream& out) const override { /* not implemented */ }
    void printJson(std::ostream& out) const override;
};