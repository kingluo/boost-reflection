# Name of the subproject
project("example_ioc")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CURRENT_BINARY_DIR}/../libreflection)

if(${UNIX})
    set(EXAMPLE_INSTALL_PREFIX "share/boost-reflection/")
endif()

add_library(libioc SHARED ioc.cpp ioc.hpp)
set_target_properties(libioc PROPERTIES OUTPUT_NAME ioc)
target_link_libraries(libioc libreflection)
install(TARGETS libioc DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc COMPONENT example_ioc)

add_library(libfoo SHARED foo_driver.cpp)
set_target_properties(libfoo PROPERTIES OUTPUT_NAME foo)
target_link_libraries(libfoo libreflection)
install(TARGETS libfoo DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc COMPONENT example_ioc)

add_library(libbar SHARED bar_driver.cpp)
set_target_properties(libbar PROPERTIES OUTPUT_NAME bar)
target_link_libraries(libbar libreflection)
install(TARGETS libbar DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc COMPONENT example_ioc)

if(${UNIX})
    set(EXTRA_LINK_LIBS dl)
endif()
add_executable(test_ioc test_ioc.cpp)
target_link_libraries(test_ioc libreflection libioc ${EXTRA_LINK_LIBS})
install(TARGETS test_ioc DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc COMPONENT example_ioc)

install(FILES bar.xml foo.xml driver_manager.xml DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc)

add_subdirectory(mvc)
