TEMPLATE = lib

# Static library without any Qt functionality
QT -= gui core

CONFIG += static exceptions stl console
CONFIG -= thread qt rtti warn_on

VERSION = 1.0.0

INCLUDEPATH += include/simpleini
INCLUDEPATH += ../SDK/CHeaders/XPLM
INCLUDEPATH += ../SDK/CHeaders/Widgets

# Defined to use X-Plane SDK 2.1 capabilities - no backward compatibility before 10.0
DEFINES += XPLM200 XPLM210

OBJECTS_DIR  = objects
TARGET = ppl

DEFINES += PRIVATENAMESPACE=$$PRIVATENAMESPACE
DESTDIR = lib$$PRIVATENAMESPACE

standalone {
    DEFINES += BUILD_FOR_STANDALONE
    TARGET = pplstandalone
}

macx {
    DEFINES += APL=1 IBM=0 LIN=0
    QMAKE_CXXFLAGS += -Wall -Wextra -Wfloat-equal -pedantic

    # Build for multiple architectures.
    # The following line is only needed to build universal on PPC architectures.
    # QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
    # This line defines for which architectures we build.
    CONFIG += x86 x86_64
}

win32 {
    DEFINES += APL=0 IBM=1 LIN=0
    !win32-msvc2008:!win32-msvc2010 {
        QMAKE_CXXFLAGS += -Werror -Wall -Wextra -pedantic
    } else {
        CONFIG += warn_on
        #disable the deprecated warnings that make writing standards-compliant code impossible
        QMAKE_CXXFLAGS += -wd4996
        DEFINES += NOMINMAX
    }
    INCLUDEPATH += include C:\\Boost\\include\\boost-1_52 openALsoft/include
}

unix:!macx {
    DEFINES += APL=0 IBM=0 LIN=1 HAVE_TR1
    QMAKE_CXXFLAGS += -Wall -Wextra -Wfloat-equal -pedantic
    QMAKE_CXXFLAGS += -fvisibility=hidden -fno-stack-protector
}

CONFIG( debug, debug|release ) {
    # debug settings go here
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
    src/namespaces.h \
    src/vertexbuffer.hpp

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
    src/smoothed.cpp \
    src/processor.cpp \
    src/vertexbuffer.cpp

withsound {
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
        INCLUDEPATH += include ../../Downloads/freetype-2.3.5/include
    }
    unix:!macx {
        INCLUDEPATH += /usr/local/include/freetype2
    }
    macx {
        INCLUDEPATH += /usr/local/include/freetype2
    }

    HEADERS += src/fontmgr.h
    SOURCES += src/fontmgr.cpp
}

withserialization {
    win32 {
        INCLUDEPATH += C:\\Boost\\include\\boost-1_52
    }
    unix:!macx {
        INCLUDEPATH += /usr/local/include/
    }
    macx {
        INCLUDEPATH += /usr/local/include/
    }

    HEADERS += src/sharedobject.h
    SOURCES += src/sharedobject.cpp

}
