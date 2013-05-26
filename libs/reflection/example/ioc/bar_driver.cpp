//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <list>
#include <iostream>

#include <boost/reflection/reflection.hpp>
#include <boost/foreach.hpp>

#include "driver_interface.hpp"

namespace bar {
struct BarDriver : public DriverInterface {
    std::list<std::string> supportDeviceId;
    BarDriver(std::list<std::string> id) : supportDeviceId(id) {}
    virtual bool matchDevice(std::string devId) {
        BOOST_FOREACH(std::string& id, supportDeviceId) {
            if (id == devId) {
                return true;
            }
        }
        return false;
    }
    virtual void doTask(std::string devId) {
        std::cout << "BarDriver::doTask " << devId << std::endl;
    }
};
}

BOOST_REFL_CLASS (
    (bar)(BarDriver),
    BOOST_REFL_IMPLEMENT_INTERFACE( (DriverInterface) )
    BOOST_REFL_CONSTRUCTOR(std::list<std::string>)
)
