//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
using namespace std;

//the reflected sample classes woule be compiled into static library or shared library,
//and the library would be linked with this main program.

// include the test case
#include "test_reflection.hpp"

extern "C" void dummy();

int test_main(int argc, char* argv[])
{
    //enable dll link on Windows
    dummy();

    try {
        test_reflection();
    }
    catch(std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        throw;
    }

    return 0;
}
