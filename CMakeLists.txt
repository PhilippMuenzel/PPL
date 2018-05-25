cmake_minimum_required (VERSION 3.8)
project(PPL VERSION 2.0.0 DESCRIPTION "Plugin Patterns Library for X-Plane 11")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

if(UNIX AND NOT APPLE)
set(LINUX TRUE)
endif()

if(APPLE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)
set(CMAKE_OSX_SYSROOT "macosx")
set(PLATFORM_CORE_DEFINITIONS
    APL=1
    LIN=0
    IBM=0)
else()
if(LINUX)
endif()
endif()

set(DEFINITIONS
    XPLM200=1
    XPLM210=1
    XPLM300=1
    XPLM301=1
    ${PLATFORM_CORE_DEFINITIONS})

include_directories(/usr/X11/include/freetype2
${CMAKE_SOURCE_DIR}/../SDK/CHeaders/XPLM
${CMAKE_SOURCE_DIR}/../SDK/CHeaders/Widgets
${CMAKE_SOURCE_DIR}/include/simpleini
)

file(GLOB SOURCES "src/*.cpp")


add_library(PPL STATIC ${SOURCES})
target_compile_definitions(PPL PRIVATE ${DEFINITIONS})
