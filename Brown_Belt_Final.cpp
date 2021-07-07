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
    //TestRunner tr;
    //RUN_TEST(tr, test_stop_hash);

    setOutPrecision();

    try {
    const auto modify_requests  = readRequests<QueryType::MODIFY>(); // filling the DB
    const auto query_requests = readRequests<QueryType::READ>(); // reading DB queries
        const auto query_responses = processRequests(modify_requests, query_requests);
        printResponses(query_responses);
    }
    catch (exception& e) {
        cout << e.what();
    }
    return 0;
}
 