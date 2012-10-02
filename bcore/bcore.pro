TARGET = beqtcore
TEMPLATE = lib
VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

DEFINES += BCORE_LIBRARY

QT -= gui

CONFIG += release

SOURCES += \
    bcore.cpp \
    bcryptor.cpp \
    bsignalmapper.cpp \
    bterminaliohandler.cpp

HEADERS += bcore.h \
    bcryptor.h \
    bplugininterface.h \
    bsignalmapper.h \
    bterminaliohandler.h

RESOURCES += \
    bcore.qrc

builddir = .build

MOC_DIR = $$builddir
OBJECTS_DIR = $$builddir
RCC_DIR = $$builddir

###############################################################################
# PREFIX
###############################################################################

unix:isEmpty(PREFIX):PREFIX = /usr
win32:PREFIX = ../BeQt

###############################################################################
# INSTALLS
###############################################################################

target.path = $$PREFIX/lib
INSTALLS = target
