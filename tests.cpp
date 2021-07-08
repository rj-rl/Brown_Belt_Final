#include "tests.h"
#include "test_runner.h"
#include "profile.h"
#include "geo.h"
#include "util.h"
#include "Request.h"
#include "Map.h"
#include "BusPark.h"

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

void test_stop_hash()
{
	unordered_set<Stop, Stop::Hasher> stops;
	Stop one("Kek", {1.1, 2.2});
	Stop another("Kek", {9.9, 9.9});

	stops.insert(one);
	stops.insert(another);
	ASSERT_EQUAL(stops.find(Stop("Kek"))->location, one.location);
}

// In the end: no whitespace, no noise allowed, string must only contain a valid
// representation of a number
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

void test_main()
{
	string correct_output =
		R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length)""\n"
		R"(Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length)""\n"
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
