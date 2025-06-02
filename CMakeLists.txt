project(kaleidoscope)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

#Find libraries

file(GLOB_RECURSE HEADERS "src/*.h")

set(INCLUDE_DIRS
	${CMAKE_BINARY_DIR}
	src
)

include_directories(${INCLUDE_DIRS})

add_subdirectory(src/util)
add_subdirectory(src/tests)
add_subdirectory(src/codegen)


set(SOURCES
	src/cpoint.cpp
)

add_executable(cpoint ${SOURCES})
target_include_directories(cpoint PRIVATE ${INCLUDE_DIRS})
target_link_libraries(cpoint PUBLIC util cg tests)

message("Build type is ${CMAKE_BUILD_TYPE}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
	target_compile_options(cpoint PUBLIC -g -O0)
endif()

file(MAKE_DIRECTORY "build")

