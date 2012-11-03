TEMPLATE = lib
TARGET = BeQtWidgets

CONFIG += release

QT = core gui #Replace "gui" with "widgets" if using Qt5

BEQT = core

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_WIDGETS_LIB

HEADERS += \
    baboutdialog.h \
    baboutdialog_p.h \
    babstractsettingstab.h \
    bapplication.h \
    bflowlayout.h \
    bsettingsdialog.h

SOURCES += \
    baboutdialog.cpp \
    babstractsettingstab.cpp \
    bapplication.cpp \
    bflowlayout.cpp \
    bsettingsdialog.cpp
