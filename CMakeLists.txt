cmake_minimum_required(VERSION 3.30.2)

project(stack_size_summarizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

file(GLOB SRC_FILES "src/*.cpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
add_executable(stack_size_summarizer ${SRC_FILES})

add_library(lib_proc_ps SHARED IMPORTED)
set_property(
    TARGET lib_proc_ps PROPERTY IMPORTED_LOCATION
    "/usr/lib/x86_64-linux-gnu/libprocps.so"
)
target_link_libraries(stack_size_summarizer lib_proc_ps stdc++fs)
