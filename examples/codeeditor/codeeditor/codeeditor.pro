TEMPLATE = app
TARGET = codeeditor

CONFIG += release

QT = core gui

BEQT = core widgets codeeditor
BEQT_PART=example

include(../../../depend.pri)

SOURCES = \
    main.cpp
