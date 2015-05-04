
cmake_minimum_required(VERSION 2.8)


# Define the color code for error message.
if (NOT WIN32)
    string(ASCII 27 Esc)
    set(RESET "${Esc}[m")
    set(RED "${Esc}[31m")
endif()


# Define the constants to parse command options.
set(OPT_BUILD_DEBUG "Debug")
set(OPT_BUILD_RELEASE "Release")


# Define the constants for path generation.
set(NAME_INC "include")
set(NAME_BIN "bin")
set(PATH_INC "${CMAKE_CURRENT_SOURCE_DIR}/../${NAME_INC}")
set(PATH_BIN "${CMAKE_CURRENT_SOURCE_DIR}/../${NAME_BIN}")
set(PATH_ENGINE "${PATH_BIN}/engine")
#set(PATH_INC_PLGIN "${PROJECT_SOURCE_DIR}/../plugin/${NAME_INC}")
set(SRC_MAIN "main.c")
set(SRC_NGRAM "ngram.c")
set(SRC_PE "pe_info.c")
set(SRC_RGN "region.c")
set(SRC_RPT "report.c")
set(SRC_UTIL "util.c")
set(SRC_EXPT "except.c")
set(TGE_PENGRAM "PENGRAM")
set(OUT_PENGRAM "pe_ngram")
set(IMPORT_CONFIG "-lconfig")
set(IMPORT_DL "-ldl")
set(IMPORT_MATH "-lm")


# Parse the command options.
if (CMAKE_BUILD_TYPE STREQUAL OPT_BUILD_DEBUG)
    set(PATH_OUT "${PATH_ENGINE}/debug")
elseif (CMAKE_BUILD_TYPE STREQUAL OPT_BUILD_RELEASE)
    set(PATH_OUT "${PATH_ENGINE}/release")
else()
    message("${RED}-- Error: CMAKE_BUILD_TYPE is not properly specified.${RESET}")
    return()
endif()


# List the source building rules.
include_directories(${PATH_INC} ${PATH_PLGIN})

add_executable(${TGE_PENGRAM}
    ${SRC_MAIN} ${SRC_PE} ${SRC_RGN} ${SRC_NGRAM} ${SRC_RPT} ${SRC_UTIL} ${SRC_EXPT}
)
target_link_libraries(${TGE_PENGRAM}
    ${IMPORT_CONFIG} ${IMPORT_DL} ${IMPORT_MATH})

set_target_properties( ${TGE_PENGRAM} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PATH_OUT}
    OUTPUT_NAME ${OUT_PENGRAM}
)

message("${PROJECT_SOURCE_DIR}")

