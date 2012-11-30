TEMPLATE = app
TARGET = simple-editor

CONFIG += release

QT = core gui

BEQT = core widgets codeeditor

include(../example.pri)

SOURCES = \
    main.cpp
