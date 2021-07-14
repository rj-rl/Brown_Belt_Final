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
    case Response::Type::ROUTE_INFO:
        return make_unique<RouteInfoResponse>(request_id);
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

void RouteInfoResponse::printJson(std::ostream& out) const
{
    out << "{\n"
        << R"("request_id": )" << request_id << ",\n";
    if (data) {
        out << R"("total_time": )" << secondsToMinutes(data->total_time) << ",\n"
            << R"("items": )""[";
        if (data->segments.size() > 0) out << '\n';
        {   // printing variant<WaitInfo, RideInfo>> segments
            bool first = true;
            for (const auto& segment : data->segments) {
                if (first) first = false;
                else out << ", \n";
                out << "{\n";
                if (holds_alternative<WaitInfo>(segment)) {
                    out << R"("type": "Wait",)""\n"
                        << R"("stop_name": )"
                        << "\"" << get<WaitInfo>(segment).stop_id << "\",\n"
                        << R"("time": )" << secondsToMinutes(wait_time) << '\n';
                }
                else {
                    const auto& ride_info = get<RideInfo>(segment);
                    out << R"("type": "Bus",)""\n"
                        << R"("bus": )"
                        << "\"" << ride_info.bus_id << "\",\n"
                        << R"("span_count": )" << ride_info.span_count << ",\n"
                        << R"("time": )" 
                        << secondsToMinutes(ride_info.time) << '\n';
                }
                out << "}";
            }
            if (!first) out << '\n';
        }
        out << "]\n";
    }
    else {
        out << R"("error_message": "not found")" << '\n';
    }
    out << '}';
}
