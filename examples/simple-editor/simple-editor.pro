TEMPLATE = app
TARGET = simple-editor

CONFIG += release

QT = core gui widgets

BEQT = core widgets codeeditor

include(../example.pri)

SOURCES = \
    main.cpp
