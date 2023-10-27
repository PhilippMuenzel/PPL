cmake_minimum_required (VERSION 3.0)
project(PPL VERSION 2.0.0 DESCRIPTION "Plugin Patterns Library for X-Plane 11/12")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

if(UNIX AND NOT APPLE)
set(LINUX TRUE)
endif()

if(UNIX)
    set(PLATFORM_COMPILE_OPTIONS -Wall -Wextra -Wfloat-equal -pedantic)
else()
    set(PLATFORM_COMPILE_OPTIONS /W3 /wd4996)
endif()

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
    set(CMAKE_OSX_ARCHITECTURES $(ARCHS_STANDARD))
    set(PLATFORM_CORE_DEFINITIONS
        APL=1
        LIN=0
        IBM=0)
    set(PLATFORM_INCLUDE_DIRECTORIES
        include/freetype2/include)
else()
if(LINUX)
    set(PLATFORM_CORE_DEFINITIONS
        APL=0
        LIN=1
        IBM=0)
    set(PLATFORM_INCLUDE_DIRECTORIES
        /usr/include/freetype2)
else()
    set(PLATFORM_CORE_DEFINITIONS
        APL=0
        LIN=0
        IBM=1
        FREETYPE2_STATIC
        _USE_MATH_DEFINES
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        GLEW_STATIC=1)
    set(PLATFORM_INCLUDE_DIRECTORIES
        include/glew/include
        include/openal-soft/include
        include/freetype2/include)
    set(PLATFORM_DEBUG_OPTIONS "-MTd")
    set(PLATFORM_RELEASE_OPTIONS "-MT")
endif()
endif()

set(DEFINITIONS
    XPLM200=1
    XPLM210=1
    XPLM300=1
    XPLM301=1
    ${PLATFORM_CORE_DEFINITIONS})

include_directories(${CMAKE_SOURCE_DIR}/../SDK/CHeaders/XPLM
                    ${CMAKE_SOURCE_DIR}/../SDK/CHeaders/Widgets
                    ${CMAKE_SOURCE_DIR}/include/simpleini
                    ${PLATFORM_INCLUDE_DIRECTORIES})


set(SOURCES
    src/pluginpath.cpp
    src/settings.cpp
    src/dataref.cpp
    src/messagewindow.cpp
    src/onscreendisplay.cpp
    src/owneddata.cpp
    src/logichandler.cpp
    src/texture.cpp
    src/overlaygauge.cpp
    src/log.cpp
    src/logwriter.cpp
    src/menuitem.cpp
    src/processor.cpp
    src/vertexbuffer.cpp
    src/alsoundbuffer.cpp
    src/alcontextmanager.cpp
    src/alcontextchanger.cpp
    src/fontmgr.cpp)


add_library(PPL STATIC ${SOURCES})
target_compile_options(PPL PUBLIC ${PLATFORM_COMPILE_OPTIONS})
target_compile_options(PPL PUBLIC "$<$<CONFIG:DEBUG>:${PLATFORM_DEBUG_OPTIONS}>")
target_compile_options(PPL PUBLIC "$<$<CONFIG:RELEASE>:${PLATFORM_RELEASE_OPTIONS}>")
target_compile_definitions(PPL PRIVATE ${DEFINITIONS})
