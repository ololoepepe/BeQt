TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core

BEQT =

include(../module.pri)

DEFINES += BEQT_BUILD_CORE_LIB

HEADERS += \
    bbase.h \
    bbase_p.h \
    bcoreapplication.h \
    bcoreapplication_p.h \
    bdirtools.h \
    bglobal.h \
    bnamespace.h \
    bpersoninfoprovider.h \
    bpersoninfoprovider_p.h \
    bplugininterface.h \
    bpluginwrapper.h \
    bpluginwrapper_p.h \
    bterminaliohandler.h \
    bterminaliohandler_p.h \
    btranslator.h \
    btranslator_p.h

SOURCES += \
    bbase.cpp \
    bcoreapplication.cpp \
    bdirtools.cpp \
    bglobal.cpp \
    bpersoninfoprovider.cpp \
    bpluginwrapper.cpp \
    bterminaliohandler.cpp \
    btranslator.cpp

contains(CONFIG, beqt_builtin_resources):RESOURCES += beqtcore.qrc
