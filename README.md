# Boost Reflection Library

## Introduction

This library provides **Java-like Reflection API** to C++ language.

First, you should declare which classes to be reflected, via some simple register macros.

These declarations are **non-intrusive** to the reflected classes themselves, and they could be
placed at anywhere (same **header file**, separate header file, or source file),
as long as they appears behind the definitions of the reflected classes. 

This non-intrusive feature means you could even declare and reflect classes from third-party libraries,
which are normally read-only and closed-source binaries.

Then, you could use the API to find the reflected classes by name, and use them, e.g.
* instantiate objects
* invoke methods upon the objects

Besides the basic reflection, it has other highlighted features:
* **Java-like Annotation** (could be bound on classes, constructors, methods, fields)
* **Java-like Object Proxy** (C++ interface based, could be used to implement somehow AOP framework)
* Support dynamic loaded library (you could reflect classes declared and compiled within libraries)

## Usage

### Build & Test

#### prerequisite

```
boost >= 1.46 (header files only)
cmake >= 2.8.8
gcc >= 4.6 (for Linux)
vc2008 or later versions (for Windows)
```

#### Windows

* install vc2008 or later versions (for later versions, you need to adjust batch files)
* invoke `build.bat`
* invoke `test.bat`

#### Linux

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Release ../
make
make test
```

### Install & Run Example

#### Windows

* download and install NSIS (Nullsoft Scriptable Install System)
* invoke `package.bat`
* invoke `build/boost-reflection-1.0.0-win32.exe`
* cd into `C:\Program Files\boost-reflection\example\ioc`
* invoke `test_ioc -xml driver_manager.xml -xml foo.xml -xml bar.xml -lib foo.dll -lib bar.dll`
* invoke `test_annotation_mvc -xml mvc_dispatcher.xml`

#### Linux

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Release ../
make
make install
ldconfig
cd /usr/local/share/boost-reflection/example/ioc
LD_LIBRARY_PATH=./ ./test_ioc -xml driver_manager.xml -xml foo.xml -xml bar.xml -lib libfoo.so -lib libbar.so
LD_LIBRARY_PATH=./ ./test_annotation_mvc -xml mvc_dispatcher.xml
```

## API

### register macros

#### basic declaration

```cpp
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

} //namespace test
```

#### declaration with annotation

```cpp
// Annotation definitions
BOOST_REFL_DEF_ANNOTATION(
    (tags)(ControllerTag),
    BOOST_REFL_DEF_ANNOTATION_ELEM(std::list<std::string>, capabilities)
    BOOST_REFL_DEF_ANNOTATION_ELEM_DEFAULT(std::string, desc, "NO DESC")
)

struct ComponenetInfo {
    int id;
    std::string desc;
    ComponenetInfo() {}
    ComponenetInfo(int id, std::string desc) : id(id), desc(desc) {}
};

BOOST_REFL_DEF_ANNOTATION(
    (tags)(ComponentTag),
    BOOST_REFL_DEF_ANNOTATION_ELEM(ComponenetInfo, info)
    BOOST_REFL_DEF_ANNOTATION_ELEM(int, hash)
)

BOOST_REFL_DEF_SIMPLE_ANNOTATION(
    (tags)(DummyTag)
)

namespace foo {
struct MyController {
    void echo(std::string banner) {
        std::cout << "FooController echo: " << banner << std::endl;
    }
    void reset() {
        std::cout << "FooController reset" << std::endl;
    }
};
}

BOOST_REFL_CLASS_WITH_ANNOTATION (
    //class name sequence
    (foo)(MyController),

    //class-level annotation
    BOOST_REFL_ANNOTATION (
        (tags)(ControllerTag),
        (capabilities = make_list<std::string>("CAN_ECHO", "CAN_RESET"))
    )
    //another class-level annotation
    BOOST_REFL_ANNOTATION (
        (tags)(ComponentTag),
        (info = ComponenetInfo(1, "any description"))
        (hash = 23243)
    ),

    //default constructor with annotation
    BOOST_REFL_CONSTRUCTOR_WITH_ANNOTATION (
        (),
        BOOST_REFL_SIMPLE_ANNOTATION (
            (tags)(DummyTag)
        )
    )
    //methods
    BOOST_REFL_METHOD(echo, void(std::string banner))
    BOOST_REFL_METHOD(reset, void())
)
```

### Class

The class `Class` represents the reflected class.

```cpp
class Class {
public:
    //
    //find reflected class via full qualified class name
    //
    static Class forObject(any object);
    static Class forName(const std::string& name);
    static Class forTypeName(const std::string& name);
    static std::list<Class> forNamespace(const std::string& ns);
    static Container forContainerObject(any object);
    static Container forContainerName(const std::string& name);

    //
    //object new/delete shortcuts
    //
    any newInstance(std::vector<any> args);
    any newInstance(any arg1, any arg2, ...);
    template<class T> T newInstance(any arg1, any arg2, ...);
    void deleteInstance(any obj);

    //
    //method invocation shortcuts
    //
    any invoke(any obj, const std::string& methodName, any arg1, ...);
    any invokeStatic(const std::string& methodName, any arg1, ...);
    any invoke(any obj, const std::string& methodName, const std::vector<any>& args);
    any invokeStatic(const std::string& methodName, const std::vector<any>& args);

    //
    //field access shortcuts
    //
    any get(any obj, const std::string& fieldName);
    void set(any obj, const std::string& fieldName, any data);

    //
    //constructor query
    //
    std::list<Constructor> getConstructors();
    Constructor getConstructor(const TypeInfoArray& arg_type_array);
    Constructor getConstructor();
    Destructor getDestructor();

    //
    //method query
    //
    std::list<Method> getMethods();
    Method getMethod(const std::string& methodName, const TypeInfoArray& arg_type_array);
    Method getMethod(const std::string& methodName;
    Method getStaticMethod(const std::string& methodName);
    Method getStaticMethod(const std::string& methodName, const TypeInfoArray& arg_type_array);

    //
    //field query
    //
    std::list<Field> getFields();
    Field getField(const std::string& fieldName);

    //
    //create object proxy
    //
    ProxyBase* createProxy(InvocationHandler* h);
};
```

### Method

The class `Method` represents the reflected method.

```cpp
class Method {
public:
    std::string getName() const;
    const TypeInfoArray& getParameterTypes() const;
    const std::type_info& getResultType() const;
    std::string getSignature() const;
    bool isStatic() const;
    any invoke(any obj, const std::vector<any>& args) const;
    any invokeStatic(std::vector<any> args) const;
    any invoke(any obj, any arg1, ...) const;
    any invokeStatic(any arg1, ...) const;
};
```
