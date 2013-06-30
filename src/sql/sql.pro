TEMPLATE = lib
TARGET = BeQtSql

CONFIG += release

QT = core sql

BEQT = core

include(../module.pri)

DEFINES += BEQT_BUILD_SQL_LIB

HEADERS += \
    bsqldatabase.h \
    bsqlquery.h \
    bsqlresult.h \
    bsqlwhere.h

SOURCES += \
    bsqldatabase.cpp \
    bsqlquery.cpp \
    bsqlresult.cpp \
    bsqlwhere.cpp

!contains(BEQT_CONFIG, no_install) {

include(../../prefix.pri)

target.path = $${BEQT_LIBS_INSTALLS_PATH}
INSTALLS = target

} #end !contains(BEQT_CONFIG, no_install)
