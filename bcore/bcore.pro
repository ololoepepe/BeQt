TARGET = beqtcore
TEMPLATE = lib
VER_MAJ = 0
VER_MIN = 1
VER_PAT = 0

DEFINES += BCORE_LIBRARY

QT -= gui

SOURCES += \
    bcore.cpp \
    bcryptor.cpp \
    bsignalmapper.cpp \
    bstdio.cpp

HEADERS += bcore.h \
    bcryptor.h \
    bplugininterface.h \
    bsignalmapper.h \
    bstdio.h

RESOURCES += \
    bcore.qrc
