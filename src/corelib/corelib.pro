TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core

BEQT =

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_CORE_LIB

include(corelib.pri)
