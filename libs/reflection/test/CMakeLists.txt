# Name of the subproject
project("test")

if (WIN32)
add_definitions(/EHa)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../libreflection)

# In order to run tests we will need to set the approriate environment
# variable so that the test program can locate its dependent DLL's. First
# we want to know what directory our dependent DLL was installed into:

get_target_property(libdir libreflection LOCATION_Release)
get_filename_component(libdir "${libdir}" PATH)
file(TO_NATIVE_PATH "${libdir}" native_libdir)

# Determine which environment variable controls the locating of
# DLL's and set that variable.

if (WIN32)
   set (LD_VARNAME "PATH")
   set (LD_PATH "${native_libdir};$ENV{PATH}")

   # IMPORTANT NOTE: The set_tests_properties(), below, internally
   # stores its name/value pairs with a semicolon delimiter.
   # because of this we must protect the semicolons in the path

   string(REPLACE ";" "\\;" LD_PATH "${LD_PATH}")
else ()
   set (LD_VARNAME "LD_LIBRARY_PATH")
   set (LD_PATH "${native_libdir}:$ENV{LD_LIBRARY_PATH}")
endif ()

# test cases
add_subdirectory(test_refleciton)

add_executable(test_annotation test_annotation.cpp)
target_link_libraries(test_annotation libreflection)
add_test(NAME test_annotation COMMAND test_annotation)
set_tests_properties(test_annotation PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")

add_executable(test_container test_container.cpp)
target_link_libraries(test_container libreflection)
add_test(NAME test_container COMMAND test_container)
set_tests_properties(test_container PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")

add_executable(test_proxy test_proxy.cpp)
target_link_libraries(test_proxy libreflection)
add_test(NAME test_proxy COMMAND test_proxy)
set_tests_properties(test_proxy PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")
