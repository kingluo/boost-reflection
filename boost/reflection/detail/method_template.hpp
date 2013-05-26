//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_REFL_NUM_OF_ARGS BOOST_PP_ITERATION()

#define BOOST_REFL_ARGTYPE_NAME(z, n, text) \
        BOOST_PP_CAT(BOOST_PP_CAT(function_traits<F>::arg, BOOST_PP_INC(n)), _type)

#define BOOST_REFL_ARGTYPE_TYPEDEF_NAME(z, n, text) \
        BOOST_PP_CAT(BOOST_PP_CAT(F_arg, BOOST_PP_INC(n)), _type)

#define BOOST_REFL_ARGTYPE_REAL_TYPEDEF_NAME(n) \
        BOOST_PP_CAT(BOOST_PP_CAT(F_arg, BOOST_PP_INC(n)), _real_type)

#define BOOST_REFL_ARGTYPE_REAL(n) \
        mpl::if_< \
            typename is_reference<BOOST_REFL_ARGTYPE_TYPEDEF_NAME(~, n, ~)>::type, \
            reference_wrapper< typename remove_reference<BOOST_REFL_ARGTYPE_TYPEDEF_NAME(~, n, ~)>::type >, \
            BOOST_REFL_ARGTYPE_TYPEDEF_NAME(~, n, ~) \
        >::type

#define BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE(z, n, text) \
        typedef typename BOOST_REFL_ARGTYPE_REAL(n) BOOST_REFL_ARGTYPE_REAL_TYPEDEF_NAME(n);

#define BOOST_REFL_CAST_ARGTYPE(z, n, text) \
        arg_cast<BOOST_REFL_ARGTYPE_REAL_TYPEDEF_NAME(n)> (const_cast<boost::any&>(args[n]))

#define BOOST_REFL_CAST_ARGTYPE_FOR_CHECK(z, n, text) BOOST_REFL_CAST_ARGTYPE(z,n,text);

#define BOOST_REFL_CONSTRUCT_ARGTYPE_ARRAY(z, n, array) \
        array.push_back(cref(typeid(BOOST_REFL_ARGTYPE_REAL_TYPEDEF_NAME(n))));

#define BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE(z, n, text) \
        typedef typename BOOST_REFL_ARGTYPE_NAME(~, n, ~) BOOST_REFL_ARGTYPE_TYPEDEF_NAME(~, n, ~);

#define BOOST_REFL_METHODREFL_BASECLASS BOOST_PP_CAT(MethodReflBase_, BOOST_REFL_NUM_OF_ARGS)

template<class C, class F, bool isConstMemFunc, bool isMemFunc>
class BOOST_REFL_METHODREFL_BASECLASS : public MethodReflItf {
protected:
    typedef typename function_traits<F>::result_type F_result_type;
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef F_result_type (*method_t0) (BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_NAME, ~));
    typedef F_result_type (C::*method_t1) (BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_NAME, ~));
    typedef F_result_type (C::*method_t2) (BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_NAME, ~)) const;
    typedef typename boost::mpl::if_< boost::integral_constant<bool, isMemFunc>,
        typename boost::mpl::if_< boost::integral_constant<bool, isConstMemFunc>, method_t2, method_t1 >::type,
        method_t0 >::type method_t;
    std::string name_;
    std::string signature_;
    method_t method_;
    TypeInfoArray arg_type_array_;
protected:
    void preInvokeCheck(const std::vector<any>& args) const
    {
        if (args.size() != arg_type_array_.size())
        {
            BOOST_THROW_EXCEPTION(ArgTypeMismatchException());
        }
        else
        {
            try {
                BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE_FOR_CHECK, ~)
            } catch(boost::bad_any_cast&) {
                BOOST_THROW_EXCEPTION(ArgTypeMismatchException());
            }
        }
    }
public:
    BOOST_REFL_METHODREFL_BASECLASS(std::string n, std::string signature, method_t m)
        : name_(n), signature_(signature), method_(m)
    {
        BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CONSTRUCT_ARGTYPE_ARRAY, arg_type_array_)
    }
    std::string getName() const
    {
        return name_;
    }
    const TypeInfoArray& getParameterTypes() const
    {
        return arg_type_array_;
    }
    const std::type_info& getResultType() const
    {
        return typeid(F_result_type);
    }
    std::string getSignature() const
    {
        return signature_;
    }
    bool isStatic() const
    {
        return !isMemFunc;
    }
};

template<class C, class F, bool isConstMemFunc>
class MethodRefl<C, F, isConstMemFunc, true, false_type, false_type, BOOST_REFL_NUM_OF_ARGS>
    : public BOOST_REFL_METHODREFL_BASECLASS<C,F,isConstMemFunc,true> {
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef BOOST_REFL_METHODREFL_BASECLASS<C,F,isConstMemFunc,true> base_t;
public:
    MethodRefl(std::string n, std::string signature, typename base_t::method_t m)
        : base_t(n,signature,m)
    {
    }
    any invoke(any obj, const std::vector<any>& args) const
    {
        base_t::preInvokeCheck(args);
        return ((arg_cast<C*>(obj)->*base_t::method_)(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~)));
    }
};

