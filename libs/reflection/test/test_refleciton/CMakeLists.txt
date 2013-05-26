# basic reflection function test

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(${UNIX})
    set(EXTRA_LINK_LIBS rt dl)
endif()

add_executable(main main.cpp)
target_link_libraries(main libreflection)
add_test(NAME main COMMAND $<TARGET_FILE:main>)
set_tests_properties(main PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")

add_library(sample_classes_static OBJECT libtest.cpp)
add_executable(main_use_static_lib main_use_lib.cpp $<TARGET_OBJECTS:sample_classes_static>)
target_link_libraries(main_use_static_lib libreflection)
add_test(NAME main_use_static_lib COMMAND $<TARGET_FILE:main_use_static_lib>)
set_tests_properties(main_use_static_lib PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")

add_library(sample_classes_shared SHARED libtest.cpp)
target_link_libraries(sample_classes_shared libreflection)

add_executable(main_use_shared_lib main_use_lib.cpp)
target_link_libraries(main_use_shared_lib libreflection sample_classes_shared)
add_test(NAME main_use_shared_lib COMMAND $<TARGET_FILE:main_use_shared_lib>)
set_tests_properties(main_use_shared_lib PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")

add_executable(use_dl main_use_dl.cpp)
target_link_libraries(use_dl libreflection ${EXTRA_LINK_LIBS})
add_test(NAME use_dl COMMAND $<TARGET_FILE:use_dl> $<TARGET_FILE:sample_classes_shared>)
set_tests_properties(use_dl PROPERTIES ENVIRONMENT "${LD_VARNAME}=${LD_PATH}")
