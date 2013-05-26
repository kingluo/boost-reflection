//
// Copyright (C) 2009-2013 King Luo (home_king@163.com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_REFLECTION_CLASS_HPP
#define BOOST_REFLECTION_CLASS_HPP

# if (defined(__GNUC__) && __GNUC__ >= 3) \
 || defined(_AIX) \
 || (   defined(__sgi) && defined(__host_mips)) \
 || (defined(__hpux) && defined(__HP_aCC)) \
 || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#  define BOOST_REFL_USE_TYPE_ID_NAME
#include <cstring>
# endif

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <typeinfo>
#include <algorithm>

#include <boost/config.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/global_fun.hpp>

#include <boost/any.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/throw_exception.hpp>
#include <boost/static_assert.hpp>
#include <boost/algorithm/string.hpp>

# if defined(BOOST_REFLECTION_SOURCE)
#   define BOOST_REFLECTION_DYNC_SYMBOL BOOST_SYMBOL_EXPORT
# else
#   define BOOST_REFLECTION_DYNC_SYMBOL BOOST_SYMBOL_IMPORT
# endif

#if !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_REFLECTION_NO_LIB) && !defined(BOOST_REFLECTION_SOURCE)
#  define BOOST_LIB_NAME boost_reflection
#  define BOOST_DYN_LINK
#  include <boost/config/auto_link.hpp>
#endif

#include <boost/config/abi_prefix.hpp> // must be the last #include

