TARGET = beqtcore
TEMPLATE = lib
VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

DEFINES += BCORE_LIBRARY

QT -= gui

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

unix {
### Target ###
target.path = /usr/lib
INSTALLS = target
### Includes ###
includes.files = *.h
includes.path = /usr/include/beqt
INSTALLS += includes
}
win32 {
isEmpty(PREFIX) {
    PREFIX = $$(systemdrive)/Program files/BeQt
}
### Target ###
target.path = $$PREFIX/lib
INSTALLS = target
### Includes ###
includes.files = *.h
includes.path = $$PREFIX/include
INSTALLS += includes
}
