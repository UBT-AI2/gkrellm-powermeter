cmake_minimum_required(VERSION 3.10)
project(powermeter_library)

#SET(CMAKE_CXX_FLAGS "-O0")
#SET(CMAKE_C_FLAGS "-O0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set_property(GLOBAL PROPERTY CMAKE_POSITION_INDEPENDENT_CODE True)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(powermeter_library STATIC powermeter_library.cpp powermeter_library.h rapl_reader.cpp rapl_reader.h powerlogger.cpp powerlogger.hpp)
#add_library(powermeter_library_shared SHARED powermeter_library.cpp powermeter_library.h rapl_reader.cpp rapl_reader.h)
#set_target_properties(powermeter_library_shared PROPERTIES OUTPUT_NAME powermeter_library)