namespace boost {
namespace reflection {
namespace mi = ::boost::multi_index;

#define BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS 20
#define BOOST_REFL_MAX_NUM_OF_ANNOTATION_PARAMETERS 20

typedef reference_wrapper<const std::type_info> TypeInfoRef;
typedef std::vector<TypeInfoRef> TypeInfoArray;

#ifdef BOOST_REFL_USE_TYPE_ID_NAME
#define BOOST_REFL_IS_TYPE_SAME(typeInfo1, typeInfo2) \
    (std::strcmp(((const std::type_info&)(typeInfo1)).name(), ((const std::type_info&)(typeInfo2)).name()) == 0)
#define BOOST_REFL_IS_TYPE_BEFORE(typeInfo1, typeInfo2) \
    (std::strcmp(((const std::type_info&)(typeInfo1)).name(), ((const std::type_info&)(typeInfo2)).name()) < 0)
#else
#define BOOST_REFL_IS_TYPE_SAME(typeInfo1, typeInfo2) \
    (((const std::type_info&)(typeInfo1)) == ((const std::type_info&)(typeInfo2)))
#define BOOST_REFL_IS_TYPE_BEFORE(typeInfo1, typeInfo2) \
    (((const std::type_info&)(typeInfo1)).before(((const std::type_info&)(typeInfo2))))
#endif

#define BOOST_REFL_REAL_TYPE(userType) \
    boost::mpl::if_< \
        boost::is_reference<userType>::type, \
        boost::reference_wrapper<boost::remove_reference<userType>::type>, \
        userType \
    >::type
#define BOOST_REFL_TYPEID(userType) \
    boost::cref(typeid(BOOST_REFL_REAL_TYPE(userType)))

// exceptions

#define BOOST_REFL_DEFINE_EXCEPTION(exc) \
struct exc : public std::exception { \
    const char* what() const throw() \
    { \
        return "boost::reflection::"BOOST_STRINGIZE(exc); \
    } \
    ~exc() throw() {} \
};

BOOST_REFL_DEFINE_EXCEPTION(ClassNotFoundException)
BOOST_REFL_DEFINE_EXCEPTION(NoSuchMethodException)
BOOST_REFL_DEFINE_EXCEPTION(NoSuchFieldException)
BOOST_REFL_DEFINE_EXCEPTION(ArgTypeMismatchException)
BOOST_REFL_DEFINE_EXCEPTION(NoSuchConstructorException)
BOOST_REFL_DEFINE_EXCEPTION(NoDestructorException)
BOOST_REFL_DEFINE_EXCEPTION(FieldReadOnlyException)
BOOST_REFL_DEFINE_EXCEPTION(WrongMethodTypeException)
BOOST_REFL_DEFINE_EXCEPTION(FatalSystemException)
BOOST_REFL_DEFINE_EXCEPTION(AnnotationNotFoundException)
BOOST_REFL_DEFINE_EXCEPTION(IllegalStateException)
BOOST_REFL_DEFINE_EXCEPTION(IllegalObjectCastException)
BOOST_REFL_DEFINE_EXCEPTION(ProxyNotSupportedException)

// Annotation
inline std::string typeinfo_to_string(const boost::any& r)
{
    return r.type().name();
}
typedef mi::multi_index_container<
    boost::any,
    mi::indexed_by<
        mi::hashed_unique<mi::global_fun<const boost::any&, std::string, &typeinfo_to_string> >,
        mi::sequenced<>
    >
> annotation_db_t;
typedef annotation_db_t::iterator annotation_db_iterator_t;

class AnnotationBase {
private:
    annotation_db_t* annotation_db;
protected:
    AnnotationBase(const std::list<boost::any>& annotations) {
        //initialize the annotation db
        annotation_db = new annotation_db_t;
        std::copy(annotations.begin(), annotations.end(),
            std::back_inserter(mi::get<1>(*annotation_db)));
    }
    void finalize() {
        delete annotation_db;
    }
public:
    template<class AnnotationType>
    bool isAnnotationPresent() {
        return annotation_db->find(typeid(AnnotationType).name()) != annotation_db->end();
    }
    template<class AnnotationType>
    AnnotationType getAnnotation() {
        annotation_db_iterator_t it = annotation_db->find(typeid(AnnotationType).name());
        if (it != annotation_db->end()) {
            return boost::any_cast<AnnotationType>(*it);
        }
        BOOST_THROW_EXCEPTION(AnnotationNotFoundException());
    }
};

// make_list
#define BOOST_REFL_ANNOTATION_PARAMETER_PACK(z, n, data) \
    l.push_back(BOOST_PP_CAT(arg, n));
#define BOOST_REFL_ANNOTATION_PARAMETER_ARG(z, n, data) \
    T BOOST_PP_CAT(arg, n)
#define BOOST_REFL_MAKE_LIST_TEMPLATE_DEF(z, n, data) \
    template<class T> \
    std::list<T> make_list(BOOST_PP_ENUM(n, BOOST_REFL_ANNOTATION_PARAMETER_ARG, ~)) \
    { \
        std::list<T> l; \
        BOOST_PP_REPEAT(n, BOOST_REFL_ANNOTATION_PARAMETER_PACK, ~) \
        return l; \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_MAKE_LIST_TEMPLATE_DEF(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_ANNOTATION_PARAMETERS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_MAKE_LIST_TEMPLATE_DEF
#undef BOOST_REFL_ANNOTATION_PARAMETER_PACK
#undef BOOST_REFL_ANNOTATION_PARAMETER_ARG

// Field

class FieldReflItf {
public:
    virtual std::string getName() const = 0;
    virtual const std::type_info& type() const = 0;
    virtual any get(any obj) const = 0;
    virtual any getPointer(any obj) const = 0;
    virtual void set(any obj, any data) const = 0;
    virtual bool isReadOnly() const = 0;
    virtual ~FieldReflItf() {}
};

#define FIELD_IS_READONLY(P) typename is_const<P>::type
template <class C, class P, class READONLY = FIELD_IS_READONLY(P)>
class FieldRefl : public FieldReflItf {
};

template <class C, class P>
class FieldRefl<C, P, true_type> : public FieldReflItf {
private:
    std::string name_;
    typedef P (C::*field_t);
    field_t field_;
public:
    FieldRefl(std::string name, field_t field)
        : name_(name), field_(field)
    {
    }
    virtual std::string getName() const
    {
        return name_;
    }
    virtual const std::type_info& type() const
    {
        return typeid(P);
    }
    virtual any get(any obj) const
    {
        return (any_cast<C*>(obj)->*field_);
    }
    virtual any getPointer(any obj) const
    {
        BOOST_THROW_EXCEPTION(FieldReadOnlyException());
        return boost::any(); // only regarding the gcc -Wreturn-type warning
    }
    virtual void set(any obj, any data) const
    {
        BOOST_THROW_EXCEPTION(FieldReadOnlyException());
    }
    virtual bool isReadOnly()  const
    {
        return true;
    }
};

template <class C, class P>
class FieldRefl<C, P, false_type> : public FieldReflItf {
private:
    std::string name_;
    typedef P (C::*field_t);
    field_t field_;
public:
    FieldRefl(std::string name, field_t field)
        : name_(name), field_(field)
    {
    }
    virtual std::string getName() const
    {
        return name_;
    }
    virtual const std::type_info& type() const
    {
        return typeid(P);
    }
    virtual any get(any obj) const
    {
        return (any_cast<C*>(obj)->*field_);
    }
    virtual any getPointer(any obj) const {
        return &(any_cast<C*>(obj)->*field_);
    }
    virtual void set(any obj, any data) const
    {
        (any_cast<C*>(obj)->*field_) = any_cast<P>(data);
    }
    virtual bool isReadOnly() const
    {
        return false;
    }
};

class Field : public AnnotationBase {
private:
    FieldReflItf* impl_;
public:
    Field(FieldReflItf* impl, const std::list<boost::any>& annotations)
        : AnnotationBase(annotations), impl_(impl)
    {
    }
    void finalize() {
        AnnotationBase::finalize();
        delete impl_;
    }
    std::string getName() const
    {
        return impl_->getName();
    }
    const std::type_info& type() const
    {
        return impl_->type();
    }
    any get(any obj) const
    {
        return impl_->get(obj);
    }
    any getPointer(any obj) const {
        return impl_->getPointer(obj);
    }
    void set(any obj, any data) const
    {
        return impl_->set(obj, data);
    }
    bool isReadOnly() const
    {
        return impl_->isReadOnly();
    }
};

// Method

class MethodReflItf {
public:
    virtual std::string getName() const = 0;
    virtual std::string getSignature() const = 0;
    virtual any invoke(any obj, const std::vector<any>& args) const
    {
        (void)obj;
        (void)args;
        BOOST_THROW_EXCEPTION(WrongMethodTypeException());
    }
    virtual any invokeStatic(std::vector<any> args) const
    {
        (void)args;
        BOOST_THROW_EXCEPTION(WrongMethodTypeException());
    }
    virtual const TypeInfoArray& getParameterTypes() const = 0;
    virtual const std::type_info& getResultType() const = 0;
    virtual bool isStatic() const = 0;
    virtual ~MethodReflItf() {}
};

template<class C, class F, bool isConstMemFunc, bool isMemFunc,
    class ReturnVoid = typename is_void<typename function_traits<F>::result_type>::type,
    class ReturnRef = typename is_reference<typename function_traits<F>::result_type>::type,
    int NumOfArgs = function_traits<F>::arity>
class MethodRefl : public MethodReflItf {
};

#define BOOST_REFL_DECLARE_ARGS(z, n, data) any BOOST_PP_CAT(arg, n)
#define BOOST_REFL_DECLARE_ARG_PACK(z, n, data) args.push_back(BOOST_PP_CAT(arg, n));

class Method : public AnnotationBase {
public:
    Method(MethodReflItf* impl, const std::list<boost::any>& annotations)
        : AnnotationBase(annotations), impl_(impl), isClone(false)
    {
    }
    void finalize() {
        if (!isClone) {
            AnnotationBase::finalize();
            delete impl_;
        }
    }
    std::string getName() const
    {
        return impl_->getName();
    }
    const TypeInfoArray& getParameterTypes() const
    {
        return impl_->getParameterTypes();
    }
    const std::type_info& getResultType() const
    {
        return impl_->getResultType();
    }
    std::string getSignature() const
    {
        return impl_->getSignature();
    }
    bool isStatic() const
    {
        return impl_->isStatic();
    }
    any invoke(any obj, const std::vector<any>& args) const
    {
        return impl_->invoke(obj, args);
    }

#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    any invoke(any obj \
    BOOST_PP_IF(n, BOOST_PP_COMMA, BOOST_PP_EMPTY)() BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) const \
    { \
        std::vector<any> args; \
        BOOST_PP_REPEAT(n, BOOST_REFL_DECLARE_ARG_PACK, ~) \
        return impl_->invoke(obj, args); \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE

    any invokeStatic(std::vector<any> args) const
    {
        return impl_->invokeStatic(args);
    }

#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    any invokeStatic(BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) const \
    { \
        std::vector<any> args; \
        BOOST_PP_REPEAT(n, BOOST_REFL_DECLARE_ARG_PACK, ~) \
        return impl_->invokeStatic(args); \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE
private:
    void setClone() {
        isClone = true;
    }
    friend class Class;
private:
    MethodReflItf* impl_;
    /// indicates whether this Method is cloned from elsewhere
    /// if so, then the finalize will not work.
    bool isClone;
};

// Constructor & Destructor

class ConstructorReflItf {
public:
    virtual std::string getSignature() = 0;
    virtual any newInstance(std::vector<any> args) = 0;
    virtual const TypeInfoArray& getParameterTypes() const = 0;
    virtual ~ConstructorReflItf() {}
};

template<class C, class F, int NumOfArgs = function_traits<F>::arity>
class ConstructorRefl : public ConstructorReflItf {
};

class Constructor : public AnnotationBase {
public:
    Constructor(ConstructorReflItf* impl, const std::list<boost::any>& annotations)
        : AnnotationBase(annotations), impl_(impl)
    {
    }
    void finalize() {
        AnnotationBase::finalize();
        delete impl_;
    }
    std::string getSignature()
    {
        return impl_->getSignature();
    }
    any newInstance(std::vector<any> args)
    {
        return impl_->newInstance(args);
    }

#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    any newInstance(BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) \
    { \
        std::vector<any> args; \
        BOOST_PP_REPEAT(n, BOOST_REFL_DECLARE_ARG_PACK, ~) \
        return impl_->newInstance(args); \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE

    const TypeInfoArray& getParameterTypes() const
    {
        return impl_->getParameterTypes();
    }
private:
    ConstructorReflItf* impl_;
};

class DestructorReflItf {
public:
    virtual void deleteInstance(any obj) = 0;
    virtual ~DestructorReflItf() {}
};

template<class C>
class DestructorRefl : public DestructorReflItf {
    virtual void deleteInstance(any obj)
    {
        delete any_cast<C*>(obj);
    }
};

class Destructor {
    std::list<DestructorReflItf*>* dtors;
    Destructor(std::list<DestructorReflItf*>* dtors) : dtors(dtors) {}
    void finalize() {
        BOOST_FOREACH(DestructorReflItf* dtor, *dtors) {
            delete dtor;
        }
        delete dtors;
    }
    friend class Class;
public:
    void deleteInstance(any obj) {
        if (dtors->empty()) {
            BOOST_THROW_EXCEPTION(NoDestructorException());
        }
        BOOST_FOREACH(DestructorReflItf* dtor, *dtors) {
            try {
                dtor->deleteInstance(obj);
                return;
            } catch(boost::bad_any_cast&) {
                //ignore the exception
            }
        }
        BOOST_THROW_EXCEPTION(boost::bad_any_cast());
    }
};

template<class T>
class Deleter {
    Destructor dtor;
    Deleter(Destructor dtor) : dtor(dtor) {}
    friend class Class;
public:
    void operator()(T* p) {
        dtor.deleteInstance(p);
    }
};

// Interface

struct Interface {
    std::string qualified_name;
    std::string type_name;
    Interface(std::string n1, std::string n2)
        : qualified_name(n1), type_name(n2)
    {
    }
};

template<class C, class I>
Interface createInterface(std::string n1, std::string n2)
{
    BOOST_STATIC_ASSERT((boost::is_base_of<I, C>::value));
    return Interface(n1, n2);
}

// any caster

struct ObjectCastItf {
    virtual void* cast(any obj) = 0;
    virtual ~ObjectCastItf() {}
};

template <class C, class I = C>
struct ObjectCast : public ObjectCastItf {
    void* cast(any obj)
    {
        return static_cast<I*>(any_cast<C*>(obj));
    }
};

struct ObjectCastWrapper {
    std::string typeName;
    ObjectCastItf* objCast;
    ObjectCastWrapper(std::string typeName, ObjectCastItf* objCast)
        : typeName(typeName), objCast(objCast) {}
    void finalize() {
        delete objCast;
    }
};

// Container
struct ContainerItf; // forward declaration
struct IteratorItf {
    virtual bool hasNext() = 0;
    virtual void next() = 0;
    virtual void remove() = 0;
    virtual boost::any key() = 0;
    virtual boost::any value() = 0;
    virtual void setValue(boost::any v) = 0;
    virtual ~IteratorItf() {}
    virtual IteratorItf* clone() = 0;
    virtual ContainerItf* getAssoc() = 0;
};

class Iterator {
    IteratorItf* impl;
public:
    Iterator(IteratorItf* impl) :impl(impl) {}
    Iterator(const Iterator& other) {
        this->impl = other.impl->clone();
    }
    Iterator& operator=(const Iterator& other) {
        if (this != &other) { // protect against invalid self-assignment
            this->impl = other.impl->clone();
        }
        return *this;
    }
    ~Iterator();
    bool hasNext() {
        return impl->hasNext();
    }
    void next() {
        return impl->next();
    }
    void remove() {
        return impl->remove();
    }
    boost::any key() {
        return impl->key();
    }
    boost::any value() {
        return impl->value();
    }
    void setValue(boost::any v) {
        return impl->setValue(v);
    }
};

struct ContainerItf {
    virtual boost::any newInstance() = 0;
    virtual void deleteInstance(boost::any c) = 0;
    virtual void add(boost::any c, boost::any element, boost::any key = boost::any()) = 0;
    virtual Iterator find(boost::any c, boost::any key) = 0;
    virtual Iterator iterator(boost::any c) = 0;
    virtual std::size_t count(boost::any c) = 0;
    virtual ~ContainerItf() {}
    virtual void deleteIterator(IteratorItf* iter) = 0;
};

inline Iterator::~Iterator() {
    //MUST be delegated to the container impl,
    //in case of cross library heap memory release
    impl->getAssoc()->deleteIterator(impl);
}

class Container {
public:
    Container(const std::string& name, const std::string& pName, ContainerItf* impl)
        : name(name), pName(pName), impl(impl) {
    }
    void finalize() {
        delete impl;
    }
    boost::any newInstance() {
        return impl->newInstance();
    }
    void deleteInstance(boost::any c) {
        return impl->deleteInstance(c);
    }
    void add(boost::any c, boost::any element, boost::any key = boost::any()) {
        return impl->add(c, element, key);
    }
    Iterator find(boost::any c, boost::any key) {
        return impl->find(c, key);
    }
    Iterator iterator(boost::any c) {
        return impl->iterator(c);
    }
    std::size_t count(boost::any c) {
        return impl->count(c);
    }
    static std::string normalizeName(const std::string& str) {
        std::string v(str);
        boost::trim(v);
        //eliminate the spaces between the element type name and '>'
        std::size_t pos = v.find_last_of('>');
        if (pos != std::string::npos) {
            while(--pos != std::string::npos) {
                if (isspace(v[pos]) != 0) {
                    v.erase(pos, 1);
                } else if (v[pos] != '*') {
                    break;
                }
            }
        }
        return v;
    }
public:
    std::string name;
    std::string pName;
private:
    ContainerItf* impl;
};

//-----#-----
struct InvocationHandler {
    virtual boost::any invoke(boost::any proxy, std::string method, std::vector<boost::any> args) = 0;
    virtual ~InvocationHandler() {}
};

struct ProxyBase {
    virtual InvocationHandler*     getInvocationHandler() = 0;
    virtual ~ProxyBase() {}
};

struct ProxyCreator {
    virtual ProxyBase* createProxy(InvocationHandler* h) = 0;
    virtual ~ProxyCreator() {}
};
//-----#-----

/// Represent the meta-data used by reflection for C++ class
/**
* The Class class represents the reflected C++ class, which enables reflection upon
* declared members via BOOST_REFL_CLASS macro, including:
* @li constructor
* @li method
* @li field
* Moreover, you can access the annotations declared on those members if any.
* It has Java-like API with similiar prototype.
*/
class Class : public AnnotationBase {
private:
    typedef mi::multi_index_container<
        Field,
        mi::indexed_by<
            mi::hashed_unique<
                BOOST_MULTI_INDEX_CONST_MEM_FUN(Field, std::string, getName)
            >,
            mi::sequenced<>
        >
    > field_db_t;
    typedef field_db_t::iterator field_db_iterator_t;

    struct method_key : mi::composite_key<
        Method,
        BOOST_MULTI_INDEX_CONST_MEM_FUN(Method, std::string, getName),
        BOOST_MULTI_INDEX_CONST_MEM_FUN(Method, bool, isStatic),
        BOOST_MULTI_INDEX_CONST_MEM_FUN(Method, const TypeInfoArray&, getParameterTypes)
    >{};
    struct MethodLessCompare {
        bool operator()(const TypeInfoArray& a, const TypeInfoArray& b) const
        {
            if (a.size() != b.size())
            {
                return a.size() < b.size();
            }
            for(std::size_t idx = 0; idx < a.size(); idx++)
            {
                if (BOOST_REFL_IS_TYPE_BEFORE((const std::type_info&)a[idx], (const std::type_info&)b[idx]))
                {
                    return true;
                }
            }
            return false;
        }
    };
    typedef mi::composite_key_compare<std::less<std::string>, std::less<bool>, MethodLessCompare> method_key_compare;
    typedef mi::multi_index_container<
        Method,
        mi::indexed_by<
            mi::ordered_unique<
                method_key,
                method_key_compare
            >,
            mi::sequenced<>
        >
    > method_db_t;
    typedef method_db_t::iterator method_db_iterator_t;

    typedef mi::multi_index_container<
        Constructor,
        mi::indexed_by<
            mi::ordered_unique<
                BOOST_MULTI_INDEX_CONST_MEM_FUN(Constructor, const TypeInfoArray&, getParameterTypes),
                MethodLessCompare
            >,
            mi::sequenced<>
        >
    > constructor_db_t;
    typedef constructor_db_t::iterator constructor_db_iterator_t;

    typedef mi::multi_index_container<
        Interface,
        mi::indexed_by<
            mi::hashed_unique<
                BOOST_MULTI_INDEX_MEMBER(Interface, std::string, type_name)
            >,
            mi::hashed_unique<
                BOOST_MULTI_INDEX_MEMBER(Interface, std::string, qualified_name)
            >,
            mi::sequenced<>
        >
    > interface_db_t;

    typedef mi::multi_index_container<
        ObjectCastWrapper,
        mi::indexed_by<
            mi::hashed_unique<
                BOOST_MULTI_INDEX_MEMBER(ObjectCastWrapper, std::string, typeName)
            >,
            mi::sequenced<>
        >
    > objCast_db_t;
    typedef objCast_db_t::iterator objCast_db_iterator_t;
public:
// constructor invocation wrappers
    any newInstance(std::vector<any> args)
    {
        TypeInfoArray tArray;
        BOOST_FOREACH(boost::any& arg, args) {
            tArray.push_back(cref(arg.type()));
        }
        Constructor c = getConstructor(tArray);
        return c.newInstance(args);
    }
#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    any newInstance(BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) \
    { \
        std::vector<any> args; \
        BOOST_PP_REPEAT(n, BOOST_REFL_DECLARE_ARG_PACK, ~) \
        return newInstance(args); \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE

#define BOOST_REFL_ENUM_ARGS(z, n, data) BOOST_PP_CAT(arg, n)
#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    template<class T> \
    T newInstance(BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) \
    { \
        BOOST_STATIC_ASSERT(is_pointer<T>::value); \
        typedef typename remove_pointer<T>::type c_t; \
        T p = 0; \
        if (BOOST_REFL_IS_TYPE_SAME(typeid(c_t), this->type()) || this->supportInterface(typeid(c_t).name())) { \
            try { \
                boost::any o = newInstance(BOOST_PP_ENUM(n, BOOST_REFL_ENUM_ARGS, ~)); \
                p = static_cast<T>(this->cast(typeid(T).name(), o)); \
            } catch(NoSuchConstructorException&) { \
            } \
        } \
        return p; \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE
#undef BOOST_REFL_ENUM_ARGS

    // destructor invocation wrapper
    void deleteInstance(any obj)
    {
        destructor_.deleteInstance(obj);
    }

// method invocation wrappers
#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    any invoke(any obj, const std::string& methodName \
    BOOST_PP_IF(n, BOOST_PP_COMMA, BOOST_PP_EMPTY)() BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) \
    { \
        std::vector<any> args; \
        BOOST_PP_REPEAT(n, BOOST_REFL_DECLARE_ARG_PACK, ~) \
        return invoke(obj, methodName, args); \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE

#define BOOST_REFL_DEFINE_METHOD_INVOKE(z, n, data) \
    any invokeStatic(const std::string& methodName \
    BOOST_PP_IF(n, BOOST_PP_COMMA, BOOST_PP_EMPTY)() BOOST_PP_ENUM(n, BOOST_REFL_DECLARE_ARGS, ~)) \
    { \
        std::vector<any> args; \
        BOOST_PP_REPEAT(n, BOOST_REFL_DECLARE_ARG_PACK, ~) \
        return invokeStatic(methodName, args); \
    }
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_DEFINE_METHOD_INVOKE(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_DEFINE_METHOD_INVOKE

#undef BOOST_REFL_DECLARE_ARG_PACK
#undef BOOST_REFL_DECLARE_ARGS

    any invoke(any obj, const std::string& methodName, const std::vector<any>& args)
    {
        TypeInfoArray tArray;
        BOOST_FOREACH(const boost::any& arg, args) {
            tArray.push_back(cref(arg.type()));
        }
        Method m = getMethod(methodName, tArray);
        return m.invoke(obj, args);
    }

    any invokeStatic(const std::string& methodName, const std::vector<any>& args)
    {
        TypeInfoArray tArray;
        BOOST_FOREACH(const boost::any& arg, args) {
            tArray.push_back(cref(arg.type()));
        }
        Method m = getStaticMethod(methodName, tArray);
        return m.invokeStatic(args);
    }

    // field access wrappers
    any get(any obj, const std::string& fieldName)
    {
        Field field = getField(fieldName);
        return field.get(obj);
    }

    any getPointer(any obj, const std::string& fieldName)
    {
        Field field = getField(fieldName);
        return field.getPointer(obj);
    }

    void set(any obj, const std::string& fieldName, any data)
    {
        Field field = getField(fieldName);
        return field.set(obj, data);
    }
private:
    std::string name_;
    std::string ptrTypeName_;
    objCast_db_t* objCastDb;
    TypeInfoRef typeInfo_;
    constructor_db_t* constructorDb;
    Destructor destructor_;
    method_db_t* methodDb;
    field_db_t* fieldDb;
    interface_db_t* interfaceDb;
    ProxyCreator* proxyCreator;
public:
    void finalize() {
        AnnotationBase::finalize();
        BOOST_FOREACH(Constructor v, mi::get<1>(*constructorDb)) {
            v.finalize();
        }
        BOOST_FOREACH(Method v, mi::get<1>(*methodDb)) {
            v.finalize();
        }
        BOOST_FOREACH(Field v, mi::get<1>(*fieldDb)) {
            v.finalize();
        }
        BOOST_FOREACH(ObjectCastWrapper v, mi::get<1>(*objCastDb)) {
            v.finalize();
        }
        destructor_.finalize();
        delete constructorDb;
        delete methodDb;
        delete fieldDb;
        delete interfaceDb;
        delete proxyCreator;
        std::cout << "META-DATA FINALIZE class=" << name_ << std::endl;
    }
public:
    std::string getName() const
    {
        return name_;
    }
    const std::type_info& type() const
    {
        return typeInfo_;
    }
    std::string getTypeName() const
    {
        return type().name();
    }
    std::string getPtrTypeName() const
    {
        return ptrTypeName_;
    }
    void* cast(const std::string& targetTypeName, any obj) const
    {
        objCast_db_iterator_t it = objCastDb->find(targetTypeName);
        if (it != objCastDb->end()) {
            return it->objCast->cast(obj);
        }
        BOOST_THROW_EXCEPTION(IllegalObjectCastException());
    }
public:
    std::list<Constructor> getConstructors()
    {
        std::list<Constructor> v;
        std::copy(mi::get<1>(*constructorDb).begin(), mi::get<1>(*constructorDb).end(), std::back_inserter(v));
        return v;
    }
    Constructor getConstructor(const TypeInfoArray& arg_type_array)
    {
        constructor_db_iterator_t it = constructorDb->find(arg_type_array);
        if (it != constructorDb->end())
        {
            return *it;
        }
        BOOST_THROW_EXCEPTION(NoSuchConstructorException());
    }
    Constructor getConstructor()
    {
        TypeInfoArray emptyArray;
        return getConstructor(emptyArray);
    }
    Destructor getDestructor()
    {
        return destructor_;
    }
    std::list<Method> getMethods()
    {
        std::list<Method> v;
        std::copy(mi::get<1>(*methodDb).begin(), mi::get<1>(*methodDb).end(), std::back_inserter(v));
        BOOST_FOREACH(Method& m, v) {
            m.setClone();
        }
        return v;
    }
    Method getMethod(const std::string& methodName, const TypeInfoArray& arg_type_array, bool isStatic = false)
    {
        method_db_iterator_t it = methodDb->find(boost::make_tuple(methodName, isStatic,
            reference_wrapper<const TypeInfoArray>(arg_type_array)));
        if (it != methodDb->end())
        {
            Method m = *it;
            m.setClone();
            return m;
        }
        BOOST_THROW_EXCEPTION(NoSuchMethodException());
    }
    Method getMethod(const std::string& methodName, bool isStatic = false)
    {
        TypeInfoArray emptyArray;
        return getMethod(methodName, emptyArray, isStatic);
    }
    Method getStaticMethod(const std::string& methodName)
    {
        return getMethod(methodName, true);
    }
    Method getStaticMethod(const std::string& methodName, const TypeInfoArray& arg_type_array)
    {
        return getMethod(methodName, arg_type_array, true);
    }
    std::list<Field> getFields()
    {
        std::list<Field> v;
        std::copy(mi::get<1>(*fieldDb).begin(), mi::get<1>(*fieldDb).end(), std::back_inserter(v));
        return v;
    }
    Field getField(const std::string& fieldName)
    {
        field_db_iterator_t it = fieldDb->find(fieldName);
        if (it != fieldDb->end())
        {
            return *it;
        }
        BOOST_THROW_EXCEPTION(NoSuchFieldException());
    }
    bool supportInterface(const std::string& type_name)
    {
        return interfaceDb->find(type_name) != interfaceDb->end();
    }
    ProxyBase* createProxy(InvocationHandler* h)
    {
        return proxyCreator->createProxy(h);
    }
private:
    Class(const std::string& name, const std::string& ptrTypeName,
        const std::list<ObjectCastWrapper>& objCasts, const std::type_info& typeInfo,
        const std::list<Constructor>& constructors, std::list<DestructorReflItf*>* dtors,
        const std::list<Method>& methods, const std::list<Field>& fields,
        const std::list<Interface>& interfaces, const std::list<boost::any>& annotations,
        ProxyCreator* proxyCreator)
        : AnnotationBase(annotations), name_(name), ptrTypeName_(ptrTypeName),
          typeInfo_(cref(typeInfo)), destructor_(dtors), proxyCreator(proxyCreator)
    {
        //initialize the constructor db
        constructorDb = new constructor_db_t;
        std::copy(constructors.begin(), constructors.end(), std::back_inserter(mi::get<1>(*constructorDb)));

        //initialize the field db
        fieldDb = new field_db_t;
        std::copy(fields.begin(), fields.end(), std::back_inserter(mi::get<1>(*fieldDb)));

        //initialize the method db
        methodDb = new method_db_t;
        std::copy(methods.begin(), methods.end(), std::back_inserter(mi::get<1>(*methodDb)));

        //initialize the interface db
        interfaceDb = new interface_db_t;
        std::copy(interfaces.begin(), interfaces.end(), std::back_inserter(mi::get<2>(*interfaceDb)));

        //initialize the objCast db
        objCastDb = new objCast_db_t;
        std::copy(objCasts.begin(), objCasts.end(), std::back_inserter(mi::get<1>(*objCastDb)));
    }
    /////////class db///////
private:
    struct typename_tag {};
    struct typeid_name_tag {};
    struct class_name_tag {};
    typedef mi::multi_index_container<
        Class,
        mi::indexed_by<
            mi::hashed_unique<
                mi::tag<typeid_name_tag>,
                BOOST_MULTI_INDEX_CONST_MEM_FUN(Class, std::string, getPtrTypeName)
            >,
            mi::ordered_unique<
                mi::tag<class_name_tag>,
                BOOST_MULTI_INDEX_CONST_MEM_FUN(Class, std::string, getName)
            >,
            mi::hashed_unique<
                mi::tag<typename_tag>,
                BOOST_MULTI_INDEX_CONST_MEM_FUN(Class, std::string, getTypeName)
            >
        >
    > class_db_t;

    typedef mi::multi_index_container<
        Container,
        mi::indexed_by<
            mi::hashed_unique<
                mi::tag<typeid_name_tag>,
                BOOST_MULTI_INDEX_MEMBER(Container, std::string, pName)
            >,
            mi::hashed_unique<
                mi::tag<class_name_tag>,
                BOOST_MULTI_INDEX_MEMBER(Container, std::string, name)
            >
        >
    > container_db_t;

    struct db_t {
        class_db_t* class_db;
        container_db_t* container_db;
        db_t() {
            class_db = new class_db_t;
            container_db = new container_db_t;
        }
    };

    BOOST_REFLECTION_DYNC_SYMBOL static db_t* getDb();

    template<class Tag>
    static Class findClass(const std::string& name)
    {
        typedef typename mi::index<class_db_t, Tag>::type index_t;
        typedef typename index_t::iterator iterator_t;
        index_t& db = mi::get<Tag>(*(getDb()->class_db));
        iterator_t it = db.find(name);
        if (it != db.end())
        {
            return *it;
        }
        BOOST_THROW_EXCEPTION(ClassNotFoundException());
    }

    template<class Tag>
    static Container findContainer(const std::string& name)
    {
        typedef typename mi::index<container_db_t, Tag>::type index_t;
        typedef typename index_t::iterator iterator_t;
        index_t& db = mi::get<Tag>(*(getDb()->container_db));
        iterator_t it = db.find(name);
        if (it != db.end())
        {
            return *it;
        }
        BOOST_THROW_EXCEPTION(ClassNotFoundException());
    }
public:
    static void registerClass(const std::string& name, const std::string& ptrTypeName,
        const std::list<ObjectCastWrapper>& objCasts, const std::type_info& typeInfo,
        const std::list<Constructor>& constructors, std::list<DestructorReflItf*>* dtors,
        const std::list<Method>& methods, const std::list<Field>& fields, const std::list<Interface>& interfaces,
        const std::list<boost::any>& annotations, ProxyCreator* proxyCreator)
    {
        bool ok = getDb()->class_db->insert(
            Class(name, ptrTypeName, objCasts, typeInfo,
                constructors, dtors, methods,
                fields, interfaces, annotations, proxyCreator)
            ).second;
        (void)ok;
        BOOST_ASSERT(ok);
    }

    static void unRegisterClass(const std::string& name) {
        typedef mi::index<class_db_t, class_name_tag>::type index_t;
        typedef index_t::iterator iterator_t;
        index_t& db = mi::get<class_name_tag>(*(getDb()->class_db));
        iterator_t it = db.find(name);
        BOOST_ASSERT(it != db.end());
        Class c = *it;
        c.finalize();
        db.erase(it);
    }

    static Class forObject(any object)
    {
        return findClass<typeid_name_tag>(object.type().name());
    }

    static Class forName(const std::string& name)
    {
        return findClass<class_name_tag>(name);
    }

    static Class forTypeName(const std::string& name)
    {
        return findClass<typename_tag>(name);
    }

    template<class T>
    static Deleter<T> getDeleter(T* p) {
        Class t = forTypeName(typeid(*p).name());
        return Deleter<T>(t.destructor_);
    }

    static bool isClassRegistered(const std::string& name) {
        typedef mi::index<class_db_t, class_name_tag>::type index_t;
        typedef index_t::iterator iterator_t;
        index_t& db = mi::get<class_name_tag>(*(getDb()->class_db));
        iterator_t it = db.find(name);
        return it != db.end();
    }

    static std::list<Class> forNamespace(const std::string& ns)
    {
        std::list<Class> classes;
        typedef mi::index<class_db_t, class_name_tag>::type index_t;
        typedef index_t::iterator iterator_t;
        index_t& db = mi::get<class_name_tag>(*(getDb()->class_db));
        iterator_t it = db.lower_bound(ns);
        while(it != db.end()) {
            if ((it->getName().compare(0, ns.size(), ns)) != 0) {
                break;
            }
            classes.push_back(*it);
            it++;
        }
        return classes;
    }

    static void registerContainer(const Container& c)
    {
        bool ok = getDb()->container_db->insert(c).second;
        (void)ok;
        BOOST_ASSERT(ok);
    }

    static void unRegisterContainer(const std::string& name) {
        typedef mi::index<container_db_t, class_name_tag>::type index_t;
        typedef index_t::iterator iterator_t;
        index_t& db = mi::get<class_name_tag>(*(getDb()->container_db));
        iterator_t it = db.find(name);
        BOOST_ASSERT(it != db.end());
        Container c = *it;
        c.finalize();
        db.erase(it);
    }

    static bool isContainerRegistered(const std::string& name) {
        typedef mi::index<container_db_t, class_name_tag>::type index_t;
        typedef index_t::iterator iterator_t;
        index_t& db = mi::get<class_name_tag>(*(getDb()->container_db));
        iterator_t it = db.find(name);
        return it != db.end();
    }

    static Container forContainerObject(any object)
    {
        return findContainer<typeid_name_tag>(object.type().name());
    }

    static Container forContainerName(const std::string& name)
    {
        return findContainer<class_name_tag>(name);
    }
};

#define BOOST_REFL_ANY_OBJECT_GUARD(t, o) \
struct BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(boost_reflection_any_object_guard_t_, __LINE__), t), o) { \
    Class cls; \
    boost::any obj; \
    BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(boost_reflection_any_object_guard_t_, __LINE__), t), o) \
        (Class cls, boost::any obj) : cls(cls), obj(obj) {} \
    ~BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(boost_reflection_any_object_guard_t_, __LINE__), t), o) () { \
        cls.deleteInstance(obj); \
    } \
} BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(boost_reflection_any_object_guard_t_, __LINE__), t), o), _var) (t,o);

