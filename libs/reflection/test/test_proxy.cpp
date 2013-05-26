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

class TestInterface1 {
public:
    virtual void doTask(long count) = 0;
    virtual ~TestInterface1() {}
};

BOOST_REFL_INTERFACE(
    (test)(TestInterface1),
    BOOST_REFL_INTERFACE_METHOD(doTask, void,
        BOOST_REFL_INTERFACE_METHOD_PARAMS(
            BOOST_REFL_INTERFACE_METHOD_PARAM(long, count)
        )
    )
)

class TestInterface2 {
public:
    virtual std::string getId() = 0;
    virtual void setId(std::string id) = 0;
    virtual ~TestInterface2() {}
};

BOOST_REFL_INTERFACE(
    (test)(TestInterface2),
    BOOST_REFL_INTERFACE_METHOD(getId, std::string,
        BOOST_REFL_INTERFACE_METHOD_NO_PARAMS
    )
    BOOST_REFL_INTERFACE_METHOD(setId, void,
        BOOST_REFL_INTERFACE_METHOD_PARAMS(
            BOOST_REFL_INTERFACE_METHOD_PARAM(std::string, id)
        )
    )
)

class TestClass : public TestInterface1, public TestInterface2 {
public:
    TestClass() : id("initial_id") {}
    virtual void doTask(long count) {
        for(long i = 0; i < count; i++) {
            std::cout << "doTask, id=" << id << ", count=" << i << std::endl;
        }
    }
    virtual std::string getId() {
        return id;
    }
    virtual void setId(std::string id) {
        this->id = id;
    }
private:
    std::string id;
};

BOOST_REFL_CLASS (
    (test)(TestClass),
    //derived Intefaces
    BOOST_REFL_IMPLEMENT_INTERFACE( (test)(TestInterface1) )
    BOOST_REFL_IMPLEMENT_INTERFACE( (test)(TestInterface2) )
    //constructors
    BOOST_REFL_CONSTRUCTOR()
)

}

namespace test_driver {
struct LogInvocation : public InvocationHandler {
    Class clazz;
    boost::any obj;
    int invCount;
    LogInvocation(Class clazz, boost::any obj) : clazz(clazz), obj(obj), invCount(0) {}
    virtual boost::any invoke(boost::any proxy, std::string method, std::vector<boost::any> args) {
        (void)proxy;
        std::cout << "--> invoking " << method << ", count=" << invCount << std::endl;
        boost::any ret = clazz.invoke(obj, method, args);
        std::cout << "<-- invoking " << method << ", count=" << invCount << std::endl;
        invCount++;
        return ret;
    }
};

class InjectMethod : public InvocationHandler {
    Class clazz;
    boost::any obj;
public:
    InjectMethod(Class clazz, boost::any obj) : clazz(clazz), obj(obj) {}
    virtual boost::any invoke(boost::any proxy, std::string method, std::vector<boost::any> args) {
        (void)proxy;
        boost::any ret = clazz.invoke(obj, method, args);
        if (method == "getId") {
            ret = boost::any_cast<std::string>(ret) + "@InjectMethod";
        }
        return ret;
    }
};
}

int test_main(int argc, char* argv[])
{
    try {
        Class t = Class::forName("test.TestClass");
        boost::any o = t.newInstance();
        BOOST_REQUIRE(!o.empty());
        BOOST_REFL_ANY_OBJECT_GUARD(t, o);

        {
            test_driver::LogInvocation logInv(t, o);
            ProxyBase* p = t.createProxy(&logInv);
            test::TestInterface1* p1 = dynamic_cast<test::TestInterface1*>(p);
            p1->doTask(8);
            BOOST_REQUIRE(logInv.invCount == 1);
            test::TestInterface2* p2 = dynamic_cast<test::TestInterface2*>(p);
            p2->setId("id#232");
            BOOST_REQUIRE(p2->getId() == "id#232");
            BOOST_REQUIRE(logInv.invCount == 3);
        }

        {
            test_driver::InjectMethod injMethod(t, o);
            test::TestInterface2* p2 = dynamic_cast<test::TestInterface2*>(t.createProxy(&injMethod));
            p2->setId("id#232");
            BOOST_REQUIRE(p2->getId() == "id#232@InjectMethod");
        }
    }
    catch(std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        BOOST_FAIL("should be no exception");
    }

    return 0;
}
