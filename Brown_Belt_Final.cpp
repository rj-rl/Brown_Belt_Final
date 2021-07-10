//==============================================================================================//
//                                                                                              //
//	https://www.coursera.org/learn/c-plus-plus-brown/programming/ZthPn/transportnyi-spravochnik-chast-a //
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
    //RUN_TEST(tr, test_from_chars);
    //RUN_TEST(tr, test_basic_A);
    //RUN_TEST(tr, test_basic_B);
    //RUN_TEST(tr, test_basic_C);

    setOutPrecision(6);

    const auto modify_requests  = readRequests<RequestCategory::MODIFY>(); // filling the DB
    const auto query_requests = readRequests<RequestCategory::READ>(); // reading DB queries
    const auto query_responses = processRequests(modify_requests, query_requests);
    printResponses(query_responses);

    return 0;
}
 