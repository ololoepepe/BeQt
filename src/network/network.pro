TEMPLATE = lib
TARGET = BeQtNetwork

CONFIG += release

QT = core network

BEQT = core

include(../module.pri)

DEFINES += BEQT_BUILD_NETWORK_LIB

HEADERS += \
    bapplicationserver.h \
    bapplicationserver_p.h \
    bgenericserver.h \
    bgenericserver_p.h \
    bgenericsocket.h \
    bgenericsocket_p.h \
    bnetworkconnection.h \
    bnetworkconnection_p.h \
    bnetworkoperation.h \
    bnetworkoperation_p.h \
    bnetworkoperationmetadata.h \
    bnetworkserver.h \
    bnetworkserver_p.h \
    bsocketwrapper.h \
    bsocketwrapper_p.h

SOURCES += \
    bapplicationserver.cpp \
    bgenericserver.cpp \
    bgenericsocket.cpp \
    bnetworkconnection.cpp \
    bnetworkoperation.cpp \
    bnetworkoperationmetadata.cpp \
    bnetworkserver.cpp \
    bsocketwrapper.cpp
