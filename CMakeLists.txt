cmake_minimum_required(VERSION 2.8)

project(final)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.54.0 REQUIRED)

IF(Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

file(GLOB_RECURSE final_SOURCES "final/*")

file(MAKE_DIRECTORY /home/box/final)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/box/final)

set(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} pthread)

add_executable(final ${final_SOURCES})
target_link_libraries(final ${USED_LIBS})