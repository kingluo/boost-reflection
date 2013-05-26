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
#include <boost/foreach.hpp>

// Annotation definitions
BOOST_REFL_DEF_ANNOTATION(
    (tags)(ControllerTag),
    BOOST_REFL_DEF_ANNOTATION_ELEM(std::list<std::string>, capabilities)
    BOOST_REFL_DEF_ANNOTATION_ELEM_DEFAULT(std::string, desc, "NO DESC")
)

struct ComponenetInfo {
    int id;
    std::string desc;
    ComponenetInfo() {}
    ComponenetInfo(int id, std::string desc) : id(id), desc(desc) {}
};

BOOST_REFL_DEF_ANNOTATION(
    (tags)(ComponentTag),
    BOOST_REFL_DEF_ANNOTATION_ELEM(ComponenetInfo, info)
    BOOST_REFL_DEF_ANNOTATION_ELEM(int, hash)
)

BOOST_REFL_DEF_SIMPLE_ANNOTATION(
    (tags)(DummyTag)
)

namespace foo {
struct MyController {
    void echo(std::string banner) {
        std::cout << "FooController echo: " << banner << std::endl;
    }
    void reset() {
        std::cout << "FooController reset" << std::endl;
    }
};
}

BOOST_REFL_CLASS_WITH_ANNOTATION (
    //class name sequence
    (foo)(MyController),
    //class-level annotation
    BOOST_REFL_ANNOTATION (
        (tags)(ControllerTag),
        (capabilities = make_list<std::string>("CAN_ECHO", "CAN_RESET"))
    )
    //another class-level annotation
    BOOST_REFL_ANNOTATION (
        (tags)(ComponentTag),
        (info = ComponenetInfo(1, "any description"))
        (hash = 23243)
    ),
    //default constructor with annotation
    BOOST_REFL_CONSTRUCTOR_WITH_ANNOTATION (
        (),
        BOOST_REFL_SIMPLE_ANNOTATION (
            (tags)(DummyTag)
        )
    )
    //methods
    BOOST_REFL_METHOD(echo, void(std::string banner))
    BOOST_REFL_METHOD(reset, void())
)

int test_main(int argc, char* argv[])
{
    try {
        Class t = Class::forName("foo.MyController");
        Constructor ctor = t.getConstructor();
        BOOST_REQUIRE(ctor.isAnnotationPresent<tags::DummyTag>());
        boost::any o = ctor.newInstance();
        BOOST_REFL_ANY_OBJECT_GUARD(t,o);
        if (t.isAnnotationPresent<tags::ControllerTag>()) {
            tags::ControllerTag tag = t.getAnnotation<tags::ControllerTag>();
            BOOST_REQUIRE(tag.desc == "NO DESC");
            BOOST_REQUIRE(tag.capabilities.size() == 2);
            BOOST_FOREACH(std::string capability, tag.capabilities) {
                if (capability == "CAN_RESET") {
                    Method m = t.getMethod("reset");
                    m.invoke(o);
                } else if (capability == "CAN_ECHO") {
                    TypeInfoArray tArray;
                    tArray.push_back(BOOST_REFL_TYPEID(std::string));
                    Method m = t.getMethod("echo", tArray);
                    m.invoke(o, std::string("test"));
                }
            }
        }
        if (t.isAnnotationPresent<tags::ComponentTag>()) {
            tags::ComponentTag tag = t.getAnnotation<tags::ComponentTag>();
            BOOST_REQUIRE(tag.info.id == 1);
            BOOST_REQUIRE(tag.info.desc == "any description");
            BOOST_REQUIRE(tag.hash == 23243);
        }
    } catch(std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        throw;
    }
    return 0;
}
