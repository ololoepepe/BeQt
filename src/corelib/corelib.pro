TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core

BEQT = core
BEQT_HEADERS_ONLY=true

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_CORE_LIB

HEADERS += \
    global/bglobal.h

SOURCES += \
    global/bglobal.cpp
