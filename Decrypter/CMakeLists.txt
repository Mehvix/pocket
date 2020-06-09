cmake_minimum_required(VERSION 3.16)
project(Pocket)

set(CMAKE_CXX_STANDARD 14)

# Doesn't show CMD
#[[SET(GCC_COVERAGE_LINK_FLAGS "-mwindows")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")]]

add_executable(PocketDecrypt main.cpp)