template<class T>
T interface_cast(any obj)
{
    BOOST_STATIC_ASSERT(is_pointer<T>::value);
    typedef typename remove_pointer<T>::type c_t;
    try {
        Class c = Class::forObject(obj);
        if (c.supportInterface(typeid(c_t).name())) {
            return static_cast<T>(c.cast(typeid(T).name(), obj));
        } else {
            return 0;
        }
    } catch(ClassNotFoundException&) {
        return 0;
    } catch(IllegalObjectCastException&) {
        return 0;
    }
}

template<class T, bool b>
T arg_cast_impl(any obj, const boost::integral_constant<bool, b>&)
{
    return boost::any_cast<T>(obj);
}

template<class T>
T arg_cast_impl(any obj, const boost::true_type&)
{
    try {
        return boost::any_cast<T>(obj);
    } catch(boost::bad_any_cast&) {
        T p = interface_cast<T>(obj);
        if (!p) {
            BOOST_THROW_EXCEPTION(boost::bad_any_cast());
        }
        return p;
    }
}

template<class T>
T arg_cast(any obj)
{
    return arg_cast_impl<T>(obj, is_pointer<T>());
}

// std::list container reflection
template<class C>
class ContainerImpl : public ContainerItf {
    typedef typename C::value_type v_t;
    typedef typename C::iterator i_t;
    class IteratorItfImpl : public IteratorItf {
    public:
        virtual bool hasNext() {
            i_t next_i = iter;
            if (proceed) {
                next_i++;
            }
            return next_i != c->end();
        }
        virtual void next() {
            if (!proceed) {
                proceed = true;
            } else {
                iter++;
            }
        }
        virtual void remove() {
            if (!proceed) {
                // if the next method has not yet been called,
                // or the remove method has already been called
                // after the last call to the next method
                BOOST_THROW_EXCEPTION(IllegalStateException());
            }
            iter = c->erase(iter);
            proceed = false; // stop next() once
        }
        virtual boost::any key() {
            return boost::any();
        }
        virtual boost::any value() {
            return *iter;
        }
        virtual void setValue(boost::any v) {
            *iter = arg_cast<v_t>(v);
        }
        IteratorItfImpl(i_t iter, C* c, ContainerItf* assoc)
            : iter(iter), c(c), proceed(false), assoc(assoc) {
        }
        virtual ContainerItf* getAssoc() {
            return assoc;
        }
        IteratorItf* clone() {
            return new IteratorItfImpl(*this);
        }
    private:
        i_t iter;
        C* c;
        bool proceed;
        ContainerItf* assoc;
    };
public:
    virtual boost::any newInstance() {
        return new C;
    }
    virtual void deleteInstance(boost::any c) {
        delete boost::any_cast<C*>(c);
    }
    virtual Iterator iterator(boost::any c) {
        return Iterator(new IteratorItfImpl(boost::any_cast<C*>(c)->begin(), boost::any_cast<C*>(c), this));
    }
    virtual Iterator find(boost::any c, boost::any key) {
        (void)key;
        return iterator(c);
    }
    virtual std::size_t count(boost::any c) {
        return boost::any_cast<C*>(c)->size();
    }
    virtual void add(boost::any c, boost::any element, boost::any key = boost::any()) {
        (void)key;
        boost::any_cast<C*>(c)->push_back(arg_cast<v_t>(element));
    }
    virtual void deleteIterator(IteratorItf* iter) {
        delete iter;
    }
};

