TEMPLATE = lib
TARGET = BeQtCodeEditor

CONFIG += release

BEQT = core widgets

include(../module.pri)

DEFINES += BEQT_BUILD_CODEEDITOR_LIB

HEADERS += \
    babstractcodeeditordocument.h \
    babstractcodeeditordocument_p.h \
    babstractdocumentdriver.h \
    babstractdocumentdriver_p.h \
    babstracteditormodule.h \
    babstracteditormodule_p.h \
    babstractfiletype.h \
    babstractfiletype_p.h \
    bbookmarkseditormodule.h \
    bbookmarkseditormodule_p.h \
    bcodeedit_p.h \
    bcodeedit.h \
    bcodeeditor_p.h \
    bcodeeditor.h \
    bcodeeditordocument.h \
    bcodeeditordocument_p.h \
    bediteditormodule.h \
    bediteditormodule_p.h \
    bextendedfiledialog.h \
    bindicatorseditormodule.h \
    bindicatorseditormodule_p.h \
    blocaldocumentdriver.h \
    blocaldocumentdriver_p.h \
    bopensaveeditormodule.h \
    bopensaveeditormodule_p.h \
    bsearcheditormodule.h \
    bsearcheditormodule_p.h \
    bsimplecodeeditordocument.h \
    bsimplecodeeditordocument_p.h \
    btextblockuserdata.h

SOURCES += \
    babstractcodeeditordocument.cpp \
    babstractdocumentdriver.cpp \
    babstracteditormodule.cpp \
    babstractfiletype.cpp \
    bbookmarkseditormodule.cpp \
    bcodeedit.cpp \
    bcodeeditor.cpp \
    bcodeeditordocument.cpp \
    bediteditormodule.cpp \
    bextendedfiledialog.cpp \
    bindicatorseditormodule.cpp \
    blocaldocumentdriver.cpp \
    bopensaveeditormodule.cpp \
    bsearcheditormodule.cpp \
    bsimplecodeeditordocument.cpp \
    btextblockuserdata.cpp

!contains(BEQT_CONFIG, no_install) {

include(../../prefix.pri)

target.path = $${BEQT_LIBS_INSTALLS_PATH}
INSTALLS = target

} #end !contains(BEQT_CONFIG, no_install)
