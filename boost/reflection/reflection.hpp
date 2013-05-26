//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_REFLECTION_REFLECTION_HPP
#define BOOST_REFLECTION_REFLECTION_HPP

#if defined(_MSC_VER) && (_MSC_VER < 1400)
#  error "only vc2005 or higher versions supported"
#endif

#include <boost/reflection/class.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/push_front.hpp>
#include <boost/preprocessor/seq/pop_front.hpp>
#include <boost/preprocessor/seq/pop_back.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/reverse.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <boost/parameter.hpp>

#define BOOST_REFL_EMPTY_SEQ (0)(0)

// Annotation
#define BOOST_REFL_DEF_ANNOTATION_ELEM(type, name) ((type)(name)(0)(0))
#define BOOST_REFL_DEF_ANNOTATION_ELEM_DEFAULT(type, name, default) ((type)(name)(1)(default))

#define BOOST_REFL_DEF_PLAIN_ANNOTATION_FIELD_ASSIGN(z, n, seq) \
    v.BOOST_PP_SEQ_ELEM(1 , BOOST_PP_SEQ_ELEM(n, seq) ) = \
        BOOST_PP_CAT(_, BOOST_PP_SEQ_ELEM(1 , BOOST_PP_SEQ_ELEM(n, seq) ));

#define BOOST_REFL_DEF_PLAIN_ANNOTATION_KEYWORD(z, n, seq) \
    BOOST_PARAMETER_NAME( ( BOOST_PP_SEQ_ELEM(1 , BOOST_PP_SEQ_ELEM(n, seq) ), tag ) \
        BOOST_PP_CAT(_, BOOST_PP_SEQ_ELEM(1 , BOOST_PP_SEQ_ELEM(n, seq) )) )

#define BOOST_REFL_DEF_PLAIN_ANNOTATION_NAMESPACE_PART1(z, n, seq) \
    namespace BOOST_PP_SEQ_ELEM(n, seq) {

#define BOOST_REFL_DEF_PLAIN_ANNOTATION_NAMESPACE_PART2(z, n, seq) }

#define BOOST_REFL_DEF_PLAIN_ANNOTATION_CLASS_FIELD(z, n, seq) \
    BOOST_PP_SEQ_ELEM(0 , BOOST_PP_SEQ_ELEM(n, seq) ) \
    BOOST_PP_SEQ_ELEM(1 , BOOST_PP_SEQ_ELEM(n, seq) );

#define BOOST_REFL_DEF_ANNOTATION_OPTIONAL(n, seq) \
BOOST_PP_COMMA() BOOST_PP_SEQ_ELEM(3 , BOOST_PP_SEQ_ELEM(n, seq) )
#define BOOST_REFL_DEF_ANNOTATION_REQUIRED(n, seq)

#define BOOST_REFL_DEF_PLAIN_ANNOTATION_PARAMETER(z, n, seq) \
    ( BOOST_PP_IF(BOOST_PP_SEQ_ELEM(2 , BOOST_PP_SEQ_ELEM(n, seq) ), optional, required) \
        ( \
            BOOST_PP_CAT(_, BOOST_PP_SEQ_ELEM(1 , BOOST_PP_SEQ_ELEM(n, seq) )) BOOST_PP_COMMA() \
            ( BOOST_PP_SEQ_ELEM(0 , BOOST_PP_SEQ_ELEM(n, seq) ) ) \
            BOOST_PP_IF(BOOST_PP_SEQ_ELEM(2 , BOOST_PP_SEQ_ELEM(n, seq) ), \
                BOOST_REFL_DEF_ANNOTATION_OPTIONAL, \
                BOOST_REFL_DEF_ANNOTATION_REQUIRED \
            )(n, seq) \
        ) \
    )

#define BOOST_REFL_ANNOTATION_KEYWORD_NS(cls) \
    BOOST_PP_CAT(BOOST_REFL_CLASS_NAME_CONSTRUCT(cls), _keywords)

#define BOOST_REFL_DEF_ANNOTATION_LL(cls, seq) \
    namespace BOOST_PP_CAT(cls, _keywords) { \
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), BOOST_REFL_DEF_PLAIN_ANNOTATION_KEYWORD, seq) \
    } \
    struct cls \
    { \
        BOOST_PARAMETER_MEMBER_FUNCTION( \
            (cls), static BOOST_PP_CAT(new_, cls), \
            BOOST_PP_CAT(cls, _keywords)::tag, \
            BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), BOOST_REFL_DEF_PLAIN_ANNOTATION_PARAMETER, seq) \
        ) \
        { \
            cls v; \
            BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), BOOST_REFL_DEF_PLAIN_ANNOTATION_FIELD_ASSIGN, seq) \
            return v; \
        } \
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), BOOST_REFL_DEF_PLAIN_ANNOTATION_CLASS_FIELD, seq) \
    };

