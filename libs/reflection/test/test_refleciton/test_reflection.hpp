//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TEST_REFLECTION_HPP
#define TEST_REFLECTION_HPP

#include <boost/test/minimal.hpp>

#include <boost/reflection/reflection.hpp>
using namespace boost::reflection;

#include "test_interface.hpp"

void test_reflection()
{
    {
        Class t = Class::forName("test.AccountImpl");
        boost::any o = t.newInstance(std::string("Account-id-100"), (unsigned int)26);
        BOOST_REQUIRE(!o.empty());

        t.invokeStatic("echo", o, std::string("TEST"));

        TypeInfoArray tArray;
        tArray.push_back(BOOST_REFL_TYPEID(std::string));
        Method m1 = t.getMethod("setId", tArray);
        m1.invoke(o, std::string("Account-id-200"));

        Method m2 = t.getMethod("getId");
        boost::any ret = m2.invoke(o);
        BOOST_REQUIRE(boost::any_cast<std::string>(ret) == "Account-id-200");

        Field field = t.getField("age");
        field.set(o, (unsigned int)30);
        BOOST_REQUIRE(boost::any_cast<unsigned int>(field.get(o)) == 30);

        test::AccountItf* itf = interface_cast<test::AccountItf*>(o);
        BOOST_REQUIRE(itf);
        BOOST_REQUIRE(itf->getId() == "Account-id-200");

        t.deleteInstance(o);
    }

    {
        Class t1 = Class::forName("test.AccountEchoService");
        TypeInfoArray tArray;
        tArray.push_back(BOOST_REFL_TYPEID(test::AccountItf*));
        tArray.push_back(BOOST_REFL_TYPEID(Deleter<test::AccountItf>));
        Method m1 = t1.getMethod("setImpl", tArray);
        Method m2 = t1.getMethod("getImpl");
        Class t2 = Class::forName("test.AccountImpl");
        Class t3 = Class::forName("test.AnotherAccountImpl");
        test::AccountEchoService* o = t1.newInstance<test::AccountEchoService*>();

        test::AccountItf* someImpl = t2.newInstance<test::AccountItf*>(std::string("Account-id-888"), (unsigned int)26);
        m1.invoke(o, someImpl, t2.getDeleter<test::AccountItf>(someImpl));
        BOOST_REQUIRE(boost::any_cast<test::AccountItf*>(m2.invoke(o)) == someImpl);
        o->echo(std::string("some Account"));

        test::AccountItf* anotherImpl = t3.newInstance<test::AccountItf*>(std::string("Account-id-999"));
        m1.invoke(o, anotherImpl, t3.getDeleter<test::AccountItf>(anotherImpl));
        BOOST_REQUIRE(boost::any_cast<test::AccountItf*>(m2.invoke(o)) == anotherImpl);
        o->echo(std::string("another Account"));

        t1.deleteInstance(o);
    }
}

#endif
