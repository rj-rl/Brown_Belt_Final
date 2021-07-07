//==============================================================================================//
//                                                                                              //
//	https://www.coursera.org/learn/c-plus-plus-brown/programming/ZthPn/transportnyi-spravochnik-chast-a //
//                                                                                              //
//==============================================================================================//

#include "profile.h"
#include "test_runner.h"
#include "tests.h"
#include "util.h"
#include "geo.h"
#include "Request.h"
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
    TestRunner tr;
    RUN_TEST(tr, test_stop_hash);

    setOutPrecision();

    const auto fill_requests  = readRequests<QueryType::MODIFY>(); // filling the DB
    const auto query_requests = readRequests<QueryType::READ>(); // reading DB queries

    processRequests(fill_requests);
    const auto query_responses = processRequests(query_requests);
    printResponses(query_responses);
   
    return 0;
}
 