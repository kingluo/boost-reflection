//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <list>

#include <boost/test/minimal.hpp>
#include <boost/reflection/reflection.hpp>
using namespace boost::reflection;

namespace test {
struct FooClass {
    std::string id;
    std::list<std::string> desc;
    FooClass(std::string id) : id(id) {}
    void echo() {
        std::cout << id << std::endl;
    }
};
BOOST_REFL_CLASS (
    (test)(FooClass),
    //fields
    BOOST_REFL_FIELD(desc, std::list<std::string>)
    //constructors
    BOOST_REFL_CONSTRUCTOR(std::string id)
    //methods
    BOOST_REFL_METHOD(echo, void())
)
}

int test_main(int argc, char* argv[])
{
    {
        std::list<test::FooClass*> hlist;
        Container c = Class::forContainerName(Container::normalizeName("list<test.FooClass *>"));
        c.add(&hlist, new test::FooClass("object-01"));
        c.add(&hlist, new test::FooClass("object-02"));
        BOOST_REQUIRE(c.count(&hlist) == 2);
        Iterator i = c.iterator(&hlist);
        Iterator i2 = i;
        i2.next();
        while(i.hasNext()) {
            i.next();
            test::FooClass* o = boost::any_cast<test::FooClass*>(i.value());
            o->echo();
            delete o;
            i.remove();
        }
        BOOST_REQUIRE(c.count(&hlist) == 0);
    }
    {
        std::map<std::string, test::FooClass*> hmap;
        Container c = Class::forContainerName(Container::normalizeName("map<test.FooClass *>"));
        c.add(&hmap, new test::FooClass("object-01"), std::string("object-01 key"));
        c.add(&hmap, new test::FooClass("object-02"), std::string("object-02 key"));
        BOOST_REQUIRE(c.count(&hmap) == 2);
        Iterator i = c.iterator(&hmap);
        Iterator i2 = i;
        i2.next();
        while(i.hasNext()) {
            i.next();
            test::FooClass* o = boost::any_cast<test::FooClass*>(i.value());
            std::cout << boost::any_cast<std::string>(i.key()) << std::endl;
            o->echo();
            delete o;
            i.remove();
        }
        BOOST_REQUIRE(c.count(&hmap) == 0);
    }

    {
        Class t = Class::forName("test.FooClass");
        boost::any o = t.newInstance(std::string("object-007"));
        Field p = t.getField("desc");
        boost::any lst = p.getPointer(o);
        Container c = Class::forContainerObject(lst);
        c.add(lst, std::string("string-01"));
        c.add(lst, std::string("string-02"));
        BOOST_REQUIRE(c.count(lst) == 2);
        Iterator i = c.iterator(lst);
        while(i.hasNext()) {
            i.next();
            std::string pStr = boost::any_cast<std::string>(i.value());
            std::cout << pStr << std::endl;
        }
        t.deleteInstance(o);
    }

    return 0;
}
