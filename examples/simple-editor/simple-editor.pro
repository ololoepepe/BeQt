TEMPLATE = app
TARGET = simple-editor

CONFIG += release

QT = core gui widgets

BEQT = core widgets codeeditor networkwidgets

include(../example.pri)

SOURCES = \
    main.cpp