#define BOOST_REFL_DEF_ANNOTATION(cls, seq) \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_POP_BACK(cls)), \
        BOOST_REFL_DEF_PLAIN_ANNOTATION_NAMESPACE_PART1, BOOST_PP_SEQ_POP_BACK(cls)) \
            BOOST_REFL_DEF_ANNOTATION_LL(BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls)), seq) \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_POP_BACK(cls)), \
        BOOST_REFL_DEF_PLAIN_ANNOTATION_NAMESPACE_PART2, ~)

#define BOOST_REFL_DEF_SIMPLE_ANNOTATION(cls) \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_POP_BACK(cls)), \
        BOOST_REFL_DEF_PLAIN_ANNOTATION_NAMESPACE_PART1, BOOST_PP_SEQ_POP_BACK(cls)) \
    struct BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls)) { \
        static BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls)) \
            BOOST_PP_CAT(new_, BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls))) () { \
            BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls)) v; \
            return v; \
        } \
    }; \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_POP_BACK(cls)), \
        BOOST_REFL_DEF_PLAIN_ANNOTATION_NAMESPACE_PART2, BOOST_PP_SEQ_POP_BACK(cls))

#define BOOST_REFL_ANNOTATION_EXPAND_PARAMETERS(z, n, seq) \
    BOOST_PP_SEQ_ELEM(0, seq)::BOOST_PP_SEQ_ELEM(BOOST_PP_INC(n), seq)

#define BOOST_REFL_ANNOTATION(cls, seq) \
    ( \
        BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)::BOOST_PP_CAT(new_, BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls))) \
        ( BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(seq), BOOST_REFL_ANNOTATION_EXPAND_PARAMETERS, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_ANNOTATION_KEYWORD_NS(cls))) ) \
    )

#define BOOST_REFL_SIMPLE_ANNOTATION(cls) \
    ( \
        BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)::BOOST_PP_CAT(new_, BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_REVERSE(cls))) () \
    )

#define BOOST_REFL_ANNOTATION_MAKE_LIST_ARG(z, n, seq) boost::any(BOOST_PP_SEQ_ELEM(n, seq))
#define BOOST_REFL_ANNOTATION_MAKE_LIST_OP(seq) \
    BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(seq), BOOST_REFL_ANNOTATION_MAKE_LIST_ARG, seq)
#define BOOST_REFL_ANNOTATION_MAKE_LIST_OP_NUL(seq)
#define BOOST_REFL_ANNOTATION_MAKE_LIST(seq) \
    boost::reflection::make_list<boost::any>( \
        BOOST_PP_IF(BOOST_PP_SEQ_ELEM(0, seq), \
            BOOST_REFL_ANNOTATION_MAKE_LIST_OP, \
            BOOST_REFL_ANNOTATION_MAKE_LIST_OP_NUL \
        )(BOOST_PP_SEQ_POP_FRONT(seq)) \
    )

