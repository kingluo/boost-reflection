# Name of the project, and language
project("boost-reflection")

# CMake version
cmake_minimum_required(VERSION 2.8.8)

# import modules
include(FindThreads)

# Version of the source code
set(BOOST_REFLECTION_VERSION_MAJOR 1 CACHE PATH "BOOST_REFLECTION MAJOR VERSION")
set(BOOST_REFLECTION_VERSION_MINOR 0 CACHE PATH "BOOST_REFLECTION MINOR VERSION")
set(BOOST_REFLECTION_VERSION_REV 0 CACHE PATH "BOOST_REFLECTION REVISION")

# Version of the API with the library
set(BOOST_REFLECTION_VERSION_API 1)

enable_testing()

find_package(Boost 1.36.0)
find_package(Threads)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "boost not found!")
endif()

# install paths
set(INSTALL_HEADERS_SUFFIX include)
set(INSTALL_BIN_SUFFIX bin)
set(INSTALL_LIBRARY_SUFFIX lib)

if(${UNIX})
    # common flags
    add_definitions(-pthread)

    # compile flags for release mode
    set(my_ssp_cflags "-fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2")
    set(CMAKE_C_FLAGS_RELEASE "-march=i686 -mtune=generic -O2 -pipe ${my_ssp_cflags} -Wall -g -DNDEBUG -DBOOST_DISABLE_ASSERTS")
    mark_as_advanced(CMAKE_C_FLAGS_RELEASE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    mark_as_advanced(CMAKE_CXX_FLAGS_RELEASE)

    # compile flags for debug mode
    set(CMAKE_C_FLAGS_DEBUG "-march=i686 -mtune=generic -O0 -pipe -Wall -g")
    mark_as_advanced(CMAKE_C_FLAGS_DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    mark_as_advanced(CMAKE_CXX_FLAGS_DEBUG)

    # link flags
    set(my_lflags "-Wl,-O1,--sort-common,--as-needed,-z,relro")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${my_lflags}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${my_lflags}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${my_lflags}")
else()
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(/MD)
    # compile flags for release mode
    set(CMAKE_C_FLAGS_RELEASE "/O2 -DNDEBUG -DBOOST_DISABLE_ASSERTS")
    mark_as_advanced(CMAKE_C_FLAGS_RELEASE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    mark_as_advanced(CMAKE_CXX_FLAGS_RELEASE)
endif()

# common include paths
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Location for the source code
add_subdirectory(libs/reflection/src)
add_subdirectory(libs/reflection/test)
add_subdirectory(libs/reflection/example)

# packaging
set(CPACK_PACKAGE_VERSION_MAJOR ${BOOST_REFLECTION_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${BOOST_REFLECTION_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${BOOST_REFLECTION_VERSION_REV})
set(CPACK_PACKAGE_INSTALL_DIRECTORY boost-reflection)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE_1_0.txt)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
include(CPack)

# install headers
install(DIRECTORY boost DESTINATION ${INSTALL_HEADERS_SUFFIX})
