TEMPLATE = lib

# Static library without any Qt functionality
QT -= gui core

CONFIG += static console c++14 warn_on
CONFIG -= thread qt

VERSION = 2.0.0

INCLUDEPATH += include/simpleini
INCLUDEPATH += include/SDK/CHeaders/XPLM
INCLUDEPATH += include/SDK/CHeaders/Widgets

# Defined to use X-Plane SDK 2.0, 2.1, 3.0 and 3.01 capabilities - no backward compatibility before 11.20
DEFINES += XPLM200 XPLM210 XPLM300 XPLM301

OBJECTS_DIR  = objects
DESTDIR = lib
TARGET = ppl

macx {
    DEFINES += APL=1 IBM=0 LIN=0
    QMAKE_CXXFLAGS += -Wextra -Wfloat-equal -pedantic

    # Build for multiple architectures.
    # The following line is only needed to build universal on PPC architectures.
    # QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
    # This line defines for which architectures we build.
    CONFIG += x86_64
}

win32 {
    INCLUDEPATH += include/glew/include
    DEFINES += APL=0 IBM=1 LIN=0
    #disable the deprecated warnings that make writing standards-compliant code impossible
    QMAKE_CXXFLAGS += -wd4996
    QMAKE_CXXFLAGS_DEBUG = -Zi -MTd
    QMAKE_CXXFLAGS_RELEASE = -MT
    DEFINES += _USE_MATH_DEFINES NOMINMAX WIN32_LEAN_AND_MEAN GLEW_STATIC=1
}

linux {
    DEFINES += APL=0 IBM=0 LIN=1
    QMAKE_CXXFLAGS += -Wextra -Wfloat-equal -Wno-c++11-narrowing -pedantic
    QMAKE_CXXFLAGS += -fvisibility=hidden
    QMAKE_CXXFLAGS_CXX11 = -std=c++14
}

CONFIG( debug, debug|release ) {
    # debug settings go here
    !win32 {
        QMAKE_CXXFLAGS_DEBUG += -ftrapv
    }
    TARGET = ppld
} else {
    DEFINES += NDEBUG
}

HEADERS += \
    src/dataref.h \
    src/messagewindow.h \
    src/onscreendisplay.h \
    src/pluginpath.h \
    src/owneddata.h \
    src/logichandler.h \
    src/settings.h \
    src/texture.h \
    src/overlaygauge.h \
    src/xposd.h \
    src/log.h \
    src/logwriter.h \
    src/basics.h \
    src/menuitem.h \
    src/action.h \
    src/smoothed.h \
    src/processor.h \
    src/vertexbuffer.hpp \
    src/command.h

SOURCES += \
    src/pluginpath.cpp \
    src/settings.cpp \
    src/dataref.cpp \
    src/messagewindow.cpp \
    src/onscreendisplay.cpp \
    src/owneddata.cpp \
    src/logichandler.cpp \
    src/texture.cpp \
    src/overlaygauge.cpp \
    src/log.cpp \
    src/logwriter.cpp \
    src/menuitem.cpp \
    src/processor.cpp \
    src/vertexbuffer.cpp \
    src/command.cpp

withsound {
    win32 {
        INCLUDEPATH += include/openal-soft/include
    }
    HEADERS += \
        src/alsoundbuffer.h \
        src/alcontextmanager.h \
        src/alcontextchanger.h
    SOURCES += \
        src/alsoundbuffer.cpp \
        src/alcontextmanager.cpp \
        src/alcontextchanger.cpp
}

withfreetype {
    win32 {
        INCLUDEPATH += include/freetype2/include
        DEFINES+=FREETYPE2_STATIC
    }
    linux {
        INCLUDEPATH += /usr/include/freetype2
    }
    macx {
        INCLUDEPATH += /usr/X11/include/freetype2/
    }

    HEADERS += src/fontmgr.h
    SOURCES += src/fontmgr.cpp
}
