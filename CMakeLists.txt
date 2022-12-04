cmake_minimum_required(VERSION 3.10)

##nclude(ExternalProject)
#ExternalProject_Add(powermeter_library SOURCE_DIR
#        PREFIX /home/nano/CLionProjects/bachelorarbeit/powermeter_library)

add_subdirectory(powermeter_library)
add_subdirectory(progmanager_library)

project(gkrellm-powermeter C)
set(CMAKE_C_STANDARD 11)

#SET(CMAKE_CXX_FLAGS "-O0")
#SET(CMAKE_C_FLAGS "-O0")

find_package(PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
include_directories (${GTK2_INCLUDE_DIRS})
link_directories (${GTK2_LIBRARY_DIRS})
add_definitions (${GTK2_CFLAGS_OTHER})

add_library(gkrellm-powermeter SHARED gkrellm-powermeter.c gkrellm-powermeter.h)


## IMPORT powermeter_library
#link_directories(/home/nano/CLionProjects/bachelorarbeit/powermeter_library/cmake-build-debug)
#add_library(powermeter_library STATIC IMPORTED)
#include_directories(/home/nano/CLionProjects/bachelorarbeit/powermeter_library/cmake-build-debug
#        /home/nano/CLionProjects/bachelorarbeit/powermeter_library)
#set_target_properties(powermeter_library PROPERTIES
#        IMPORTED_LOCATION /home/nano/CLionProjects/bachelorarbeit/powermeter_library/cmake-build-debug/libpowermeter_library.a
#        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX")

## IMPORT progmanager_library
#link_directories(/home/nano/CLionProjects/bachelorarbeit/progmanager_library/cmake-build-debug)
#add_library(progmanager_library STATIC IMPORTED)
#include_directories(/home/nano/CLionProjects/bachelorarbeit/progmanager_library/cmake-build-debug
#        /home/nano/CLionProjects/bachelorarbeit/progmanager_library)
#set_target_properties(progmanager_library PROPERTIES
#        IMPORTED_LOCATION /home/nano/CLionProjects/bachelorarbeit/progmanager_library/cmake-build-debug/libprogmanager_library.a
#        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX")


target_link_libraries (gkrellm-powermeter ${GTK2_LIBRARIES} powermeter_library  progmanager_library stdc++)