//-----#----
#define BOOST_REFL_INTERFACE_METHOD_PARAM_DECLARE(z, n, seq) \
    BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(n, seq)) \
    BOOST_PP_SEQ_ELEM(1, BOOST_PP_SEQ_ELEM(n, seq))

#define BOOST_REFL_INTERFACE_METHOD_PARAM_ASSIGN(z, n, seq) \
    args.push_back(boost::reflection::InvocationHandlerParamWrap<BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(n, seq))>()(BOOST_PP_SEQ_ELEM(1, BOOST_PP_SEQ_ELEM(n, seq))));

#define BOOST_REFL_INTERFACE_METHOD_EXPAND(name, returnType, paramSeq) \
returnType name ( BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(paramSeq), BOOST_REFL_INTERFACE_METHOD_PARAM_DECLARE, paramSeq) ) { \
    std::vector<boost::any> args; \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(paramSeq), BOOST_REFL_INTERFACE_METHOD_PARAM_ASSIGN, paramSeq) \
    return boost::reflection::InvocationHandlerInvoke<returnType>()(h, this, BOOST_STRINGIZE(name), args); \
}

#define BOOST_REFL_INTERFACE_METHOD_EXPAND_NO_ARGS(name, returnType, paramSeq) \
returnType name () { \
    std::vector<boost::any> args; \
    return boost::reflection::InvocationHandlerInvoke<returnType>()(h, this, BOOST_STRINGIZE(name), args); \
}

#define BOOST_REFL_INTERFACE_METHOD_EXPAND_WRAPPER(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM(2, BOOST_PP_SEQ_ELEM(n, seq)) ), \
        BOOST_REFL_INTERFACE_METHOD_EXPAND, \
        BOOST_REFL_INTERFACE_METHOD_EXPAND_NO_ARGS \
    )( \
        BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(n, seq)), \
        BOOST_PP_SEQ_ELEM(1, BOOST_PP_SEQ_ELEM(n, seq)), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM(2, BOOST_PP_SEQ_ELEM(n, seq)) ) \
    )

#define BOOST_REFL_ENUM_NUL(a,b,c)
#define BOOST_REFL_CONVERT_METHOD_DECALRE(z, n, seq) \
    BOOST_REFL_METHOD( \
        BOOST_PP_SEQ_ELEM(0, BOOST_PP_SEQ_ELEM(n, seq)), \
        BOOST_PP_SEQ_ELEM(1, BOOST_PP_SEQ_ELEM(n, seq)) ( \
            BOOST_PP_IF( \
                BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM(2, BOOST_PP_SEQ_ELEM(n, seq)) ), \
                BOOST_PP_ENUM, \
                BOOST_REFL_ENUM_NUL \
            )( \
                BOOST_PP_SEQ_SIZE( BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM(2, BOOST_PP_SEQ_ELEM(n, seq)) ) ), \
                BOOST_REFL_INTERFACE_METHOD_PARAM_DECLARE, \
                BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM(2, BOOST_PP_SEQ_ELEM(n, seq)) ) \
            ) \
        ) \
    )

// must be declared in global namespace
#define BOOST_REFL_INTERFACE_METHOD(name, returnType, paramSeq) \
    ((name)(returnType)(paramSeq))

#define BOOST_REFL_INTERFACE_METHOD_PARAMS(seq) \
    (1)seq
#define BOOST_REFL_INTERFACE_METHOD_PARAM(type, arg) \
    ((type)(arg))
#define BOOST_REFL_INTERFACE_METHOD_NO_PARAMS \
    (0)(0)

#define BOOST_REFL_INTERFACE_PROXY_NAME(seq) \
    BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(seq), _interface)

