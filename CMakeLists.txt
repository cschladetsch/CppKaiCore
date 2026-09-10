cmake_minimum_required(VERSION 3.20)

# CppKaiCore - the foundational KAI runtime.
#
# Bundles the mutually-dependent base of the KAI system:
#   - Core      (object model, registry, types, memory, containers)
#   - Executor  (continuation-based virtual machine)
#   - CommonLang(shared lexer/parser/translator base for the languages)
#
# These three form a dependency cycle (Core <-> Executor <-> CommonLang) and so
# live together in one reusable module. Extracted from CppKAI for reuse on
# other platforms (including Android). Builds standalone or via add_subdirectory.

if(NOT DEFINED PROJECT_NAME)
    project(CppKaiCore CXX)
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CPPKAICORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CPPKAICORE_INCLUDE ${CPPKAICORE_ROOT}/Include)

# CommonLang's lexer can optionally use std::pmr arena allocation; enable it
# with -DKAI_USE_MONOTONIC_ALLOCATOR. There are no third-party dependencies.

# When built standalone (not as part of CppKAI), establish the compile
# definitions the sources expect. Inside CppKAI these are set by the parent.
if(NOT DEFINED KAI_HOME)
    add_definitions(
        -DKAI_USE_NAMESPACES
        -DKAI_NAMESPACE_NAME=kai
        -DKAI_USE_EXCEPTIONS
        -DKAI_BOOST_UNORDERED_REGISTRY
        -DKAI_VERSION_MAJOR=0
        -DKAI_VERSION_MINOR=3
        -DKAI_VERSION_PATCH=0)
    if(NOT DEFINED KAI_LOG_DIR_DEFINED)
        add_definitions(-DKAI_LOG_DIR="./Logs")
    endif()
endif()

# MSVC 2026 compatibility - force-include missing transitive headers
if(MSVC)
    set(KAI_COMPAT "${CPPKAICORE_INCLUDE}/KAI/Core/msvc_compat.h")
    add_compile_options(/FI${KAI_COMPAT})
endif()

# --- Core -------------------------------------------------------------------
file(GLOB_RECURSE cppkaicore_core_src
    ${CPPKAICORE_ROOT}/Source/Library/Core/Source/*.cpp)
add_library(Core ${cppkaicore_core_src})
target_include_directories(Core PUBLIC
    ${CPPKAICORE_INCLUDE}
    ${CPPKAICORE_INCLUDE}/KAI/Core)

# --- CommonLang -------------------------------------------------------------
file(GLOB_RECURSE cppkaicore_common_src
    ${CPPKAICORE_ROOT}/Source/Library/Language/Common/Source/*.cpp)
add_library(CommonLang ${cppkaicore_common_src})
target_include_directories(CommonLang PUBLIC
    ${CPPKAICORE_INCLUDE}
    ${CPPKAICORE_INCLUDE}/KAI/Language/Common)
target_link_libraries(CommonLang PUBLIC Core)

# --- Executor ---------------------------------------------------------------
# Console.cpp is excluded: it is parent-level glue that pulls in the Pi/Rho
# translators (CppKaiLanguage), which would invert the layering. The consuming
# project compiles it separately if it needs the interactive console.
file(GLOB_RECURSE cppkaicore_exec_src
    ${CPPKAICORE_ROOT}/Source/Library/Executor/Source/*.cpp)
list(FILTER cppkaicore_exec_src EXCLUDE REGEX ".*Console\\.cpp$")
add_library(Executor ${cppkaicore_exec_src})
target_include_directories(Executor PUBLIC
    ${CPPKAICORE_INCLUDE}
    ${CPPKAICORE_INCLUDE}/KAI/Executor)
target_link_libraries(Executor PUBLIC CommonLang Core)

# Honour a parent-provided output directory (CppKAI's BIN_HOME).
if(DEFINED BIN_HOME)
    set_target_properties(Core CommonLang Executor PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${BIN_HOME}"
        LIBRARY_OUTPUT_DIRECTORY "${BIN_HOME}")
endif()
