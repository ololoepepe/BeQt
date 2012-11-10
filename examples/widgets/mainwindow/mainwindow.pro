TEMPLATE = app
TARGET = mainwindow

CONFIG += release

QT = core network gui

BEQT = core network widgets
BEQT_PART=example

include(../../../depend.pri)

HEADERS += \
    settingstab.h \
    application.h \
    mainwindow.h

SOURCES = \
    main.cpp \
    settingstab.cpp \
    application.cpp \
    mainwindow.cpp
