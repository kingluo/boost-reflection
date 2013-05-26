//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
namespace p = boost::property_tree;

#include <boost/reflection/reflection.hpp>
using namespace boost::reflection;

#include <boost/foreach.hpp>

#define BOOST_REFLECTION_TEST_IOC_SOURCE
#include "ioc.hpp"

BOOST_REFLECTION_TEST_IOC_SYMBOL boost::any ApplicationContext::getBean(std::string beanName)
{
    boost::any b;
    map_t::iterator it = beans.find(beanName);
    if (it != beans.end()) {
        BeanDefinition& def = it->second;
        std::cout << "LIBIOC: USE BEAN DEF: " << def.id << std::endl;
        if (!def.singletonObj.empty()) {
            b = def.singletonObj;
        } else {
            Class t = Class::forName(def.className);
            std::vector<boost::any> args;
            BOOST_FOREACH(BeanProperty& prop, def.ctorArgs) {
                if (!prop.values.empty()) {
                    if (prop.values.size() == 1) {
                        args.push_back(prop.values.front());
                    } else {
                        args.push_back(prop.values);
                    }
                }
            }
            b = t.newInstance(args);
            BOOST_FOREACH(BeanProperty& prop, def.props) {
                if (!prop.values.empty()) {
                    if (prop.values.size() == 1) {
                        t.set(b, prop.name, prop.values.front());
                    } else {
                        boost::any p = t.getPointer(b, prop.name);
                        Container c = Class::forContainerObject(p);
                        BOOST_FOREACH(std::string& v, prop.values) {
                            c.add(p, v);
                        }
                    }
                } else if (!prop.beans.empty()) {
                    boost::any otherBean;
                    if (prop.beans.size() == 1) {
                        otherBean = getBean(prop.beans.front());
                        t.set(b, prop.name, otherBean);
                    } else {
                        boost::any p = t.getPointer(b, prop.name);
                        std::cout << "LIBIOC: #BEAN: " << p.type().name() << std::endl;
                        Container c = Class::forContainerObject(p);
                        BOOST_FOREACH(std::string& v, prop.beans) {
                            otherBean = getBean(v);
                            std::cout << "LIBIOC: *BEAN: " << otherBean.type().name() << std::endl;
                            c.add(p, otherBean);
                        }
                    }
                }
            }
            def.singletonObj = b;
        }
    }
    std::cout << "LIBIOC: BEAN: " << b.type().name() << std::endl;
    return b;
}

BOOST_REFLECTION_TEST_IOC_SYMBOL ApplicationContext::ApplicationContext(const std::list<std::string>& xmls)
{
    BOOST_FOREACH(const std::string &xml, xmls) {
        using namespace p;
        ptree pt;
        read_xml(xml, pt, xml_parser::no_comments | xml_parser::trim_whitespace);
        BOOST_FOREACH(ptree::value_type &v, pt.get_child("beans")) {
            ptree& beanPt = v.second;
            BeanDefinition bean;
            bean.id = beanPt.get<std::string>("<xmlattr>.id");
            bean.className = beanPt.get<std::string>("<xmlattr>.class");
            BOOST_FOREACH(ptree::value_type &v2, beanPt) {
                std::string tagName = v2.first;
                if (tagName != "constructor-arg" && tagName != "property") {
                    continue;
                }
                ptree& subPt = v2.second;
                BeanProperty prop;
                prop.name = subPt.get<std::string>("<xmlattr>.name");
                boost::optional<std::string> value = subPt.get_optional<std::string>("<xmlattr>.value");
                boost::optional<std::string> ref = subPt.get_optional<std::string>("<xmlattr>.ref");
                if (!value && !ref) {
                    BOOST_FOREACH(ptree::value_type &v, subPt.get_child("list")) {
                        if (v.first == "value") {
                            prop.values.push_back(v.second.data());
                        } else if (v.first == "ref") {
                            prop.beans.push_back(v.second.get<std::string>("<xmlattr>.bean"));
                        }
                    }
                } else if (value) {
                    prop.values.push_back(*value);
                } else if (ref) {
                    prop.beans.push_back(*ref);
                }

                if (tagName == "constructor-arg") {
                    bean.ctorArgs.push_back(prop);
                } else if (tagName == "property") {
                    bean.props.push_back(prop);
                }
            }
            std::cout << "LIBIOC: BEAN DEF: " << bean.id << std::endl;
            beans.insert(map_t::value_type(bean.id, bean));
        }
    }
}
