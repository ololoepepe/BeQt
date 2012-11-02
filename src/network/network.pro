TEMPLATE = lib
TARGET = BeQtNetwork

CONFIG += release

QT = core network

BEQT = core network_h

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_NETWORK_LIB

HEADERS += \
    bgenericserver.h \
    bgenericsocket.h \
    blocalserver.h \
    bnetworkconnection.h \
    bnetworkoperation.h \
    bnetworkserver.h \
    bnetworkserverthread.h \
    bnetworkserverworker.h \
    bsocketwrapper.h \
    btcpserver.h

SOURCES += \
    bgenericserver.cpp \
    bgenericsocket.cpp \
    blocalserver.cpp \
    bnetworkconnection.cpp \
    bnetworkoperation.cpp \
    bnetworkserver.cpp \
    bnetworkserverthread.cpp \
    bnetworkserverworker.cpp \
    bsocketwrapper.cpp \
    btcpserver.cpp
