//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_REFLECTION_TEST_DRIVER_INTERFACE_HPP
#define BOOST_REFLECTION_TEST_DRIVER_INTERFACE_HPP

#include <string>

struct DriverInterface {
    virtual bool matchDevice(std::string devId) = 0;
    virtual void doTask(std::string devId) = 0;
    virtual ~DriverInterface() {}
};

BOOST_REFL_INTERFACE(
    (DriverInterface),
    BOOST_REFL_INTERFACE_METHOD(matchDevice, bool,
        BOOST_REFL_INTERFACE_METHOD_PARAMS(
            BOOST_REFL_INTERFACE_METHOD_PARAM(std::string, devId)
        )
    )
    BOOST_REFL_INTERFACE_METHOD(doTask, void,
        BOOST_REFL_INTERFACE_METHOD_PARAMS(
            BOOST_REFL_INTERFACE_METHOD_PARAM(std::string, devId)
        )
    )
)

#endif
