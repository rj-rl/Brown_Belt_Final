#include "Response.h"
#include "util.h"
using namespace std;

ResponseHolder Response::create(Type type, size_t request_id)
{
    switch (type) {
    case Response::Type::BUS_INFO:
        return make_unique<BusInfoResponse>(request_id);
    case Response::Type::STOP_INFO:
        return make_unique<StopInfoResponse>(request_id);
    default:
        return nullptr;
    }
}

void BusInfoResponse::print(std::ostream& out) const
{
    out << "Bus " << bus_id << ": ";
    if (data) {
        out << data->stop_cnt << " stops on route, "
            << data->unique_stop_cnt << " unique stops, "
            << data->route_len << " route length, "
            << data->curvature << " curvature";
    }
    else {
        out << "not found";
    }
}

void BusInfoResponse::printJson(std::ostream& out) const
{
    out << "{\n";
    if (data) {
        out << R"("route_length": )" << data->route_len << ",\n"
            << R"("request_id": )" << request_id << ",\n"
            << R"("curvature": )" << data->curvature << ",\n"
            << R"("stop_count": )" << data->stop_cnt << ",\n"
            << R"("unique_stop_count": )" << data->unique_stop_cnt << '\n';
    }
    else {
        out << R"("request_id": )" << request_id << ",\n"
            << R"("error_message": "not found")" << '\n';
    }
    out << '}';
}

void StopInfoResponse::print(std::ostream& out) const
{
    out << "Stop " << stop_id << ": ";
    if (data) {
        if (data->empty()) {
            out << "no buses";
        }
        else {
            out << "buses ";
            print_list(data->begin(), data->end(), out);
        }
    }
    else {
        out << "not found";
    }
}

void StopInfoResponse::printJson(std::ostream& out) const
{
    out << "{\n";
    if (data) {
        out << R"("buses": )" << '[';
        if (data->size() > 0) out << '\n';
        print_list(data->begin(), data->end(), out, ",\n", "\"");
        if (data->size() > 0) out << '\n';
        out << "],\n"
            << R"("request_id": )" << this->request_id << '\n';
    }
    else {
        out << R"("request_id": )" << request_id << ",\n"
            << R"("error_message": "not found")" << '\n';
    }
    out << '}';
}
