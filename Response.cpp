#include "Response.h"
#include "util.h"
using namespace std;

ResponseHolder Response::create(Type type)
{
    switch (type) {
    case Response::Type::BUS_INFO:
        return make_unique<BusInfoResponse>();
    case Response::Type::STOP_INFO:
        return make_unique<StopInfoResponse>();
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

}
