TEMPLATE = lib
TARGET = BeQtWidgets

CONFIG += release

QT = core gui #Replace "gui" with "widgets" if using Qt5

BEQT = core widgets_h

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_WIDGETS_LIB

HEADERS += \
    bflowlayout.h

SOURCES += \
    bflowlayout.cpp