#define BOOST_REFL_INTERFACE(nameSeq, methodSeq) \
    BOOST_REFL_CLASS( \
        nameSeq, \
        BOOST_PP_REPEAT( BOOST_PP_SEQ_SIZE(methodSeq), BOOST_REFL_CONVERT_METHOD_DECALRE, methodSeq ) \
    ) \
    class BOOST_REFL_INTERFACE_PROXY_NAME(nameSeq) : public BOOST_REFL_CLASS_NAME_CONSTRUCT(nameSeq) { \
    protected: \
        boost::reflection::InvocationHandler* h; \
        BOOST_REFL_INTERFACE_PROXY_NAME(nameSeq) (boost::reflection::InvocationHandler* h) : h(h) { \
        } \
    public: \
        BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(methodSeq), BOOST_REFL_INTERFACE_METHOD_EXPAND_WRAPPER, methodSeq) \
    };

//-----#----

// tagging
#define BOOST_REFL_METHOD_TAG 1
#define BOOST_REFL_CONSTRUCTOR_TAG 2
#define BOOST_REFL_FIELD_TAG 3
#define BOOST_REFL_INTERFACE_TAG 4

#define BOOST_REFL_CONSTRUCTOR(...) \
    ((BOOST_REFL_CONSTRUCTOR_TAG)((__VA_ARGS__))(BOOST_REFL_EMPTY_SEQ))
#define BOOST_REFL_CONSTRUCTOR_WITH_ANNOTATION(signature, annotations) \
    ((BOOST_REFL_CONSTRUCTOR_TAG)(signature)((1)annotations))

#define BOOST_REFL_METHOD(name, signature) \
    ((BOOST_REFL_METHOD_TAG)(name)(signature)(false)(true)(BOOST_REFL_EMPTY_SEQ))
#define BOOST_REFL_METHOD_WITH_ANNOTATION(name, signature, annotations) \
    ((BOOST_REFL_METHOD_TAG)(name)(signature)(false)(true)((1)annotations))

#define BOOST_REFL_CONST_METHOD(name, signature) \
    ((BOOST_REFL_METHOD_TAG)(name)(signature)(true)(true)(BOOST_REFL_EMPTY_SEQ))
#define BOOST_REFL_CONST_METHOD_WITH_ANNOTATION(name, signature, annotations) \
    ((BOOST_REFL_METHOD_TAG)(name)(signature)(true)(true)((1)annotations))

#define BOOST_REFL_FIELD(name, type) ((BOOST_REFL_FIELD_TAG)(name)(type)(BOOST_REFL_EMPTY_SEQ))
#define BOOST_REFL_FIELD_WITH_ANNOTATION(name, type, annotations) ((BOOST_REFL_FIELD_TAG)(name)(type)((1)annotations))

#define BOOST_REFL_STATIC_METHOD(name, signature) \
    ((BOOST_REFL_METHOD_TAG)(name)(signature)(false)(false)(BOOST_REFL_EMPTY_SEQ))
#define BOOST_REFL_STATIC_METHOD_WITH_ANNOTATION(name, signature, annotations) \
    ((BOOST_REFL_METHOD_TAG)(name)(signature)(false)(false)((1)annotations))

#define BOOST_REFL_IMPLEMENT_INTERFACE(nameSeq) ((BOOST_REFL_INTERFACE_TAG)(nameSeq)(0))

// constuctor registration
#define BOOST_REFL_REGISTER_CONSTRUCTOR(cls, seq) \
    constructors.push_back( \
        boost::reflection::Constructor( \
            new boost::reflection::ConstructorRefl<cls, cls* BOOST_PP_SEQ_ELEM(0, seq)>( \
                BOOST_PP_STRINGIZE(cls* BOOST_PP_SEQ_ELEM(0, seq)) \
            ) \
            , BOOST_REFL_ANNOTATION_MAKE_LIST(BOOST_PP_SEQ_ELEM(1, seq)) \
        ) \
    ); \
    if (!destructor) \
    { \
        destructor = new boost::reflection::DestructorRefl<cls>; \
    }
#define BOOST_REFL_REGISTER_CONSTRUCTOR_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_CONSTRUCTOR(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_CONSTRUCTOR_TAG \
        ), \
        BOOST_REFL_REGISTER_CONSTRUCTOR, \
        BOOST_REFL_REGISTER_CONSTRUCTOR_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// method registration
