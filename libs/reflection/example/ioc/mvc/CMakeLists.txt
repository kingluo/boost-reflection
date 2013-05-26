include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ioc)
link_directories(${CURRENT_BINARY_DIR}/../libreflection)
link_directories(${CURRENT_BINARY_DIR}/../libioc)

if(${UNIX})
    set(EXTRA_LINK_LIBS dl)
endif()
add_executable(test_annotation_mvc
    test_annotation_mvc.cpp
    mvc_controller_foo.cpp
    mvc_default_dispatcher.cpp
    mvc_common.hpp
    mvc_dispatcher_interface.hpp
)
target_link_libraries(test_annotation_mvc libreflection libioc ${EXTRA_LINK_LIBS})
install(TARGETS test_annotation_mvc DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc COMPONENT example_ioc)

install(FILES mvc_dispatcher.xml DESTINATION ${EXAMPLE_INSTALL_PREFIX}example/ioc)
