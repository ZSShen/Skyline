cmake_minimum_required(VERSION 2.8)
set(CMAKE_INSTALL_PREFIX "")
project(ENGINE)

# If the build type is not specified, we apply release build by default.
set(BUILD_TYPE_REL "Release")
set(MSG_BUILD_TYPE "Default Release Build")
if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE ${BUILD_TYPE_REL} CACHE STRING ${MSG_BUILD_TYPE} FORCE)
endif()

set(DIR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_subdirectory(${DIR_SRC})