TEMPLATE = lib
TARGET = BeQtCodeEditor

CONFIG += release

QT = core gui #widgets #Qt5

BEQT = core widgets

include(../module.pri)

DEFINES += BEQT_BUILD_CODEEDITOR_LIB

HEADERS += \
    babstractdocumentdriver.h \
    babstractdocumentdriver_p.h \
    babstracteditormodule.h \
    babstracteditormodule_p.h \
    babstractfiletype.h \
    bbookmarkseditormodule.h \
    bbookmarkseditormodule_p.h \
    bcodeedit_p.h \
    bcodeedit.h \
    bcodeeditor_p.h \
    bcodeeditor.h \
    bcodeeditordocument.h \
    bcodeeditordocument_p.h \
    bediteditormodule.h \
    bediteditormodule_p.h \
    bindicatorseditormodule.h \
    bindicatorseditormodule_p.h \
    blocaldocumentdriver.h \
    blocaldocumentdriver_p.h \
    bopensaveeditormodule.h \
    bopensaveeditormodule_p.h \
    bsearcheditormodule.h \
    bsearcheditormodule_p.h

SOURCES += \
    babstractdocumentdriver.cpp \
    babstracteditormodule.cpp \
    babstractfiletype.cpp \
    bbookmarkseditormodule.cpp \
    bcodeedit.cpp \
    bcodeeditor.cpp \
    bcodeeditordocument.cpp \
    bediteditormodule.cpp \
    bindicatorseditormodule.cpp \
    blocaldocumentdriver.cpp \
    bopensaveeditormodule.cpp \
    bsearcheditormodule.cpp
