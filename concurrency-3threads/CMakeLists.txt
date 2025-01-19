cmake_minimum_required(VERSION 3.2)
project(Rainbow-concurrency-3threads)
set(CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif(CMAKE_COMPILER_IS_GNUCXX)

option(ACTION "Allow Action or Phi Function" OFF)
if(ACTION)
	add_definitions("-DACTION")
	message(STATUS "Allow Action or Phi Function")
else()
	message(STATUS "Not Allow Action or Phi Function")
endif()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${SRC_DIR}/*.h")

add_executable(Rainbow-concurrency-3threads ${SOURCES} ${HEADERS}
		src/concurrency.cpp
		src/concurrency.cpp)