template<class C, class F, bool isConstMemFunc>
class MethodRefl<C, F, isConstMemFunc, true, false_type, true_type, BOOST_REFL_NUM_OF_ARGS>
    : public BOOST_REFL_METHODREFL_BASECLASS<C,F,isConstMemFunc,true> {
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef BOOST_REFL_METHODREFL_BASECLASS<C,F,isConstMemFunc,true> base_t;
public:
    MethodRefl(std::string n, std::string signature, typename base_t::method_t m)
        : base_t(n,signature,m)
    {
    }
    any invoke(any obj, const std::vector<any>& args) const
    {
        base_t::preInvokeCheck(args);
        typedef typename remove_reference<typename base_t::F_result_type>::type F_result_type_remove_reference_t;
        typedef reference_wrapper<F_result_type_remove_reference_t> reference_wrapper_t;
        reference_wrapper_t return_reference(
            (arg_cast<C*>(obj)->*base_t::method_)(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~))
        );
        return return_reference;
    }
};

template<class C, class F, bool isConstMemFunc>
class MethodRefl<C, F, isConstMemFunc, true, true_type, false_type, BOOST_REFL_NUM_OF_ARGS>
    : public BOOST_REFL_METHODREFL_BASECLASS<C,F,isConstMemFunc,true> {
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef BOOST_REFL_METHODREFL_BASECLASS<C,F,isConstMemFunc,true> base_t;
public:
    MethodRefl(std::string n, std::string signature, typename base_t::method_t m)
        : base_t(n,signature,m)
    {
    }
    any invoke(any obj, const std::vector<any>& args) const
    {
        base_t::preInvokeCheck(args);
        (arg_cast<C*>(obj)->*base_t::method_)(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~));
        any void_ret;
        return void_ret;
    }
};

//static methods
template<class C, class F>
class MethodRefl<C, F, false, false, false_type, false_type, BOOST_REFL_NUM_OF_ARGS>
    : public BOOST_REFL_METHODREFL_BASECLASS<C,F,false,false> {
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef BOOST_REFL_METHODREFL_BASECLASS<C,F,false,false> base_t;
public:
    MethodRefl(std::string n, std::string signature, typename base_t::method_t m)
        : base_t(n,signature,m)
    {
    }
    any invokeStatic(std::vector<any> args) const
    {
        base_t::preInvokeCheck(args);
        return ((*base_t::method_)(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~)));
    }
};

template<class C, class F>
class MethodRefl<C, F, false, false, false_type, true_type, BOOST_REFL_NUM_OF_ARGS>
    : public BOOST_REFL_METHODREFL_BASECLASS<C,F,false, false> {
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef BOOST_REFL_METHODREFL_BASECLASS<C,F,false, false> base_t;
public:
    MethodRefl(std::string n, std::string signature, typename base_t::method_t m)
        : base_t(n,signature,m)
    {
    }
    any invokeStatic(std::vector<any> args) const
    {
        base_t::preInvokeCheck(args);
        typedef typename remove_reference<typename base_t::F_result_type>::type F_result_type_remove_reference_t;
        typedef reference_wrapper<F_result_type_remove_reference_t> reference_wrapper_t;
        reference_wrapper_t return_reference(
            (*base_t::method_)(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~))
        );
        return return_reference;
    }
};

template<class C, class F>
class MethodRefl<C, F, false, false, true_type, false_type, BOOST_REFL_NUM_OF_ARGS>
    : public BOOST_REFL_METHODREFL_BASECLASS<C,F,false, false> {
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    typedef BOOST_REFL_METHODREFL_BASECLASS<C,F,false, false> base_t;
public:
    MethodRefl(std::string n, std::string signature, typename base_t::method_t m)
        : base_t(n,signature,m)
    {
    }
    any invokeStatic(std::vector<any> args) const
    {
        base_t::preInvokeCheck(args);
        (*base_t::method_)(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~));
        any void_ret;
        return void_ret;
    }
};

template<class C, class F>
class ConstructorRefl<C, F, BOOST_REFL_NUM_OF_ARGS> : public ConstructorReflItf {
private:
    typedef typename function_traits<F>::result_type F_result_type;
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE, ~)
    BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE, ~)
    std::string signature_;
    TypeInfoArray arg_type_array_;
private:
    void preInvokeCheck(std::vector<any>& args) const
    {
        if (args.size() != arg_type_array_.size())
        {
            BOOST_THROW_EXCEPTION(ArgTypeMismatchException());
        }
        else
        {
            try {
                BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE_FOR_CHECK, ~)
            } catch(boost::bad_any_cast&) {
                BOOST_THROW_EXCEPTION(ArgTypeMismatchException());
            }
        }
    }
public:
    ConstructorRefl(std::string signature) : signature_(signature)
    {
        BOOST_PP_REPEAT(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CONSTRUCT_ARGTYPE_ARRAY, arg_type_array_)
    }
    any newInstance(std::vector<any> args)
    {
        preInvokeCheck(args);
        C* obj = new C(BOOST_PP_ENUM(BOOST_REFL_NUM_OF_ARGS, BOOST_REFL_CAST_ARGTYPE, ~));
        return obj;
    }
    std::string getSignature()
    {
        return signature_;
    }
    const TypeInfoArray& getParameterTypes() const
    {
        return arg_type_array_;
    }
};

#undef BOOST_REFL_NUM_OF_ARGS
#undef BOOST_REFL_ARGTYPE_NAME
#undef BOOST_REFL_ARGTYPE_TYPEDEF_NAME
#undef BOOST_REFL_ARGTYPE_REAL_TYPEDEF_NAME
#undef BOOST_REFL_ARGTYPE_REAL
#undef BOOST_REFL_ARGTYPE_REAL_TYPEDEF_DECLARE
#undef BOOST_REFL_CAST_ARGTYPE
#undef BOOST_REFL_CAST_ARGTYPE_FOR_CHECK
#undef BOOST_REFL_CONSTRUCT_ARGTYPE_ARRAY
#undef BOOST_REFL_ARGTYPE_TYPEDEF_DECLARE
#undef BOOST_REFL_METHODREFL_BASECLASS