#define BOOST_REFL_REGISTER_METHOD(cls, seq) \
    methods.push_back( \
        boost::reflection::Method( \
            new boost::reflection::MethodRefl<cls, BOOST_PP_SEQ_ELEM(1, seq), \
            BOOST_PP_SEQ_ELEM(2, seq), BOOST_PP_SEQ_ELEM(3, seq)>( \
                BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(0, seq)), \
                BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(1, seq)), \
                &cls::BOOST_PP_SEQ_ELEM(0, seq) \
            ) \
            , BOOST_REFL_ANNOTATION_MAKE_LIST(BOOST_PP_SEQ_ELEM(4, seq)) \
        ) \
    );
#define BOOST_REFL_REGISTER_METHOD_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_METHOD(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_METHOD_TAG \
        ), \
        BOOST_REFL_REGISTER_METHOD, \
        BOOST_REFL_REGISTER_METHOD_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// field registration
#define BOOST_REFL_REGISTER_FIELD(cls, seq) \
    fields.push_back( \
        boost::reflection::Field( \
            new boost::reflection::FieldRefl<cls, BOOST_PP_SEQ_ELEM(1, seq)>( \
                BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(0, seq)), \
                &cls::BOOST_PP_SEQ_ELEM(0, seq) \
            ) \
            , BOOST_REFL_ANNOTATION_MAKE_LIST(BOOST_PP_SEQ_ELEM(2, seq)) \
        ) \
    );
#define BOOST_REFL_REGISTER_FIELD_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_FIELD(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_FIELD_TAG \
        ), \
        BOOST_REFL_REGISTER_FIELD, \
        BOOST_REFL_REGISTER_FIELD_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// interface registration
#define BOOST_REFL_REGISTER_INTERFACE(cls, seq) \
    interfaces.push_back( \
        boost::reflection::createInterface<cls, \
            BOOST_REFL_CLASS_NAME_CONSTRUCT( BOOST_PP_SEQ_ELEM(0, seq) )>( \
            BOOST_PP_STRINGIZE( BOOST_REFL_CLASS_NAME_CONSTRUCT_NO_GLOBAL_SCOPE( BOOST_PP_SEQ_ELEM(0, seq) ) ), \
            typeid( BOOST_REFL_CLASS_NAME_CONSTRUCT( BOOST_PP_SEQ_ELEM(0, seq) ) ).name() \
        ) \
    ); \
    dtors->push_back(new boost::reflection::DestructorRefl< \
        BOOST_REFL_CLASS_NAME_CONSTRUCT(BOOST_PP_SEQ_ELEM(0, seq))>);
#define BOOST_REFL_REGISTER_INTERFACE_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_INTERFACE_TAG \
        ), \
        BOOST_REFL_REGISTER_INTERFACE, \
        BOOST_REFL_REGISTER_INTERFACE_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// interface methods clone
// NOTE, the duplicated methods will be kept the first one within Class initialization
#define BOOST_REFL_REGISTER_INTERFACE_METHOD_CLONE(cls, seq) \
    { \
        boost::reflection::Class t = boost::reflection::Class::forName( BOOST_PP_STRINGIZE( BOOST_REFL_CLASS_NAME_CONSTRUCT2( BOOST_PP_SEQ_ELEM(0, seq) ) ) ); \
        std::list<boost::reflection::Method> v = t.getMethods(); \
        std::copy(v.begin(), v.end(), std::back_inserter(methods)); \
    }
#define BOOST_REFL_REGISTER_INTERFACE_METHOD_CLONE_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE_METHOD_CLONE(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_INTERFACE_TAG \
        ), \
        BOOST_REFL_REGISTER_INTERFACE_METHOD_CLONE, \
        BOOST_REFL_REGISTER_INTERFACE_METHOD_CLONE_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// register object cast for interfaces
