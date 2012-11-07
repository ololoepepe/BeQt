TEMPLATE = app
TARGET = mainwindow

CONFIG += release

QT = core gui

BEQT = core widgets
BEQT_PART=example

include(../../../depend.pri)

HEADERS += \
    settingstab.h \
    application.h

SOURCES = \
    main.cpp \
    settingstab.cpp \
    application.cpp
