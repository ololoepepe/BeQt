TEMPLATE = lib
TARGET = BeQtWidgets

CONFIG += release

QT = core gui
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets

BEQT = core

include(../module.pri)

DEFINES += BEQT_BUILD_WIDGETS_LIB

HEADERS += \
    baboutdialog.h \
    baboutdialog_p.h \
    babstractsettingstab.h \
    babstractterminaldriver.h \
    bapplication.h \
    bapplication_p.h \
    bclipboardnotifier.h \
    bclipboardnotifier_p.h \
    bdialog.h \
    bdialog_p.h \
    bfiledialog.h \
    bfiledialog_p.h \
    bflowlayout.h \
    bguiplugininterface.h \
    bhelpbrowser.h \
    bhelpbrowser_p.h \
    blineedit.h \
    blineedit_p.h \
    blinenumberwidget.h \
    blinenumberwidget_p.h \
    blocalecombobox.h \
    blocalterminaldriver.h \
    bpasswordwidget_p.h \
    bpasswordwidget.h \
    bplaintextedit.h \
    bplaintextedit_p.h \
    bpluginssettingstab.h \
    bpluginssettingstab_p.h \
    bsettingsdialog.h \
    bsettingsdialog_p.h \
    bterminalwidget.h \
    bterminalwidget_p.h \
    btextcodecmenu_p.h \
    btextcodeccombobox.h \
    btextcodeccombobox_p.h \
    btextcodecmenu.h \
    binputfield.h \
    bpasswordgroup_p.h \
    bpasswordgroup.h \
    beditgroup_p.h \
    beditgroup.h \
    bloginwidget_p.h \
    bloginwidget.h \
    bimagewidget.h \
    bguitools.h

SOURCES += \
    baboutdialog.cpp \
    babstractsettingstab.cpp \
    babstractterminaldriver.cpp \
    bapplication.cpp \
    bclipboardnotifier.cpp \
    bdialog.cpp \
    bflowlayout.cpp \
    bfiledialog.cpp \
    bhelpbrowser.cpp \
    blineedit.cpp \
    blinenumberwidget.cpp \
    blocalecombobox.cpp \
    blocalterminaldriver.cpp \
    bpasswordwidget.cpp \
    bplaintextedit.cpp \
    bpluginssettingstab.cpp \
    bsettingsdialog.cpp \
    bterminalwidget.cpp \
    btextcodeccombobox.cpp \
    btextcodecmenu.cpp \
    binputfield.cpp \
    bpasswordgroup.cpp \
    beditgroup.cpp \
    bloginwidget.cpp \
    bimagewidget.cpp \
    bguitools.cpp

contains(BEQT_CONFIG, builtin_resources) {
    RESOURCES += beqtwidgets.qrc
    DEFINES += BEQT_BUILTIN_RESOURCES
}

!contains(BEQT_CONFIG, no_install) {

include(../../prefix.pri)

target.path = $${BEQT_LIBS_INSTALLS_PATH}
INSTALLS = target

!contains(BEQT_CONFIG, builtin_resources) {
    beqtInstallsIcons.files= \
        $$files($${PWD}/icons/*.png) \
        $$files($${PWD}/icons/*.svgz)
    beqtInstallsIcons.path=$${BEQT_RESOURCES_INSTALLS_PATH}/icons
    INSTALLS += beqtInstallsIcons
    beqtInstallsPixmaps.files=$$files($${PWD}/pixmaps/*.png)
    beqtInstallsPixmaps.path=$${BEQT_RESOURCES_INSTALLS_PATH}/pixmaps
    INSTALLS += beqtInstallsPixmaps
}

} #end !contains(BEQT_CONFIG, no_install)