// std::map container reflection
template<class C>
class ContainerImplWithKey : public ContainerItf {
    typedef typename C::value_type kv_t;
    typedef typename C::key_type k_t;
    typedef typename C::mapped_type v_t;
    typedef typename C::iterator i_t;
    class IteratorItfImpl : public IteratorItf {
    public:
        virtual bool hasNext() {
            i_t next_i = iter;
            if (proceed) {
                next_i++;
            }
            return next_i != c->end();
        }
        virtual void next() {
            if (!proceed) {
                proceed = true;
            } else {
                iter++;
            }
        }
        virtual void remove() {
            if (!proceed) {
                // if the next method has not yet been called,
                // or the remove method has already been called
                // after the last call to the next method
                BOOST_THROW_EXCEPTION(IllegalStateException());
            }
            c->erase(iter++);
            proceed = false; // stop next() once
        }
        virtual boost::any key() {
            return iter->first;
        }
        virtual boost::any value() {
            return iter->second;
        }
        virtual void setValue(boost::any v) {
            (*iter).second = arg_cast<v_t>(v);
        }
        IteratorItfImpl(i_t iter, C* c, ContainerItf* assoc)
            : iter(iter), c(c), proceed(false), assoc(assoc) {
        }
        virtual ContainerItf* getAssoc() {
            return assoc;
        }
        IteratorItf* clone() {
            return new IteratorItfImpl(*this);
        }
    private:
        i_t iter;
        C* c;
        bool proceed;
        ContainerItf* assoc;
    };
public:
    virtual boost::any newInstance() {
        return new C;
    }
    virtual void deleteInstance(boost::any c) {
        delete boost::any_cast<C*>(c);
    }
    virtual Iterator iterator(boost::any c) {
        return Iterator(new IteratorItfImpl(
            boost::any_cast<C*>(c)->begin(), boost::any_cast<C*>(c), this));
    }
    virtual Iterator find(boost::any c, boost::any key) {
        return Iterator(new IteratorItfImpl(
            boost::any_cast<C*>(c)->find(boost::any_cast<k_t>(key)),
            boost::any_cast<C*>(c), this));
    }
    virtual std::size_t count(boost::any c) {
        return boost::any_cast<C*>(c)->size();
    }
    virtual void add(boost::any c, boost::any element, boost::any key = boost::any()) {
        boost::any_cast<C*>(c)->insert(kv_t(
            boost::any_cast<k_t>(key), arg_cast<v_t>(element)));
    }
    virtual void deleteIterator(IteratorItf* iter) {
        delete iter;
    }
};

