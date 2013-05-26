//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
using namespace std;

#include <boost/config.hpp>

#if defined(BOOST_WINDOWS)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <boost/test/minimal.hpp>

// include the test case
#include "test_reflection.hpp"

int test_main(int argc, char* argv[])
{
    const char* library = argv[1];
    try {
        //dynamic load the sample classes
#if defined(BOOST_WINDOWS)
        HMODULE libHandle = LoadLibraryA (library);
#else
        void* libHandle = dlopen(library, RTLD_NOW);
#endif

        test_reflection();

#if defined(BOOST_WINDOWS)
        FreeLibrary(libHandle);
#else
        dlclose(libHandle);
#endif

        try {
            Class t = Class::forName("test.AccountImpl");
            BOOST_FAIL("test.AccountImpl should be non-exist");
        }
        catch(ClassNotFoundException&) {
            //correct branch
        }
    }
    catch(std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        throw;
    }

    return 0;
}
