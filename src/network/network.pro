TEMPLATE = lib
TARGET = BeQtNetwork

CONFIG += release

QT = core network

BEQT = core

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_NETWORK_LIB

include(network.pri)
