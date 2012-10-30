TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

include(../module.pri)

QT = core

#TODO: Make it possinle to include headers from "include" directory without including libs

DEFINES += BEQT_BUILD_CORE_LIB

HEADERS += \
    global/bglobal.h

SOURCES += \
    global/bglobal.cpp
