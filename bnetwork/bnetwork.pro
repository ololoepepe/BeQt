TARGET = beqtnetwork
TEMPLATE = lib
VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

DEFINES += BNETWORK_LIBRARY

QT -= gui
QT += network

CONFIG += release

HEADERS += \
    bgenericserver.h \
    bgenericsocket.h \
    bnetworkconnection.h \
    bnetworkoperation.h \
    bnetworkserver.h \
    bnetworkserverworker.h \
    bsocketwrapper.h \
    private/blocalserver.h \
    private/bnetworkserverthread.h \
    private/btcpserver.h

SOURCES += \
    bgenericserver.cpp \
    bgenericsocket.cpp \
    bnetworkconnection.cpp \
    bnetworkoperation.cpp \
    bnetworkserver.cpp \
    bnetworkserverworker.cpp \
    bsocketwrapper.cpp \
    private/blocalserver.cpp \
    private/bnetworkserverthread.cpp \
    private/btcpserver.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../bcore/release/ -lbeqtcore1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../bcore/debug/ -lbeqtcore1
else:unix:!symbian: LIBS += -L$$OUT_PWD/../bcore/ -lbeqtcore

INCLUDEPATH += $$PWD/../bcore
DEPENDPATH += $$PWD/../bcore

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