#define BOOST_REFL_REGISTER_INTERFACE_OBJECTCAST(cls, seq) \
    objCastWrappers.push_back( \
        boost::reflection::ObjectCastWrapper( \
            typeid(BOOST_REFL_CLASS_NAME_CONSTRUCT(BOOST_PP_SEQ_ELEM(0, seq))*).name(), \
            new boost::reflection::ObjectCast<cls, \
                BOOST_REFL_CLASS_NAME_CONSTRUCT(BOOST_PP_SEQ_ELEM(0, seq))> \
        ) \
    );
#define BOOST_REFL_REGISTER_INTERFACE_OBJECTCAST_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE_OBJECTCAST(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_INTERFACE_TAG \
        ), \
        BOOST_REFL_REGISTER_INTERFACE_OBJECTCAST, \
        BOOST_REFL_REGISTER_INTERFACE_OBJECTCAST_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// interface proxy
#define BOOST_REFL_REGISTER_INTERFACE_PROXY(cls, seq) \
    BOOST_REFL_INTERFACE_PROXY_NAME( BOOST_PP_SEQ_ELEM(0, seq) ) ,
#define BOOST_REFL_REGISTER_INTERFACE_PROXY_NUL(cls, seq)

#define BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE_PROXY(z, n, seq) \
    BOOST_PP_IF( \
        BOOST_PP_EQUAL( \
            BOOST_PP_SEQ_ELEM( 0, BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ), \
            BOOST_REFL_INTERFACE_TAG \
        ), \
        BOOST_REFL_REGISTER_INTERFACE_PROXY, \
        BOOST_REFL_REGISTER_INTERFACE_PROXY_NUL \
    )( \
        BOOST_PP_SEQ_ELEM( 0, seq ), \
        BOOST_PP_SEQ_POP_FRONT( BOOST_PP_SEQ_ELEM( BOOST_PP_INC(n), seq ) ) \
    )

// class naming
#define BOOST_REFL_CLASS_REGISTER_PREFIX __boost_reflection_class_register_
#define BOOST_REFL_CLASS_REGISTER_CONCAT(s, state, elem) BOOST_PP_CAT(BOOST_PP_CAT(state, _), elem)

#define BOOST_REFL_CLASS_REGISTER_CONSTRUCT_OP1(cls) \
    BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_PREFIX, BOOST_PP_SEQ_HEAD(cls))
#define BOOST_REFL_CLASS_REGISTER_CONSTRUCT_OP2(cls) \
    BOOST_PP_SEQ_FOLD_LEFT( \
        BOOST_REFL_CLASS_REGISTER_CONCAT, \
        BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_PREFIX, BOOST_PP_SEQ_HEAD(cls)), \
        BOOST_PP_SEQ_TAIL(cls) \
    )
#define BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls) \
    BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(cls), 1), \
        BOOST_REFL_CLASS_REGISTER_CONSTRUCT_OP1, \
        BOOST_REFL_CLASS_REGISTER_CONSTRUCT_OP2 \
    )(cls)
#define BOOST_REFL_CLASS_REGISTER_VARIABLE_CONSTRUCT(cls) \
    BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _instance)

#define BOOST_REFL_CLASS_NAME_CONSTRUCT_OP1(cls) \
    BOOST_PP_SEQ_HEAD(cls)
#define BOOST_REFL_CLASS_NAME_CONSTRUCT_OP2(cls) \
    BOOST_PP_SEQ_FOLD_LEFT(BOOST_REFL_CLASS_NAME_CONCAT, BOOST_PP_SEQ_HEAD(cls), BOOST_PP_SEQ_TAIL(cls))
#define BOOST_REFL_CLASS_NAME_CONCAT(s, state, elem) state::elem
#define BOOST_REFL_CLASS_NAME_CONSTRUCT_NO_GLOBAL_SCOPE(cls) \
    BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(cls), 1), \
        BOOST_REFL_CLASS_NAME_CONSTRUCT_OP1, \
        BOOST_REFL_CLASS_NAME_CONSTRUCT_OP2 \
    )(cls)
