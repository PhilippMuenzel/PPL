cmake_minimum_required (VERSION 3.1)
cmake_policy(VERSION 3.1)
project (PPL)
include(FindOpenGL)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 14)

option(PPL_VERSION "The version for PPL" OFF)
option(PPL_PRIVATE_NAMESPACE "The namespace that is required for building PPL as per the forum" OFF)
option(PPL_BUILD_STANDALONE "Whether or not to build PPL as a library independent from X-Plane" OFF)
option(PPL_WITH_SOUND "Whether or not to build PPL with sound support" OFF)
option(PPL_WITH_FREETYPE "Whether or not to build PPL with freetype support" OFF)
option(PPL_WITH_SERIALIZATION "Whether or not to build PPL with serialization support" OFF)

if(NOT PPL_PRIVATE_NAMESPACE)
    message(FATAL_ERROR "PPL_PRIVATE_NAMESPACE not defined, but it is required!")
endif()

if(NOT PPL_VERSION)
    message(STATUS "No version defined, using 1.0.0")
    set(PPL_VERSION "1.0.0")
endif()

include_directories(include/simpleini ../SDK/CHeaders/XPLM ../SDK/CHeaders/Widgets)
add_definitions(-DXPLM200 -DXPLM210 -DPRIVATENAMESPACE=${PPL_PRIVATE_NAMESPACE})
set(LIBRARY_NAME ppl)
if(PPL_BUILD_STANDALONE)
    add_definitions(-DBUILD_FOR_STANDALONE)
    set(LIBRARY_NAME pplstandalone)
endif()

set(HEADERS
    src/dataref.h
    src/messagewindow.h
     src/onscreendisplay.h
     src/pluginpath.h
     src/owneddata.h
     src/logichandler.h
     src/settings.h
     src/texture.h
     src/overlaygauge.h
     src/xposd.h
     src/log.h
     src/logwriter.h
     src/basics.h
     src/menuitem.h
     src/action.h
     src/smoothed.h
     src/processor.h
     src/namespaces.h
     src/vertexbuffer.hpp)

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
     src/smoothed.cpp
     src/processor.cpp
     src/vertexbuffer.cpp )

if(APPLE)
    add_definitions(-DAPL=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wfloat-equal -pedantic")
elseif(WIN32)
    add_definitions(-DIBM=1 -D_USE_MATH_DEFINES -DNOMINMAX -DWIN32_LEAN_AND_MEAN -DGLEW_STATIC=1)
    set(SOURCES ${SOURCES} include/glew-2.1.0/src/glew.c)
    include_directories(include/glew-2.1.0/include)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4996")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Zi -MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -MT")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -MT")
    set(LIBRARY_NAME ppld)
else()
    add_definitions(-DLIN=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wfloat-equal -Wno-c++11-narrowing -pedantic -fvisibility=hidden -std=c++14")
endif()

if(NOT PPL_BUILD_STANDALONE AND NOT EXISTS ${CMAKE_SOURCE_DIR}/../SDK)
    message(FATAL_ERROR "Unable to locate X-Plane SDK.  Please put it one level up from your project in a folder called SDK")
endif()

 if(PPL_WITH_SOUND)
    if(WIN32)
        include_directories(include/openal-soft/include)
    endif()
    set(HEADERS "${HEADERS} src/alsoundbuffer.h src/alcontextmanager.h src/alcontextchanger.h")
    set(SOURCES ${SOURCES}
         src/alsoundbuffer.cpp
          src/alcontextmanager.cpp
          src/alcontextchanger.cpp)
 endif()

 if(PPL_WITH_FREETYPE)
    if(WIN32)
        include_directories(include/freetype2/include)
        add_definitions(-DFREETYPE2_STATIC)
    endif()

    set(HEADERS "${HEADERS} src/fontmgr.h")
    set(SOURCE ${SOURCES} src/fontmgr.cpp)
 endif()

 if(PPL_WITH_SERIALIZATION)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost REQUIRED COMPONENTS serialization)
    include_directories(${Boost_INCLUDE_DIRS})
    set(HEADERS "${HEADERS} src/sharedobject.h")
    set(SOURCES ${SOURCES} src/sharedobject.cpp)
 endif()

 find_package(OpenGL REQUIRED)
 include_directories(${OPENGL_INCLUDE_DIRS})
 if(WIN32 AND NOT EXISTS ${CMAKE_SOURCE_DIR}/include/glew-2.1.0)
    file(DOWNLOAD https://jaist.dl.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0.tgz ${CMAKE_SOURCE_DIR}/include/glew.tgz)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/include/glew.tgz WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/include)
    file(REMOVE ${CMAKE_SOURCE_DIR}/include/glew.tgz)
 endif()

 add_library(${LIBRARY_NAME} SHARED ${SOURCES})
 target_link_libraries(${LIBRARY_NAME} ${OPENGL_LIBRARIES})
 set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${PPL_VERSION} PUBLIC_HEADER ${HEADERS})

 if(NOT PPL_BUILD_STANDALONE)
     if(WIN32)
         find_library(XPLM_LIB NAMES XPLM_64 HINTS "../SDK/Libraries/Win")
        if(NOT XPLM_LIB)
             message(FATAL_ERROR "XPLM_64.lib not found.  X-Plane SDK is corrupted")
        endif()

        find_library(XPWIDGETS_LIB NAMES XPWidgets_64 HINTS "../SDK/Libraries/Win")
        if(NOT XPWIDGETS_LIB)
             message(FATAL_ERROR "XPWidgets_64.lib not found.  X-Plane SDK is corrupted.")
        endif()

        target_link_libraries(${LIBRARY_NAME} ${XPLM_LIB} ${XPWIDGETS_LIB})
    elseif(APPLE)
        find_library(XPLM_LIB NAMES XPLM HINTS "../SDK/Libraries/Mac")
        if(NOT XPLM_LIB)
            message(FATAL_ERROR "XPLM.framework not found.  X-Plane SDK is corrupted")
        endif()

        find_library(XPWIDGETS_LIB NAMES XPWidgets HINTS "../SDK/Libraries/Mac")
        if(NOT XPWIDGETS_LIB)
            message(FATAL_ERROR "XPWidgets.framework not found.  X-Plane SDK is corrupted.")
        endif()

        target_link_libraries(${LIBRARY_NAME} ${XPLM_LIB} ${XPWIDGETS_LIB})
    endif()
endif()

if(PPL_WITH_SOUND)
    if(WIN32)
        find_library(OPENAL_LIB OpenAL32 HINTS "$ENV{SystemDrive}/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64")
        if(NOT OPENAL_LIB)
            message(FATAL_ERROR "Unable to locate OpenAL, expected to be in $ENV{SystemDrive}/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64")
        else()
            message(STATUS "Found OpenAL at ${OPENAL_LIB}")
        endif()

        target_link_libraries(${LIBRARY_NAME} ${OPENAL_LIB})
    else()
        find_package(OpenAL REQUIRED)
        target_link_libraries(${LIBRARY_NAME} ${OPENAL_LIBRARY})
    endif()
endif()

if(PPL_WITH_SERIALIZATION)
    add_definitions(-DBOOST_ALL_NO_LIB)
    target_link_libraries(${LIBRARY_NAME} ${Boost_LIBRARIES})
endif()
