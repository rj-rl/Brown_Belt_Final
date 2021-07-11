#pragma once
#include <iostream>
#include <memory>
#include <utility>
#include <optional>
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
    };
    const Type type;

    Response(Type type) : type {type} {}
    static ResponseHolder create(Type type);
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

    BusInfoResponse()
        : Response(Response::Type::BUS_INFO)
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

    StopInfoResponse()
        : Response(Response::Type::STOP_INFO)
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