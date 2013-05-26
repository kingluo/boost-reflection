//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TEST_INTERFACE_HPP
#define TEST_INTERFACE_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/reflection/reflection.hpp>

namespace test {

class AccountItf {
public:
    virtual std::string getId() = 0;
    virtual void setId(std::string id) = 0;
    virtual ~AccountItf() {}
};

BOOST_REFL_INTERFACE(
    (test)(AccountItf),
    BOOST_REFL_INTERFACE_METHOD(getId, std::string,
        BOOST_REFL_INTERFACE_METHOD_NO_PARAMS
    )
    BOOST_REFL_INTERFACE_METHOD(setId, void,
        BOOST_REFL_INTERFACE_METHOD_PARAMS(
            BOOST_REFL_INTERFACE_METHOD_PARAM(std::string, id)
        )
    )
)

class AccountEchoService {
    boost::shared_ptr<test::AccountItf> impl;
public:
    void setImpl(test::AccountItf* impl, boost::reflection::Deleter<test::AccountItf> deleter) {
        this->impl.reset(impl, deleter);
    }
    void setImpl(test::AccountItf* impl) {
        this->impl.reset(impl);
    }
    test::AccountItf* getImpl() {
        return impl.get();
    }
    void echo(std::string banner) {
        std::cout << banner << ": " << impl->getId() << std::endl;
    }
};

BOOST_REFL_CLASS (
    (test)(AccountEchoService),
    //constructors
    BOOST_REFL_CONSTRUCTOR()
    //methods
    BOOST_REFL_METHOD(echo, void(std::string banner))
    BOOST_REFL_METHOD(setImpl, void(test::AccountItf* impl))
    BOOST_REFL_METHOD(setImpl, void(test::AccountItf* impl, boost::reflection::Deleter<test::AccountItf> deleter))
    BOOST_REFL_METHOD(getImpl, test::AccountItf*())
)

}

#endif //TEST_INTERFACE_HPP
