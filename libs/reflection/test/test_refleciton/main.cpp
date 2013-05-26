//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
using namespace std;

// compile the reflected sample classes together with the main program
#include "sample_classes.hpp"

// include the test case
#include "test_reflection.hpp"

int test_main(int argc, char* argv[])
{
    try {
        test_reflection();
    }
    catch(std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        throw;
    }

    return 0;
}
