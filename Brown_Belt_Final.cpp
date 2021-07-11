//==============================================================================================//
//                                                                                              //
//	https://www.coursera.org/learn/c-plus-plus-brown/programming/ZfEFg/transportnyi-spravochnik-chast-d //
//                                                                                              //
//==============================================================================================//

//#include "profile.h"
//#include "test_runner.h"
//#include "tests.h"
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
    //TestRunner tr;
    //RUN_TEST(tr, test_basic_A);
    //RUN_TEST(tr, test_basic_B);
    //RUN_TEST(tr, test_basic_C);
    //RUN_TEST(tr, test_basic_D);

    setOutPrecision(6);

    const auto requests = readRequestsJson();
    const auto query_responses = processRequests(requests, requests);
    printResponsesJSON(query_responses);

    return 0;
}
 