//==============================================================================================//
//                                                                                              //
//	https://www.coursera.org/learn/c-plus-plus-brown/programming/GBep2/transportnyi-spravochnik-chast-e //
//                                                                                              //
//==============================================================================================//

#include "profile.h"
#include "test_runner.h"
#include "tests.h"
#include "util.h"
#include "geo.h"
#include "Request.h"
#include "Response.h"
#include "Map.h"
#include "BusPark.h"

#include <iostream>
#include <iomanip>
using namespace std;

//==============================================================================================//


//==============================================================================================//
//==============================================================================================//

int main()
{
    setOutPrecision(6);

    TestRunner tr;
    //RUN_TEST(tr, test_router);
    //RUN_TEST(tr, test_routing_settings);
    //RUN_TEST(tr, test_SI_units);
    RUN_TEST(tr, test_basic_E_example_4);
    //RUN_TEST(tr, test_basic_A);
    //RUN_TEST(tr, test_basic_B);
    //RUN_TEST(tr, test_basic_C);
    //RUN_TEST(tr, test_basic_D);

    const auto requests = readRequestsJson();
    const auto query_responses = processRequests(requests, requests);
    printResponsesJSON(query_responses);

    return 0;
}
 