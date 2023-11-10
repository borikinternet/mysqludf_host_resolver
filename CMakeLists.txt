CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
project(mysqludf_host_resolver VERSION 0.0.1.0)

SET(SOURCES src/lib_mysqludf_host_resolver.c)

configure_file(include/config.h.in config.h)

add_library(mysqludf_host_resolver SHARED ${SOURCES})
set_target_properties(mysqludf_host_resolver PROPERTIES PREFIX "lib_")
target_compile_definitions(mysqludf_host_resolver PRIVATE STANDARD MYSQL_SERVER)
target_include_directories(mysqludf_host_resolver PRIVATE /usr/include/mysql include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mysqludf_host_resolver pthread)

add_library(udf_example SHARED src/udf_example.c)
set_target_properties(udf_example PROPERTIES PREFIX "lib_")
target_compile_definitions(udf_example PRIVATE STANDARD HAVE_DLOPEN)
target_include_directories(udf_example PRIVATE /usr/include/mysql include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(udf_example pthread)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK check)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

if (${CHECK_FOUND})
    add_executable(mysqludf_host_resolver_test ${SOURCES})
    target_compile_definitions(mysqludf_host_resolver_test PRIVATE STANDARD MYSQL_SERVER TESTING)
    target_include_directories(mysqludf_host_resolver_test PRIVATE /usr/include/mysql include ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(mysqludf_host_resolver_test PRIVATE ${CHECK_LIBRARIES})

    enable_testing()

    add_test(NAME udf_test COMMAND mysqludf_host_resolver_test)
endif ()