#define BOOST_REFL_CLASS_NAME_CONSTRUCT(cls) \
    ::BOOST_REFL_CLASS_NAME_CONSTRUCT_NO_GLOBAL_SCOPE(cls)

// JAVA style class naming
#define BOOST_REFL_CLASS_NAME_CONCAT2(s, state, elem) state.elem
#define BOOST_REFL_CLASS_NAME_CONSTRUCT2_OP1(cls) BOOST_PP_SEQ_HEAD(cls)
#define BOOST_REFL_CLASS_NAME_CONSTRUCT2_OP2(cls) \
    BOOST_PP_SEQ_FOLD_LEFT(BOOST_REFL_CLASS_NAME_CONCAT2, BOOST_PP_SEQ_HEAD(cls), BOOST_PP_SEQ_TAIL(cls))
#define BOOST_REFL_CLASS_NAME_CONSTRUCT2(cls) \
    BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(cls), 1), \
        BOOST_REFL_CLASS_NAME_CONSTRUCT2_OP1, \
        BOOST_REFL_CLASS_NAME_CONSTRUCT2_OP2 \
    )(cls)

//////

#define BOOST_REFL_CONTAINER_FOR_TYPE(typeName, type) \
{ \
    std::string cname = boost::reflection::Container::normalizeName( \
        BOOST_PP_STRINGIZE(list<typeName>)); \
    if (!boost::reflection::Class::isContainerRegistered(cname)) { \
        boost::reflection::Class::registerContainer(boost::reflection::Container( \
            cname, \
            typeid(std::list<type>*).name(), \
            new boost::reflection::ContainerImpl<std::list<type> > \
        )); \
        container_names.push_back(cname); \
    } \
} \
{ \
    std::string cname = boost::reflection::Container::normalizeName( \
        BOOST_PP_STRINGIZE(map<typeName>)); \
    if (!boost::reflection::Class::isContainerRegistered(cname)) { \
        boost::reflection::Class::registerContainer(boost::reflection::Container( \
            cname, \
            typeid(std::map<std::string, type>*).name(), \
            new boost::reflection::ContainerImplWithKey<std::map<std::string, type> > \
        )); \
        container_names.push_back(cname); \
    } \
}

#define BOOST_REFL_POINTER_CONTAINER(cls) \
struct BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _ptr_container) { \
    BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _ptr_container) () { \
        BOOST_REFL_CONTAINER_FOR_TYPE( \
            BOOST_REFL_CLASS_NAME_CONSTRUCT2(cls)*, \
            BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)* \
        ) \
    } \
    ~BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _ptr_container) () { \
        BOOST_FOREACH(const std::string& cname, container_names) { \
            boost::reflection::Class::unRegisterContainer(cname); \
        } \
    } \
    std::list<std::string> container_names; \
}; \
static BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _ptr_container) \
BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_VARIABLE_CONSTRUCT(cls), _ptr_container);

#define BOOST_REFL_CONTAINER(cls) \
struct BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _container) { \
    BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _container) () { \
        BOOST_REFL_CONTAINER_FOR_TYPE( \
            BOOST_REFL_CLASS_NAME_CONSTRUCT2(cls), \
            BOOST_REFL_CLASS_NAME_CONSTRUCT_NO_GLOBAL_SCOPE(cls) \
        ) \
    } \
    ~BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _container) () { \
        BOOST_FOREACH(const std::string& cname, container_names) { \
            boost::reflection::Class::unRegisterContainer(cname); \
        } \
    } \
    std::list<std::string> container_names; \
}; \
static BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls), _container) \
BOOST_PP_CAT(BOOST_REFL_CLASS_REGISTER_VARIABLE_CONSTRUCT(cls), _container);

//////

#define BOOST_REFL_CLASS_WITH_ANNOTATION(cls, annotations, seq) \
    BOOST_REFL_CLASS_LL(cls, seq, (1)annotations)

