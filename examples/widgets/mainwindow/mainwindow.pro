TEMPLATE = app
TARGET = mainwindow

CONFIG += release

QT = core gui

BEQT = core widgets
BEQT_PART=example

include(../../../depend.pri)

SOURCES = \
    main.cpp \
    settingstab.cpp \
    application.cpp

HEADERS += \
    settingstab.h \
    application.h
