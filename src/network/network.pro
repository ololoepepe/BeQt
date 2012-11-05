TEMPLATE = lib
TARGET = BeQtNetwork

CONFIG += release

QT = core network

BEQT = core

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_NETWORK_LIB

HEADERS += \
    bgenericserver.h \
    bgenericserver_p.h \
    bgenericsocket.h \
    bgenericsocket_p.h \
    blocalserver.h \
    bnetworkconnection.h \
    bnetworkoperation.h \
    bnetworkoperationmetadata.h \
    bnetworkserver.h \
    bnetworkserver_p.h \
    bnetworkserverthread.h \
    bnetworkserverworker.h \
    bsocketwrapper.h \
    bsocketwrapper_p.h \
    btcpserver.h \

SOURCES += \
    bgenericserver.cpp \
    bgenericsocket.cpp \
    blocalserver.cpp \
    bnetworkconnection.cpp \
    bnetworkoperation.cpp \
    bnetworkoperationmetadata.cpp \
    bnetworkserver.cpp \
    bnetworkserverthread.cpp \
    bnetworkserverworker.cpp \
    bsocketwrapper.cpp \
    btcpserver.cpp