#define BOOST_REFL_CLASS(cls, seq) \
    BOOST_REFL_CLASS_LL(cls, seq, BOOST_REFL_EMPTY_SEQ)

#define BOOST_REFL_CLASS_LL(cls, seq, annotations) \
struct BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls) { \
    BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls)() { \
        class_name = BOOST_PP_STRINGIZE(BOOST_REFL_CLASS_NAME_CONSTRUCT2(cls)); \
        if (boost::reflection::Class::isClassRegistered(class_name)) { \
            need_to_finalize = false; \
            return; \
        } else { \
            need_to_finalize = true; \
        } \
        std::list<boost::reflection::Field> fields; \
        std::list<boost::reflection::Method> methods; \
        std::list<boost::reflection::Constructor> constructors; \
        boost::reflection::DestructorReflItf* destructor = 0; \
        std::list<boost::reflection::DestructorReflItf*>* dtors \
            = new std::list<boost::reflection::DestructorReflItf*>; \
        std::list<boost::reflection::Interface> interfaces; \
        std::list<boost::reflection::ObjectCastWrapper> objCastWrappers; \
        objCastWrappers.push_back( \
            boost::reflection::ObjectCastWrapper( \
                typeid(BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)*).name(), \
                new boost::reflection::ObjectCast<BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)> \
            ) \
        ); \
        BOOST_PP_REPEAT( \
            BOOST_PP_SEQ_SIZE(seq), \
            BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_METHOD, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
        ) \
        BOOST_PP_REPEAT( \
            BOOST_PP_SEQ_SIZE(seq), \
            BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_CONSTRUCTOR, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
        ) \
        BOOST_PP_REPEAT( \
            BOOST_PP_SEQ_SIZE(seq), \
            BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_FIELD, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
        ) \
        BOOST_PP_REPEAT( \
            BOOST_PP_SEQ_SIZE(seq), \
            BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
        ) \
        BOOST_PP_REPEAT( \
            BOOST_PP_SEQ_SIZE(seq), \
            BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE_METHOD_CLONE, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
        ) \
        BOOST_PP_REPEAT( \
            BOOST_PP_SEQ_SIZE(seq), \
            BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE_OBJECTCAST, \
            BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
        ) \
        if (destructor) { \
            dtors->push_front(destructor); \
        } \
        struct ThisProxyCreator : public boost::reflection::ProxyCreator { \
            virtual boost::reflection::ProxyBase* createProxy(boost::reflection::InvocationHandler* h) { \
                typedef boost::mpl::vector< \
                    BOOST_PP_REPEAT( \
                        BOOST_PP_SEQ_SIZE(seq), \
                        BOOST_REFL_CLASS_REGISTER_SEQ_ITER_FOR_INTERFACE_PROXY, \
                        BOOST_PP_SEQ_PUSH_FRONT(seq, BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)) \
                    ) int \
                > v_; \
                typedef boost::mpl::pop_back<v_>::type v; \
                return new boost::reflection::Proxy<v>(h); \
            } \
        }; \
        boost::reflection::Class::registerClass( \
            class_name, \
            typeid(BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)*).name(), \
            objCastWrappers, \
            typeid(BOOST_REFL_CLASS_NAME_CONSTRUCT(cls)), \
            constructors, dtors, \
            methods, fields, interfaces \
            , BOOST_REFL_ANNOTATION_MAKE_LIST(annotations) \
            , new ThisProxyCreator \
        ); \
    } \
    ~BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls)() { \
        if (need_to_finalize) { \
            boost::reflection::Class::unRegisterClass(class_name); \
        } \
    } \
    std::string class_name; \
    bool need_to_finalize; \
}; \
static BOOST_REFL_CLASS_REGISTER_CONSTRUCT(cls) BOOST_REFL_CLASS_REGISTER_VARIABLE_CONSTRUCT(cls); \
BOOST_REFL_POINTER_CONTAINER(cls)

#endif //BOOST_REFLECTION_REFLECTION_HPP
