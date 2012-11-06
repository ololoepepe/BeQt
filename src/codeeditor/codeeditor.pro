TEMPLATE = lib
TARGET = BeQtCodeEditor

CONFIG += release

QT = core gui #Replace "gui" with "widgets" if using Qt5

BEQT = core widgets codeeditor_h

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_CODEEDITOR_LIB

include(codeeditor.pri)
