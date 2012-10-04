TARGET = beqtgui
TEMPLATE = lib
VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

DEFINES += BGUI_LIBRARY

QT += xml

CONFIG += release

SOURCES += \
    babstractsettingstab.cpp \
    bcontainerwidget.cpp \
    private/bgeneralsettingstab.cpp \
    blogindialog.cpp \
    bmainwindow.cpp \
    bpasswordwidget.cpp \
    bsyntax.cpp \
    bterminal.cpp \
    btexteditor.cpp \
    private/baboutdialog.cpp \
    private/bfinddialog.cpp \
    private/bhelpwidget.cpp \
    private/bmacrorecorder.cpp \
    private/bmultilinesyntaxelement.cpp \
    private/bopensavedialog.cpp \
    private/bplaintextedit.cpp \
    private/bselectfilesdialog.cpp \
    private/bsettingsdialog.cpp \
    private/bsplittedlinesdialog.cpp \
    private/bsyntaxelement.cpp \
    private/bsyntaxhighlighter.cpp \
    private/btexteditordocument.cpp \
    private/btexteditorsettingstab.cpp \
    babstractfiletype.cpp \
    bkeyboardlayoutmap.cpp \
    private/ballfiles.cpp \
    bflowlayout.cpp

HEADERS += \
    babstractsettingstab.h \
    bcontainerwidget.h \
    private/bgeneralsettingstab.h \
    bguiplugininterface.h \
    blogindialog.h \
    bmainwindow.h \
    bpasswordwidget.h \
    bsyntax.h \
    bterminal.h \
    btexteditor.h \
    private/baboutdialog.h \
    private/bfinddialog.h \
    private/bhelpwidget.h \
    private/bmacrorecorder.h \
    private/bmultilinesyntaxelement.h \
    private/bopensavedialog.h \
    private/bplaintextedit.h \
    private/bselectfilesdialog.h \
    private/bsettingsdialog.h \
    private/bsplittedlinesdialog.h \
    private/bsyntaxelement.h \
    private/bsyntaxhighlighter.h \
    private/btexteditordocument.h \
    private/btexteditorsettingstab.h \
    babstractfiletype.h \
    bkeyboardlayoutmap.h \
    private/ballfiles.h \
    bflowlayout.h

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

isEmpty(PREFIX) {
unix:PREFIX = /usr
win32:PREFIX = $$(systemdrive)/PROGRA~1/BeQt
}

###############################################################################
# INSTALLS
###############################################################################

target.path = $$PREFIX/lib
INSTALLS = target
