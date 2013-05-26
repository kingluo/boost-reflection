//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/reflection/reflection.hpp>
using namespace boost::reflection;
#include "mvc_common.hpp"

namespace test { namespace mvc { namespace foo {

struct PetQueryController {
    std::string handleQuery(std::string petId, ModelMap* model) {
        std::string petDesc = std::string("Pet ") + petId + ", handled by foo Controller";
        model->insert(ModelMap::value_type("petDesc", petDesc));
        return "fooView";
    }
};

BOOST_REFL_CLASS_WITH_ANNOTATION (
    (test)(mvc)(foo)(PetQueryController),
    BOOST_REFL_SIMPLE_ANNOTATION( (mvc)(annotation)(Controller) )
    BOOST_REFL_ANNOTATION ( (mvc)(annotation)(SessionAttribute), (sid = 123456) ),
    BOOST_REFL_CONSTRUCTOR()
    BOOST_REFL_METHOD_WITH_ANNOTATION(
        handleQuery,
        std::string (std::string petId, ModelMap* model),
        BOOST_REFL_ANNOTATION (
            (mvc)(annotation)(ResquestMapping),
            (path = "/foo/petQuery")
            (paramMapping = make_list< ::mvc::annotation::ResquestParamMapping >(
                PARA_MAPPING((arg = "petId")(param = "id"))
            ))
        )
    )
)

} } }
