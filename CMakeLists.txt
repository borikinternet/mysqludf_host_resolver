CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
project(mysqludf_host_resolver VERSION 0.0.1.0)

SET(SOURCES src/lib_mysqludf_host_resolver.c)

configure_file(include/config.h.in config.h)

add_library(mysqludf_host_resolver SHARED
        ${SOURCES})
set_target_properties(mysqludf_host_resolver
        PROPERTIES PREFIX "lib_")
target_compile_definitions(mysqludf_host_resolver
        PRIVATE STANDARD MYSQL_SERVER)
target_include_directories(mysqludf_host_resolver PRIVATE
        /usr/include/mysql include ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

add_executable(mysqludf_host_resolver_test ${SOURCES})
target_compile_definitions(mysqludf_host_resolver_test PRIVATE
        STANDARD MYSQL_SERVER TESTING)
target_include_directories(mysqludf_host_resolver_test PRIVATE
        /usr/include/mysql include ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mysqludf_host_resolver_test PRIVATE
        ${CHECK_LIBRARIES})

enable_testing()

add_test(NAME udf_test COMMAND mysqludf_host_resolver_test)