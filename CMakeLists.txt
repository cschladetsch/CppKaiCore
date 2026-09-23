cmake_minimum_required(VERSION 3.28...3.30)

set(CORE_SRC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/Source/Library/Core/Source")
set(EXEC_SRC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/Source/Library/Executor/Source")
set(LANG_SRC_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/Source/Library/Language/Common/Source")
set(INC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/Include")

file(GLOB CORE_SRCS
    "${CORE_SRC_DIR}/*.cpp"
    "${CORE_SRC_DIR}/BuiltinTypes/*.cpp"
)
add_library(Core STATIC ${CORE_SRCS})
target_include_directories(Core PUBLIC
    "${INC_DIR}"
    "${INC_DIR}/KAI/Core"
    "C:/local/boost_1_91_0"
)
set_target_properties(Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${KAI_HOME}/Bin"
)

file(GLOB COMMONLANG_SRCS "${LANG_SRC_DIR}/*.cpp")
add_library(CommonLang STATIC ${COMMONLANG_SRCS})
target_include_directories(CommonLang PUBLIC
    "${INC_DIR}"
    "${INC_DIR}/KAI/Core"
)
target_link_libraries(CommonLang PUBLIC Core)
set_target_properties(CommonLang PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${KAI_HOME}/Bin"
)

file(GLOB EXECUTOR_SRCS
    "${EXEC_SRC_DIR}/*.cpp"
    "${EXEC_SRC_DIR}/Translator/*.cpp"
)
list(REMOVE_ITEM EXECUTOR_SRCS "${EXEC_SRC_DIR}/Console.cpp")
add_library(Executor STATIC ${EXECUTOR_SRCS})
target_include_directories(Executor PUBLIC
    "`${KAI_HOME}/Ext/rang/include"
    "${INC_DIR}"
    "${INC_DIR}/KAI/Executor"
    "${INC_DIR}/KAI/Core"
)
target_link_libraries(Executor PUBLIC Core CommonLang)
set_target_properties(Executor PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${KAI_HOME}/Bin"
)


