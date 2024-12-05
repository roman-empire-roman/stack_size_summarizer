cmake_minimum_required(VERSION 3.30.2)
project(stack_size_summarizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -v")

set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB SRC_FILES "src/*.cpp")

find_package(Boost REQUIRED)
find_library(procps_LIBRARY NAMES procps REQUIRED)
find_path(procps_INCLUDE_DIR NAMES proc/readproc.h REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost was NOT found")
endif()

if (NOT DEFINED procps_LIBRARY OR "${procps_LIBRARY}" STREQUAL "")
    message(FATAL_ERROR "procps_LIBRARY was NOT found")
endif()

set(Boost_USE_MULTITHREADED ON)

add_executable(stack_size_summarizer ${SRC_FILES})
target_compile_features(stack_size_summarizer PRIVATE cxx_std_20)
target_compile_options(stack_size_summarizer PRIVATE -Wall -Wextra -v)
set_target_properties(stack_size_summarizer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if (NOT DEFINED Boost_INCLUDE_DIRS OR "${Boost_INCLUDE_DIRS}" STREQUAL "")
    message(FATAL_ERROR "Boost_INCLUDE_DIRS was NOT set")
endif()

if (NOT DEFINED procps_INCLUDE_DIR OR "${procps_INCLUDE_DIR}" STREQUAL "")
    message(FATAL_ERROR "procps_INCLUDE_DIR was NOT set")
endif()

target_include_directories(
    stack_size_summarizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${Boost_INCLUDE_DIRS}
    ${procps_INCLUDE_DIR}
)

target_link_libraries(
    stack_size_summarizer PRIVATE
    ${procps_LIBRARY}
)
