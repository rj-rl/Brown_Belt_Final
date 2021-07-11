#include "tests.h"
#include "test_runner.h"
#include "profile.h"
#include "geo.h"
#include "util.h"
#include "Request.h"
#include "Map.h"
#include "BusPark.h"
#include "json.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <utility>
#include <optional>
#include <charconv>
using namespace std;

// In the end: no leading whitespace allowed
void test_from_chars()
{
	double result = 42.69;
	string d_num = "42.69";
	string_view sv {d_num};
	double d_converted;
	std::from_chars(sv.data(), sv.data()+sv.size(), d_converted);

	string d_num_ws = "   42.69 ";
	sv = d_num_ws;
	sv = strip_ws(sv);
	double d_ws_converted;
	std::from_chars(sv.data(), sv.data()+sv.size(), d_ws_converted);

	string d_num_noisy = "42.69abc,";
	sv = d_num_noisy;
	sv = strip_ws(sv);
	double d_ws_noisy_converted;
	std::from_chars(sv.data(), sv.data()+sv.size(), d_ws_noisy_converted);
		
	ASSERT_EQUAL(d_converted, result);
	ASSERT_EQUAL(d_ws_converted, result);
	ASSERT_EQUAL(d_ws_noisy_converted, result);
}

void test_basic_A()
{
	string correct_output =
		R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length, 1 curvature)""\n"
		R"(Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length, 1 curvature)""\n"
		R"(Bus 751: not found)""\n";

	setOutPrecision();
	ifstream input("Tests/test_basic.txt");
	ostringstream output;

	// cheekily ordered requests
	auto modify_requests = readRequests<RequestCategory::MODIFY>(input);
	auto query_requests = readRequests<RequestCategory::READ>(input);
	auto query_responses = processRequests(modify_requests, query_requests);
	printResponses(query_responses, output);
	ASSERT_EQUAL(output.str(), correct_output);

	// out of order requests
	output.str("");
	output.clear();
	modify_requests = readRequests<RequestCategory::MODIFY>(input);
	query_requests = readRequests<RequestCategory::READ>(input);
	query_responses = processRequests(modify_requests, query_requests);
	printResponses(query_responses, output);
	ASSERT_EQUAL(output.str(), correct_output);
}

void test_basic_B()
{
	string correct_output =
		R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length, 1 curvature)""\n"
		R"(Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length, 1 curvature)""\n"
		R"(Bus 751: not found)""\n"
		R"(Stop Samara: not found)""\n"
		R"(Stop Prazhskaya: no buses)""\n"
		R"(Stop Biryulyovo Zapadnoye: buses 256 828)""\n";

	setOutPrecision();
	ifstream input("Tests/test_basic_B.txt");
	ostringstream output;

	auto modify_requests = readRequests<RequestCategory::MODIFY>(input);
	auto query_requests = readRequests<RequestCategory::READ>(input);
	auto query_responses = processRequests(modify_requests, query_requests);
	printResponses(query_responses, output);
	ASSERT_EQUAL(output.str(), correct_output);
}

void test_basic_C()
{
	string correct_output =
		R"(Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature)""\n"
		R"(Bus 750: 5 stops on route, 3 unique stops, 27600 route length, 1.31808 curvature)""\n"
		R"(Bus 751: not found)""\n"
		R"(Stop Samara: not found)""\n"
		R"(Stop Prazhskaya: no buses)""\n"
		R"(Stop Biryulyovo Zapadnoye: buses 256 828)""\n";

	setOutPrecision();
	ifstream input("Tests/test_basic_C.txt");
	ostringstream output;

	auto modify_requests = readRequests<RequestCategory::MODIFY>(input);
	auto query_requests = readRequests<RequestCategory::READ>(input);
	auto query_responses = processRequests(modify_requests, query_requests);
	printResponses(query_responses, output);
	ASSERT_EQUAL(output.str(), correct_output);
}






optional<Request::Type> getRequestTypeFromJson(const Json::Node& node, const TypeTable& table)
{
	if (const auto it = table.find(node.AsMap().at("type").AsString());
		it != table.end()) {
		return it->second;
	}
	else {
		return nullopt;
	}
}

RequestHolder parseRequestFromJson(const Json::Node& node, RequestCategory category)
{
	const auto& details = node.AsMap();
	const TypeTable& table = (category == RequestCategory::READ)
		? STR_TO_QUERY_TYPE
		: STR_TO_MODIFYING_REQUEST_TYPE;
	const auto request_type = getRequestTypeFromJson(node, table);
	if (!request_type) {
		return nullptr;
	}
	RequestHolder request = Request::create(*request_type);
	if (request) {
		request->parseFrom(node);
	};
	return request;
}

RequestsContainer readRequestsJson(std::istream& input)
{
	RequestsContainer requests;
	Json::Document doc {Json::Load(input)};
	const auto& modify_requests = doc.GetRoot().AsMap().at("base_requests").AsArray();
	const auto& read_requests = doc.GetRoot().AsMap().at("stat_requests").AsArray();

	for (size_t i = 0; i < modify_requests.size(); ++i) {
		if (auto request = parseRequestFromJson(modify_requests[i], RequestCategory::MODIFY)) {
			if (request->type == Request::Type::ADD_BUS) {
				requests.fill_bus_park_requests.push_back(move(request));
			}
			else if (request->type == Request::Type::ADD_STOP) {
				requests.fill_map_requests.push_back(move(request));
			}
		}
	}
	for (size_t i = 0; i < read_requests.size(); ++i) {
		if (auto request = parseRequestFromJson(read_requests[i], RequestCategory::READ)) {
			requests.queries.push_back(move(request));
		}
	}
	return requests;
}

void test_JSON_input()
{
	ifstream input("Tests/test_JSON_input.txt");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto query_responses = processRequests(parsed_requests, parsed_requests);
	printResponses(query_responses);
}
