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

# Set the "make run" target.
set(TARGET_RUN "run")
set(DIR_TEST "${CMAKE_CURRENT_SOURCE_DIR}/test")
set(SCRIPT "./test.py")
set(PATH_ENGINE "${CMAKE_CURRENT_SOURCE_DIR}/bin/engine/release/pe_ngram")
set(PATH_TAR "${DIR_TEST}/case.tar.gz")
set(PATH_CASE "${DIR_TEST}/case")
add_custom_target ( ${TARGET_RUN}
    COMMAND ${SCRIPT} ${PATH_ENGINE} ${PATH_TAR} ${PATH_CASE}
    WORKING_DIRECTORY ${DIR_TEST}
)
message(${DIR_TEST})