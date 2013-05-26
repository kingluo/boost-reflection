//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_REFLECTION_TEST_IOC_HPP
#define BOOST_REFLECTION_TEST_IOC_HPP

#include <map>
#include <list>
#include <string>

#include <boost/reflection/reflection.hpp>

# if defined(BOOST_REFLECTION_TEST_IOC_SOURCE)
#   define BOOST_REFLECTION_TEST_IOC_SYMBOL BOOST_SYMBOL_EXPORT
# else
#   define BOOST_REFLECTION_TEST_IOC_SYMBOL BOOST_SYMBOL_IMPORT
# endif

struct BeanProperty {
    std::string name;
    std::list<std::string> values;
    std::list<std::string> beans;
};

struct BeanDefinition {
    std::string id;
    std::string className;
    std::list<BeanProperty> ctorArgs;
    std::list<BeanProperty> props;
    boost::any singletonObj;
};

class ApplicationContext {
public:
    BOOST_REFLECTION_TEST_IOC_SYMBOL ApplicationContext(const std::list<std::string>& xmlFile);
    template<class T>
    T getBean(std::string beanName) {
        return boost::reflection::arg_cast<T>(getBean(beanName));
    }
private:
    BOOST_REFLECTION_TEST_IOC_SYMBOL boost::any getBean(std::string beanName);
    typedef std::map<std::string, BeanDefinition> map_t;
    map_t beans;
};

#endif
