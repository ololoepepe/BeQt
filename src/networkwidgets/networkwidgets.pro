TEMPLATE = lib
TARGET = BeQtNetworkWidgets

CONFIG += release

BEQT = core network widgets

include(../module.pri)

DEFINES += BEQT_BUILD_NETWORKWIDGETS_LIB

HEADERS += \
    boperationprogressdialog.h \
    boperationprogressdialog_p.h

SOURCES += \
    boperationprogressdialog.cpp

!contains(BEQT_CONFIG, no_install) {

include(../../prefix.pri)

target.path = $${BEQT_LIBS_INSTALLS_PATH}
INSTALLS = target

} #end !contains(BEQT_CONFIG, no_install)