// MethodRefl template specialization expand
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_REFL_MAX_NUM_OF_FUNCTION_ARGUMENTS)
#define BOOST_PP_FILENAME_1 <boost/reflection/detail/method_template.hpp>
#include BOOST_PP_ITERATE()

//-------------

template<class v, int size=boost::mpl::size<v>::value>
struct Proxy {};

template<class v>
struct Proxy<v,0>  : public ProxyBase {
    InvocationHandler* h;
    Proxy(InvocationHandler* h) : h(h) {}
    InvocationHandler*     getInvocationHandler() {
        BOOST_THROW_EXCEPTION(ProxyNotSupportedException());
        return 0;
    }
};

#define BOOST_REFL_INHERIT_CONCERTE_INTERFACE_DECLARE(z,n,text) \
    public boost::mpl::at< v, boost::mpl::int_<n> >::type
#define BOOST_REFL_INHERIT_CONCERTE_INTERFACE_TYPEDEF(z,n,text) \
    typedef typename boost::mpl::at< v, boost::mpl::int_<n> >::type BOOST_PP_CAT(base_type_, n);
#define BOOST_REFL_INHERIT_CONCERTE_INTERFACE_INIT(z,n,text) \
    BOOST_PP_CAT(base_type_, n)(h)

#define BOOST_REFL_MAX_IMPLEMENT_INTERFACES 10
#define BOOST_REFL_PROXY_DEFINITION(z, n, data) \
    template<class v> \
    struct Proxy<v, n> : public ProxyBase, \
        BOOST_PP_ENUM(n, BOOST_REFL_INHERIT_CONCERTE_INTERFACE_DECLARE, ~) { \
        InvocationHandler* h; \
        BOOST_PP_REPEAT(n, BOOST_REFL_INHERIT_CONCERTE_INTERFACE_TYPEDEF, ~) \
        Proxy(InvocationHandler* h) : BOOST_PP_ENUM(n, BOOST_REFL_INHERIT_CONCERTE_INTERFACE_INIT, ~), h(h) { \
        } \
        InvocationHandler*     getInvocationHandler() { \
            return h; \
        } \
    };
