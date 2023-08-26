cmake_minimum_required(VERSION 3.6)

project(
    CppUtils
    VERSION 1.0
    LANGUAGES C CXX
)

file(GLOB_RECURSE CppUtilsC_SRC
    "single_include/*.h"
    "single_include/*.hpp"
    "single_include/*.c"
)

file(GLOB_RECURSE CppUtilsCpp_SRC
    "single_include/*.h"
    "single_include/*.hpp"
    "single_include/*.cpp"
)

add_executable(CppUtilsTestC ${CppUtilsC_SRC})
add_executable(CppUtilsTestCpp ${CppUtilsCpp_SRC})

set_target_properties(
    CppUtilsTestC PROPERTIES
    CMAKE_C_STANDARD 11
    CMAKE_C_STANDARD_REQUIRED True
)

set_target_properties(
    CppUtilsTestCpp PROPERTIES
    CMAKE_CXX_STANDARD 17
    CMAKE_CXX_STANDARD_REQUIRED True
    LINKER_LANGUAGE CXX 
)

# Definitions
target_compile_definitions(
    CppUtilsTestC PUBLIC
    -D_DEBUG
    -DGABE_CPP_UTILS_TEST_MAIN
)

target_compile_definitions(
    CppUtilsTestCpp PUBLIC
    -D_DEBUG
    -DGABE_CPP_UTILS_TEST_MAIN
)

# Set output directories
set_target_properties(
    CppUtilsTestC PROPERTIES
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CMAKE_BUILD_TYPE Debug
    # Static runtime
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

set_target_properties(
    CppUtilsTestCpp PROPERTIES
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CMAKE_BUILD_TYPE Debug
    # Static runtime
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# Set include directory
target_include_directories(CppUtilsTestC PUBLIC "single_include")
target_include_directories(CppUtilsTestCpp PUBLIC "single_include")

# Enable warnings as errors
if(MSVC)
  target_compile_options(CppUtilsTestC PRIVATE /W4 /WX)
  target_compile_options(CppUtilsTestCpp PRIVATE /W4 /WX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  target_compile_options(CppUtilsTestC PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_options(CppUtilsTestCpp PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
    PROPERTY VS_STARTUP_PROJECT CppUtilsTestC
)

add_custom_command(TARGET CppUtilsTestC 
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/logs)
add_custom_command(TARGET CppUtilsTestCpp 
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/logs)
