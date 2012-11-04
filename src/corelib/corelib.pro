TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core

BEQT =

include(../module.pri)
include(../../depend.pri)

DEFINES += BEQT_BUILD_CORE_LIB

HEADERS += \
    bbase.h \
    bbase_p.h \
    bcoreapplication.h \
    bcoreapplication_p.h \
    bdirtools.h \
    bglobal.h \
    bnamespace.h \
    bplugininterface.h \
    bpluginwrapper.h \
    bpluginwrapper_p.h \
    bterminaliohandler.h \
    btranslator.h \
    btranslator_p.h

SOURCES += \
    bbase.cpp \
    bcoreapplication.cpp \
    bdirtools.cpp \
    bglobal.cpp \
    bpluginwrapper.cpp \
    bterminaliohandler.cpp \
    btranslator.cpp \
