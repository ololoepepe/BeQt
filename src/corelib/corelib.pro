TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core
greaterThan(QT_MAJOR_VERSION, 4):QT += concurrent

BEQT =

include(../module.pri)

DEFINES += BEQT_BUILD_CORE_LIB

HEADERS += \
    babstractlocationprovider.h \
    bapplicationbase.h \
    bapplicationbase_p.h \
    bbase.h \
    bbase_p.h \
    bbaseobject.h \
    bbaseobject_p.h \
    bcoreapplication.h \
    bcoreapplication_p.h \
    bdirtools.h \
    bglobal.h \
    blocationprovider.h \
    blogger.h \
    blogger_p.h \
    bnamespace.h \
    bpassword.h \
    bpersoninfo.h \
    bpersoninfoprovider.h \
    bpersoninfoprovider_p.h \
    bplugininterface.h \
    bpluginwrapper.h \
    bpluginwrapper_p.h \
    bsettingsnode.h \
    bsignaldelayproxy.h \
    bsignaldelayproxy_p.h \
    bspamnotifier.h \
    bspamnotifier_p.h \
    bspellchecker.h \
    bspellchecker_p.h \
    bspellcheckerdictionary.h \
    bterminal.h \
    bterminal_p.h \
    btexttools.h \
    btranslation.h \
    btranslator.h \
    btranslator_p.h \
    bversion.h \
    btextmatch.h \
    btextmatchlist.h \
    buuid.h

SOURCES += \
    babstractlocationprovider.cpp \
    bapplicationbase.cpp \
    bbase.cpp \
    bbaseobject.cpp \
    bcoreapplication.cpp \
    bdirtools.cpp \
    bglobal.cpp \
    blocationprovider.cpp \
    blogger.cpp \
    bnamespace.cpp \
    bpassword.cpp \
    bpersoninfoprovider.cpp \
    bpluginwrapper.cpp \
    bsettingsnode.cpp \
    bsignaldelayproxy.cpp \
    bspamnotifier.cpp \
    bspellchecker.cpp \
    bspellcheckerdictionary.cpp \
    bterminal.cpp \
    btexttools.cpp \
    btranslation.cpp \
    btranslator.cpp \
    bversion.cpp \
    btextmatch.cpp \
    btextmatchlist.cpp \
    buuid.cpp

include(../3rdparty/3rdparty.pri)

contains(BEQT_CONFIG, builtin_resources) {
    DEFINES += BEQT_BUILTIN_RESOURCES
    RESOURCES += \
        beqtcore.qrc \
        ../../translations/beqt_translations.qrc
}

!contains(BEQT_CONFIG, no_install) {

include(../../prefix.pri)

target.path = $${BEQT_LIBS_INSTALLS_PATH}
INSTALLS = target

!contains(BEQT_CONFIG, builtin_resources) {
    beqtInstallsChangelog.files=$$files($${PWD}/changelog/*.txt)
    beqtInstallsChangelog.path=$${BEQT_RESOURCES_INSTALLS_PATH}/changelog
    INSTALLS += beqtInstallsChangelog
    beqtInstallsCopying.files=$$files($${PWD}/copying/*.txt)
    beqtInstallsCopying.path=$${BEQT_RESOURCES_INSTALLS_PATH}/copying
    INSTALLS += beqtInstallsCopying
    beqtInstallsDescription.files=$$files($${PWD}/description/*.txt)
    beqtInstallsDescription.path=$${BEQT_RESOURCES_INSTALLS_PATH}/description
    INSTALLS += beqtInstallsDescription
    beqtInstallsInfos.files=$$files($${PWD}/infos/*.beqt-info)
    beqtInstallsInfos.path=$${BEQT_RESOURCES_INSTALLS_PATH}/infos
    INSTALLS += beqtInstallsInfos
}

} #end !contains(BEQT_CONFIG, no_install)
