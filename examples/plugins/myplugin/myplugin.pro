TEMPLATE = lib
TARGET = myplugin

CONFIG += release plugin

QT = core gui

BEQT = core widgets
BEQT_PART=example

include(../../../depend.pri)

HEADERS += \
    myplugin.h \
    settingstab.h

SOURCES = \
    myplugin.cpp \
    settingstab.cpp
