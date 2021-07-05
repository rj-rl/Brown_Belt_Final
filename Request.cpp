#include "Request.h"

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
    vector<Response> result;

    for (const auto& request_holder : requests) {
        if (request_holder->type == Request::Type::GET_BUS_INFO) {
            const auto& request = static_cast<const GetBusInfo&>(*request_holder);
            result.push_back(request.process());
        }
        else {

        }
    }
    // PLACEHOLDER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
        out << "Bus " << response.id << ": ";
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

void AddBusRequest::process() const
{
}

void AddBusRequest::parseFrom(string_view input)
{
    bus_id = strToNum<Id>(readToken(input));
    coordinates.parseFromStr(input);
}

void AddStopRequest::process() const
{
}

void AddStopRequest::parseFrom(string_view input)
{
}

Response GetBusInfo::process() const
{
    Response response {bus_id};
    // PLACEHOLDER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return response;
}

void GetBusInfo::parseFrom(string_view input)
{
    bus_id = strToNum<Id>(readToken(input));
}
