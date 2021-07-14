#include "tests.h"
#include "test_runner.h"
#include "profile.h"
#include "geo.h"
#include "util.h"
#include "Request.h"
#include "Map.h"
#include "BusPark.h"
#include "json.h"
#include "graph.h"
#include "router.h"

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
using namespace Graph;

void test_router()
{
	DirectedWeightedGraph<int> graph(5);
	//			   A  B  distance
	graph.AddEdge({0, 1,    10});	// 0th edge
	graph.AddEdge({0, 2,    1});	// 1
	graph.AddEdge({2, 3,    2});	// 2
	graph.AddEdge({3, 2,    2});	// 3
	graph.AddEdge({3, 1,    4});	// 4
	Router<int> router(graph);
	auto path = router.BuildRoute(0, 1);
	for (size_t i = 0; i < path->edge_count; ++i) {
		cout << "edge # " << router.GetRouteEdge(path->id, i) << '\n';
	}
}

void test_routing_settings()
{
	ifstream input("Tests/test_routing_settings_input.txt");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto& concrete_type_parsed_requests =
		static_cast<AddRoutingSettings&>(*parsed_requests.routing_settings_request);
	ASSERT_EQUAL(concrete_type_parsed_requests.bus_wait_time, 6 * 60);
	ASSERT_EQUAL(concrete_type_parsed_requests.bus_velocity, 40 / 3.6);
}

void test_SI_units()
{
	double distance = 2600 + 890;
	ASSERT_EQUAL(5.235, secondsToMinutes(distance/kphToMps(40)));
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

void test_basic_D()
{
	ifstream correct_output_file("Tests/test_basic_D_correct_output.txt");
	string correct_output {
		(istreambuf_iterator<char>(correct_output_file)),
		istreambuf_iterator<char>()
	};
	ifstream input("Tests/test_JSON_input.txt");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto query_responses = processRequests(parsed_requests, parsed_requests);

	ostringstream output;
	printResponsesJSON(query_responses, output);
	ASSERT_EQUAL(output.str(), correct_output);
}

void test_basic_E_example_1()
{
	ifstream input("Tests/test_E_example_1.txt");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto query_responses = processRequests(parsed_requests, parsed_requests);
	printResponsesJSON(query_responses, cout);
}

void test_basic_E_example_2()
{
	ifstream input("Tests/test_E_example_2.txt");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto query_responses = processRequests(parsed_requests, parsed_requests);
	printResponsesJSON(query_responses, cout);
}

void test_basic_E_example_3()
{
	ifstream input("Tests/test_E_example_3.txt");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto query_responses = processRequests(parsed_requests, parsed_requests);
	printResponsesJSON(query_responses, cout);
}

void test_basic_E_example_4()
{
	ifstream input("Tests/test_E_example_4_input.json");
	RequestsContainer parsed_requests = readRequestsJson(input);
	const auto query_responses = processRequests(parsed_requests, parsed_requests);
	printResponsesJSON(query_responses, cout);
}
