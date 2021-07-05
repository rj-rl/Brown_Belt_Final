//==============================================================================================//
//                                                                                              //
//	https://www.coursera.org/learn/c-plus-plus-brown/programming/ZthPn/transportnyi-spravochnik-chast-a //
//                                                                                              //
//==============================================================================================//

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
    setOutPrecision();

    const auto fill_requests  = readRequests(); // filling the DB
    const auto query_requests = readRequests(); // reading DB queries
   
    return 0;
}
