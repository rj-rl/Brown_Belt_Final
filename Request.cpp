#include "Request.h"

using namespace std;

RequestHolder Request::create(Request::Type type)
{
    switch (type) {
    case Request::Type::ADD_BUS:
        return make_unique<AddBusRequest>();
    case Request::Type::ADD_STOP:
        return make_unique<AddStopRequest>();
    default:
        return nullptr;
    }
}

optional<Request::Type> convertRequestTypeFromString(string_view type_str)
{
    if (const auto it = STR_TO_REQUEST_TYPE.find(type_str);
        it != STR_TO_REQUEST_TYPE.end()) {
        return it->second;
    }
    else {
        return nullopt;
    }
}

RequestHolder parseRequest(string_view request_str)
{
    const auto request_type = convertRequestTypeFromString(readToken(request_str));
    if (!request_type) {
        return nullptr;
    }
    RequestHolder request = Request::create(*request_type);
    if (request) {
        request->parseFrom(request_str);
    };
    return request;
}

vector<RequestHolder> readRequests(istream& in)
{
    const size_t request_count = readNumberOnLine<size_t>(in);

    vector<RequestHolder> requests;
    requests.reserve(request_count);

    for (size_t i = 0; i < request_count; ++i) {
        string request_str;
        getline(in, request_str);
        if (auto request = parseRequest(request_str)) {
            requests.push_back(move(request));
        }
    }
    return requests;
}

void processRequests(const vector<RequestHolder>& requests)
{
    // PLACEHOLDER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

void printResponses(ostream& out)
{
    // PLACEHOLDER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}