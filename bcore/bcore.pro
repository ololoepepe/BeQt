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

unix {
isEmpty(PREFIX) {
    PREFIX = /usr
}
}
win32 {
isEmpty(PREFIX) {
    contains(QMAKE_HOST.arch, x86_64) {
        PREFIX = $$quote($$(systemdrive)/Program files (x86))
    } else {
        PREFIX = $$quote($$(systemdrive)/Program files)
    }
}
}

###############################################################################
# INSTALLS.path
###############################################################################

unix {
target.path = $$PREFIX/lib
}
win32 {
target.path = $$quote($$PREFIX/BeQt/lib)
}

###############################################################################
# INSTALLS
###############################################################################

INSTALLS = target
