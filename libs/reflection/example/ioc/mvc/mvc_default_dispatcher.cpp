//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <map>

#include <boost/reflection/reflection.hpp>
using namespace boost::reflection;
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "mvc_dispatcher_interface.hpp"

namespace test { namespace mvc {

using namespace ::mvc::annotation;

int indexofArg(std::string signature, std::string arg) {
    //TODO make a robust parse
    boost::trim(arg);
    std::vector<std::string> vec;
    boost::split( vec, signature, boost::is_any_of(","), boost::token_compress_on );
    int index = 0;
    BOOST_FOREACH(std::string str, vec) {
        boost::trim(str);
        if (boost::iends_with(str, arg)) {
            return index;
        }
        index++;
    }
    return -1;
}

class DefaultDispatcherServlet : public DispatcherServlet {
    std::string scan_namespace;
public:
    DefaultDispatcherServlet(std::string scan_namespace)
        : scan_namespace(scan_namespace) {}
    void handle(ServletRequest* req, ServletResponse* res) {
        std::cout << "--DefaultDispatcherServlet::handle--" << std::endl;
        std::cout << "Request path: " << req->getPath() << std::endl;
        std::list<Class> classes = Class::forNamespace(scan_namespace);
        BOOST_FOREACH(Class& c, classes) {
            if (!c.isAnnotationPresent<Controller>()) {
                continue;
            }
            std::list<Method> methods = c.getMethods();
            BOOST_FOREACH(Method& m, methods) {
                if (!m.isAnnotationPresent<ResquestMapping>()) {
                    continue;
                }
                ResquestMapping tag =
                    m.getAnnotation<ResquestMapping>();
                if (req->getPath() == tag.path) {
                    std::cout << "found Controller: class=" << c.getName() << std::endl;
                    const TypeInfoArray& argTypes = m.getParameterTypes();
                    ModelMap model;
                    std::vector<boost::any> args;
                    args.resize(argTypes.size());
                    //TODO make a robust argument filling
                    for(std::size_t i = 0; i < argTypes.size(); i++) {
                        if (BOOST_REFL_IS_TYPE_SAME(typeid(ModelMap*), argTypes[i])) {
                            args[i] = &model;
                        } else {
                            BOOST_FOREACH(ResquestParamMapping& pm, tag.paramMapping) {
                                int index = indexofArg(m.getSignature(), pm.arg);
                                if (index!= -1) {
                                    args[index] = req->getParam(pm.param);
                                }
                            }
                        }
                    }
                    boost::any o = c.newInstance();
                    boost::any view = m.invoke(o, args);
                    res->resloveView(boost::any_cast<std::string>(view), model);
                    c.deleteInstance(o);
                }
            }
        }
    }
};

BOOST_REFL_CLASS (
    (test)(mvc)(DefaultDispatcherServlet),
    BOOST_REFL_IMPLEMENT_INTERFACE( (DispatcherServlet) )
    BOOST_REFL_CONSTRUCTOR(std::string scan_namespace)
)

}}
