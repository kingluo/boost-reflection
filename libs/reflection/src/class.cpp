//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_REFLECTION_SOURCE 1
#include <boost/reflection/reflection.hpp>

namespace boost { namespace reflection {
BOOST_REFLECTION_DYNC_SYMBOL Class::db_t* Class::getDb()
{
    static db_t* db = 0;
    if (!db) {
        db = new db_t;
    }
    return db;
}
}}

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

BOOST_REFL_CONTAINER((short))
BOOST_REFL_CONTAINER((ushort))
BOOST_REFL_CONTAINER((int))
BOOST_REFL_CONTAINER((uint))
BOOST_REFL_CONTAINER((long))
BOOST_REFL_CONTAINER((ulong))
BOOST_REFL_CONTAINER((float))
BOOST_REFL_CONTAINER((double))
BOOST_REFL_CONTAINER((std)(string))
