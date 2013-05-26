# Name of the subproject
project("reflection_library")

add_library(libreflection SHARED class.cpp)

# Avoid the liblib name, and set the version
set_target_properties(libreflection PROPERTIES 
    OUTPUT_NAME boost_reflection
    SOVERSION ${BOOST_REFLECTION_VERSION_API}
    VERSION ${BOOST_REFLECTION_VERSION_MAJOR}.${BOOST_REFLECTION_VERSION_MINOR}.${BOOST_REFLECTION_VERSION_REV})

if(${UNIX})
    install(TARGETS libreflection DESTINATION ${INSTALL_LIBRARY_SUFFIX})
else()
    install(TARGETS libreflection DESTINATION ${INSTALL_BIN_SUFFIX})
endif()
