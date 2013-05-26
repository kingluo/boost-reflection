//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MVC_DISPATCHER_INTERFACE_HPP
#define MVC_DISPATCHER_INTERFACE_HPP

#include "mvc_common.hpp"
class DispatcherServlet {
public:
    virtual void handle(ServletRequest* req, ServletResponse* res) = 0;
    virtual ~DispatcherServlet() {}
};

BOOST_REFL_INTERFACE(
    (DispatcherServlet),
    BOOST_REFL_INTERFACE_METHOD(handle, void,
        BOOST_REFL_INTERFACE_METHOD_PARAMS(
            BOOST_REFL_INTERFACE_METHOD_PARAM(ServletRequest*, req)
            BOOST_REFL_INTERFACE_METHOD_PARAM(ServletResponse*, res)
        )
    )
)

#endif
