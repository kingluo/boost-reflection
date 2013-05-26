//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SAMPLE_CLASSES_HPP
#define SAMPLE_CLASSES_HPP

#include <iostream>
#include <string>

#include <boost/reflection/reflection.hpp>

#include "test_interface.hpp"

namespace test {

class AccountImpl : public AccountItf {
public:
    AccountImpl() : age(0) {}
    AccountImpl(std::string id, unsigned int age) : age(age), id(id) {}
    static void echo(AccountImpl* acct, std::string banner) {
        std::cout << banner + ": " << acct->getId() << std::endl;
    }
    std::string getId() {
        return id;
    }
    void setId(std::string id) {
        this->id = id;
    }
public:
    unsigned int age;
private:
    std::string id;
};

//AccountImpl declaration for used by reflection
//this declaration can be placed anywhere, even the header file
BOOST_REFL_CLASS (
    (test)(AccountImpl),
    //derived Inteface
    BOOST_REFL_IMPLEMENT_INTERFACE( (test)(AccountItf) )
    //constructors
    BOOST_REFL_CONSTRUCTOR()
    BOOST_REFL_CONSTRUCTOR(std::string id, unsigned int age)
    //static methods
    BOOST_REFL_STATIC_METHOD(echo, void (AccountImpl* acct, std::string banner))
    //fields
    BOOST_REFL_FIELD(age, unsigned int)
)

struct AnotherAccountImpl : public test::AccountItf {
    std::string id;
    AnotherAccountImpl(std::string id) : id(id) {}
    std::string getId() {
        return id;
    }
    void setId(std::string id) {
        this->id = id;
    }
};

BOOST_REFL_CLASS (
    (test)(AnotherAccountImpl),
    //constructors
    BOOST_REFL_CONSTRUCTOR(std::string id)
    //derived Inteface
    BOOST_REFL_IMPLEMENT_INTERFACE( (test)(AccountItf) )
)

} //namespace test

//add this dummy function to enable .lib generation on Windows
extern "C" {
BOOST_SYMBOL_EXPORT void dummy() {}
}

#endif
