TARGET = beqtnetwork
TEMPLATE = lib
VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

DEFINES += BNETWORK_LIBRARY

QT -= gui
QT += network

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

MOC_DIR = .build
OBJECTS_DIR = .build
RCC_DIR = .build

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
libdir = "$$(systemdrive)/Program files/BeQt"
### Target ###
target.path = "$$libdir/lib"
INSTALLS = target
### Includes ###
includes.files = *.h
includes.path = "$$libdir/include"
INSTALLS += includes
}
