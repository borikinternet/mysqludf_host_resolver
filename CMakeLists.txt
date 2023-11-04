CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
project(mysqludf_host_resolver VERSION 0.0.1.0)

SET(SOURCES src/lib_mysqludf_host_resolver.c src/mysqludf.c)

configure_file(include/config.h.in config.h)

add_library(mysqludf_host_resolver SHARED
        ${SOURCES})
set_target_properties(mysqludf_host_resolver
        PROPERTIES PREFIX "lib_")
target_compile_definitions(mysqludf_host_resolver
        PRIVATE STANDARD MYSQL_SERVER)
target_include_directories(mysqludf_host_resolver PRIVATE
        /usr/include/mysql include ${CMAKE_CURRENT_BINARY_DIR})