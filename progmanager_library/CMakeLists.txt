cmake_minimum_required(VERSION 3.10)
project(progmanager_library)

#SET(CMAKE_CXX_FLAGS "-O0")
#SET(CMAKE_C_FLAGS "-O0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set_property(GLOBAL PROPERTY CMAKE_POSITION_INDEPENDENT_CODE True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(progmanager_library STATIC progmanager_library.cpp progmanager_library.hpp programstats.cpp programstats.hpp logger.cpp logger.hpp progmanager_library_c.h progmanager_library_c.cpp)
