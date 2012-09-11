TARGET = beqtnetwork
TEMPLATE = lib
VER_MAJ = 0
VER_MIN = 1
VER_PAT = 0

DEFINES += BNETWORK_LIBRARY

QT -= gui
QT += network

SOURCES += \
    bnetworkconnection.cpp \
    bsocketshell.cpp
    
HEADERS += \
    bnetworkconnection.h \
    bsocketshell.h

TRANSLATIONS += \
    ../bcore/res/translations/bnetwork_ru.ts

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../bcore/release/ -lbeqtcore0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../bcore/debug/ -lbeqtcore0
else:unix:!symbian: LIBS += -L$$OUT_PWD/../bcore/ -lbeqtcore

INCLUDEPATH += $$PWD/../bcore
DEPENDPATH += $$PWD/../bcore
