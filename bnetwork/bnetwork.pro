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

TRANSLATIONS += \
    ../bcore/res/translations/bnetwork_ru.ts

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../bcore/release/ -lbeqtcore1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../bcore/debug/ -lbeqtcore1
else:unix:!symbian: LIBS += -L$$OUT_PWD/../bcore/ -lbeqtcore

INCLUDEPATH += $$PWD/../bcore
DEPENDPATH += $$PWD/../bcore
