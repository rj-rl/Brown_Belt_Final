#include "tests.h"
#include "test_runner.h"
#include "profile.h"
#include "geo.h"
#include "util.h"
#include "Request.h"
#include "Map.h"
#include "BusPark.h"

#include <iostream>
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