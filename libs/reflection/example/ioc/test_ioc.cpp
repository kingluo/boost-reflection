//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <list>
#include <string>

#include <boost/reflection/reflection.hpp>
using namespace boost::reflection;

#include <boost/foreach.hpp>

#include "driver_interface.hpp"
#include "ioc.hpp"

#if defined(BOOST_WINDOWS)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

namespace test {
struct DriverManager {
    std::list<DriverInterface*> drivers;
    void handleDevice(std::string devId)
    {
        BOOST_FOREACH(DriverInterface* drv, drivers) {
            if (drv->matchDevice(devId)) {
                drv->doTask(devId);
            }
        }
    }
};

BOOST_REFL_CLASS (
    (test)(DriverManager),
    BOOST_REFL_CONSTRUCTOR()
    BOOST_REFL_FIELD(drivers, std::list<DriverInterface*>)
)
}

int main(int argc, const char* argv[])
{
    try {
        std::list<std::string> xmls;
        std::list<std::string> libs;
        for(int i = 0; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-xml") {
                xmls.push_back(argv[++i]);
            } else if (arg == "-lib") {
                libs.push_back(argv[++i]);
            }
        }

        BOOST_FOREACH(std::string& v, libs) {
            const char* lib = v.c_str();
#if defined(BOOST_WINDOWS)
            LoadLibraryA(lib);
#else
            dlopen(lib, RTLD_NOW);
#endif
        }

        ApplicationContext* ctx = new ApplicationContext(xmls);
        test::DriverManager* drvMgr = ctx->getBean<test::DriverManager*>("driverManager");
        drvMgr->handleDevice("999-777-344");
        drvMgr->handleDevice("000-111-333");
    } catch(std::exception& exc) {
        std::cout << exc.what() << std::endl;
        throw;
    }
    return 0;
}