#define BOOST_PP_LOCAL_MACRO(n) BOOST_REFL_PROXY_DEFINITION(~, n, ~)
#define BOOST_PP_LOCAL_LIMITS (1, BOOST_REFL_MAX_IMPLEMENT_INTERFACES)
#include BOOST_PP_LOCAL_ITERATE()
#undef BOOST_REFL_PROXY_DEFINITION

template<class ReturnType,
    class ReturnVoid = typename boost::is_void<ReturnType>::type,
    class ReturnRef = typename boost::is_reference<ReturnType>::type>
struct InvocationHandlerInvoke {
    ReturnType operator()(InvocationHandler* h, boost::any proxy, std::string method, std::vector<boost::any> args) {
        return boost::any_cast<ReturnType>(h->invoke(proxy, method, args));
    }
};

//FIXME
//how about return const reference?
template<class ReturnType>
struct InvocationHandlerInvoke<ReturnType, boost::true_type, boost::false_type> {
    ReturnType operator()(InvocationHandler* h, boost::any proxy, std::string method, std::vector<boost::any> args) {
        h->invoke(proxy, method, args);
        return;
    }
};

template<class ReturnType>
struct InvocationHandlerInvoke<ReturnType, boost::false_type, boost::true_type> {
    typedef typename boost::remove_reference<ReturnType>::type rtype_t;
    typedef boost::reference_wrapper<rtype_t> reference_wrapper_t;
    ReturnType operator()(InvocationHandler* h, boost::any proxy, std::string method, std::vector<boost::any> args) {
        return boost::any_cast<reference_wrapper_t>(h->invoke(proxy, method, args));
    }
};

template<class ParamType,
    class ParamRef = typename boost::is_reference<ParamType>::type>
struct InvocationHandlerParamWrap {
    ParamType& operator()(ParamType& arg) {
        return arg;
    }
};

//FIXME
//how about const reference param?
template<class ParamType>
struct InvocationHandlerParamWrap<ParamType, boost::true_type> {
    typedef boost::reference_wrapper<typename boost::remove_reference<ParamType>::type> return_type_t;
    return_type_t operator()(ParamType arg) {
        return return_type_t(arg);
    }
};

//---------

} //namespace reflection
} //namespace boost

#include <boost/config/abi_suffix.hpp> // pops abi_prefix.hpp pragmas

#endif //BOOST_REFLECTION_CLASS_HPP
