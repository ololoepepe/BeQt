TARGET = beqtgui
TEMPLATE = lib
VER_MAJ = 0
VER_MIN = 1
VER_PAT = 0

DEFINES += BGUI_LIBRARY

QT += xml

SOURCES += \
    babstractsettingstab.cpp \
    bcontainerwidget.cpp \
    bgeneralsettingstab.cpp \
    blogindialog.cpp \
    bmainwindow.cpp \
    bpasswordwidget.cpp \
    bsyntax.cpp \
    bterminal.cpp \
    btexteditor.cpp \
    private/baboutdialog.cpp \
    private/bencoding.cpp \
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
    private/btexteditorsettingstab.cpp

HEADERS += \
    babstractsettingstab.h \
    bcontainerwidget.h \
    bgeneralsettingstab.h \
    bguiplugininterface.h \
    blogindialog.h \
    bmainwindow.h \
    bpasswordwidget.h \
    bsyntax.h \
    bterminal.h \
    btexteditor.h \
    private/baboutdialog.h \
    private/bencoding.h \
    private/bfinddialog.h \
    private/bhelpwidget.h \
    private/bguicommon.h \
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
    private/btexteditorsettingstab.h

TRANSLATIONS += \
    ../bcore/res/translations/bgui_ru.ts

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../bcore/release/ -lbeqtcore0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../bcore/debug/ -lbeqtcore0
else:unix:!symbian: LIBS += -L$$OUT_PWD/../bcore/ -lbeqtcore

INCLUDEPATH += $$PWD/../bcore
DEPENDPATH += $$PWD/../bcore
