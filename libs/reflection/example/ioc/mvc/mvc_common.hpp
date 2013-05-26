//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MVC_COMMON_HPP
#define MVC_COMMON_HPP

#include <list>
#include <string>
#include <map>
#include <iostream>

#include <boost/reflection/reflection.hpp>

//mvc-related annotations
BOOST_REFL_DEF_SIMPLE_ANNOTATION(
    (mvc)(annotation)(Controller)
)

BOOST_REFL_DEF_ANNOTATION(
    (mvc)(annotation)(ResquestParamMapping),
    BOOST_REFL_DEF_ANNOTATION_ELEM(std::string, arg)
    BOOST_REFL_DEF_ANNOTATION_ELEM(std::string, param)
)

#define PARA_MAPPING(seq) \
    BOOST_REFL_ANNOTATION( \
        (mvc)(annotation)(ResquestParamMapping), seq \
    )

BOOST_REFL_DEF_ANNOTATION(
    (mvc)(annotation)(ResquestMapping),
    BOOST_REFL_DEF_ANNOTATION_ELEM(std::string, path)
    BOOST_REFL_DEF_ANNOTATION_ELEM_DEFAULT(
        std::list<ResquestParamMapping>,
        paramMapping,
        std::list<ResquestParamMapping>()
    )
)

BOOST_REFL_DEF_ANNOTATION(
    (mvc)(annotation)(SessionAttribute),
    BOOST_REFL_DEF_ANNOTATION_ELEM(int, sid)
)

//mvc dispatcher related definitions
class ServletRequest {
    std::string path;
    std::map<std::string, std::string> params;
public:
    ServletRequest(std::string path, std::map<std::string, std::string> params)
        : path(path), params(params) {}
    std::string getPath() {
        return path;
    }
    std::map<std::string, std::string> getParams() {
        return params;
    }
    std::string getParam(std::string key) {
        return params[key];
    }
};

typedef std::map<std::string, boost::any> ModelMap;

class ServletResponse {
public:
    void resloveView(std::string view, ModelMap model) {
        std::cout << "--ServletResponse::resloveView--" << std::endl;
        typedef ModelMap::iterator iterator_t;
        for(iterator_t it = model.begin(); it != model.end(); it++) {
            boost::any v = it->second;
            if (BOOST_REFL_IS_TYPE_SAME(typeid(std::string), v.type())) {
                std::cout << "MODEL STRING ITEM: key=" << it->first <<
                    ", value=" << boost::any_cast<std::string>(v) << std::endl;
            }
        }
        std::cout << "reslove view id=" << view << std::endl;
    }
};

#endif //MVC_COMMON_HPP
