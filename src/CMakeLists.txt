
cmake_minimum_required(VERSION 2.8)

#==================================================================#
#               The Definition for common constants                #
#==================================================================#
# Define the color code for error message.
if (NOT WIN32)
    string(ASCII 27 Esc)
    set(RESET "${Esc}[m")
    set(RED "${Esc}[31m")
endif()

# Define the constants to parse command options.
set(OPT_BUILD_DBG "Debug")
set(OPT_BUILD_REL "Release")
set(OPT_TARGET_ENG "Engine")
set(OPT_TARGET_PLG "Plugin")

# Define the constants for path generation.
set(NAME_INC "include")
set(NAME_BIN "bin")
set(PATH_INC "${CMAKE_CURRENT_SOURCE_DIR}/../${NAME_INC}")
set(PATH_BIN "${CMAKE_CURRENT_SOURCE_DIR}/../${NAME_BIN}")
set(PATH_PLG "${CMAKE_CURRENT_SOURCE_DIR}/plugin")
set(PATH_BIN_ENG "${PATH_BIN}/engine")
set(PATH_BIN_PLG "${PATH_BIN}/plugin")

# For plugins, we build them as dynamically loadable module.
set(LIB_TYPE "MODULE")


#==================================================================#
#                The subroutines for specific task                 #
#==================================================================#
function(SUB_BUILD_ENGINE)
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

    # Determine the build type.
    if (CMAKE_BUILD_TYPE STREQUAL OPT_BUILD_DBG)
        set(PATH_OUT "${PATH_BIN_ENG}/debug")
        set(PATH_PLG_SEARCH "${PATH_BIN_PLG}/debug")
    elseif (CMAKE_BUILD_TYPE STREQUAL OPT_BUILD_REL)
        set(PATH_OUT "${PATH_BIN_ENG}/release")
        set(PATH_PLG_SEARCH "${PATH_BIN_PLG}/release")
    else()
        message("${RED}-- Error: CMAKE_BUILD_TYPE is not properly specified.${RESET}")
        return()
    endif()

    # Set the default plugin search path.
    set(DT_RUNPATH "-Wl,-rpath,${PATH_PLG_SEARCH},--enable-new-dtags")

    # Build the engine executable.
    add_executable(${TGE_PENGRAM}
        ${SRC_MAIN} ${SRC_PE} ${SRC_RGN} ${SRC_NGRAM} ${SRC_RPT} ${SRC_UTIL} ${SRC_EXPT}
    )
    target_link_libraries(${TGE_PENGRAM}
        ${IMPORT_CONFIG} ${IMPORT_DL} ${IMPORT_MATH}
    )

    set_target_properties( ${TGE_PENGRAM} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PATH_OUT}
        OUTPUT_NAME ${OUT_PENGRAM}
        LINK_FLAGS ${DT_RUNPATH}
    )
endfunction()

function(SUB_BUILD_PLUGIN)
    set(FORBID_REG "Region_Template")
    set(FORBID_MDL "Model_Template")
    set(MATCH_REG "Region_*.c")
    set(MATCH_MDL "Model_*.c")
    set(GROUP_REG "Region_[0-9a-zA-Z]+")
    set(GROUP_MDL "Model_[0-9a-zA-Z]+")

    # Determine the build type.
    if (CMAKE_BUILD_TYPE STREQUAL OPT_BUILD_DBG)
        set(PATH_OUT "${PATH_BIN_PLG}/debug")
    elseif (CMAKE_BUILD_TYPE STREQUAL OPT_BUILD_REL)
        set(PATH_OUT "${PATH_BIN_PLG}/release")
    else()
        message("${RED}-- Error: CMAKE_BUILD_TYPE is not properly specified.${RESET}")
        return()
    endif()

    # Build the libaries for region selection.
    file(GLOB_RECURSE LIST_SRC_REG ${MATCH_REG})
    foreach(SRC_REG ${LIST_SRC_REG})
        string(REGEX MATCH ${GROUP_REG} NAME_REG ${SRC_REG})
        if (NOT NAME_REG STREQUAL FORBID_REG)
            string(TOUPPER ${NAME_REG} TGE_REG)
            add_library(${TGE_REG} ${LIB_TYPE} ${SRC_REG})
            set_target_properties(${TGE_REG} PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${PATH_OUT}
                OUTPUT_NAME ${NAME_REG}
            )
        endif()
    endforeach()

    # Build the libraries for n-gram model generation.
    file(GLOB_RECURSE LIST_SRC_MDL ${MATCH_MDL})
    foreach(SRC_MDL ${LIST_SRC_MDL})
        string(REGEX MATCH ${GROUP_MDL} NAME_MDL ${SRC_MDL})
        if (NOT NAME_MDL STREQUAL FORBID_MDL)
            string(TOUPPER ${NAME_MDL} TGE_MDL)
            add_library(${TGE_MDL} ${LIB_TYPE} ${SRC_MDL})
            set_target_properties(${TGE_MDL} PROPERTIES
                LIBRARY_OUTPUT_DIRECTORY ${PATH_OUT}
                OUTPUT_NAME ${NAME_MDL}
            )
        endif()
    endforeach()

    set(REGEX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
endfunction()


#==================================================================#
#                    The CMakeLists entry point                    #
#==================================================================#
include_directories(${PATH_INC})

if (BUILD_TARGET STREQUAL OPT_TARGET_ENG)
    SUB_BUILD_ENGINE()
elseif (BUILD_TARGET STREQUAL OPT_TARGET_PLG)
    SUB_BUILD_PLUGIN()
else()
    SUB_BUILD_ENGINE()
    SUB_BUILD_PLUGIN()
endif()