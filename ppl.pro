TEMPLATE = lib

# Static library without any Qt functionality
QT -= gui core

CONFIG += static exceptions release stl console
CONFIG -= thread qt rtti warn_on

VERSION = 1.0.0

INCLUDEPATH += include/simpleini
INCLUDEPATH += ../../SDK/CHeaders/XPLM
INCLUDEPATH += ../../SDK/CHeaders/Widgets

# Defined to use X-Plane SDK 2.0 capabilities - no backward compatibility before 9.0
DEFINES += XPLM200

OBJECTS_DIR  = objects
DESTDIR = lib
TARGET = ppl

macx {
    DEFINES += APL=1 IBM=0 LIN=0
    QMAKE_CXXFLAGS += -Werror -Wall -Wextra -Wfloat-equal -pedantic

    # Build for multiple architectures.
    # The following line is only needed to build universal on PPC architectures.
    # QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
    # This line defines for which architectures we build.
    CONFIG += x86 ppc
}

win32 {
    DEFINES += APL=0 IBM=1 LIN=0
    QMAKE_CXXFLAGS += -Werror -Wall -Wextra -pedantic
}

unix:!macx {
    DEFINES += APL=0 IBM=0 LIN=1 HAVE_TR1
    QMAKE_CXXFLAGS += -Werror -Wall -Wextra -Wshadow -Wfloat-equal -pedantic
    QMAKE_CXXFLAGS += -fvisibility=hidden
}

HEADERS += \
    src/dataref.h \
    src/xpmessagewidget.h \
    src/xposd.h \
    src/pluginpath.h \
    src/owneddata.h \
    src/logichandler.h \
    src/settings.h \
    src/bitmaploader.h \
    src/textureholder.h \
    src/overlaywindow.h \
    src/xposd.h \
    src/log.h \
    src/logwriter.h \
    src/basics.h

SOURCES += \
    src/pluginpath.cpp \
    src/settings.cpp \
    src/bitmaploader.cpp \
    src/dataref.cpp \
    src/xpmessagewidget.cpp \
    src/xposd.cpp \
    src/owneddata.cpp \
    src/logichandler.cpp \
    src/textureholder.cpp \
    src/overlaywindow.cpp \
    src/log.cpp \
    src/logwriter.cpp \
    src/basics.cpp

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
