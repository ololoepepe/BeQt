TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core

BEQT =

include(../module.pri)

DEFINES += BEQT_BUILD_CORE_LIB

HEADERS += \
    bbase.h \
    bbase_p.h \
    bcoreapplication.h \
    bcoreapplication_p.h \
    bdirtools.h \
    bglobal.h \
    blogger.h \
    blogger_p.h \
    bnamespace.h \
    bpersoninfoprovider.h \
    bpersoninfoprovider_p.h \
    bplugininterface.h \
    bpluginwrapper.h \
    bpluginwrapper_p.h \
    bterminaliohandler.h \
    bterminaliohandler_p.h \
    btranslator.h \
    btranslator_p.h


SOURCES += \
    bbase.cpp \
    bcoreapplication.cpp \
    bdirtools.cpp \
    bglobal.cpp \
    blogger.cpp \
    bpersoninfoprovider.cpp \
    bpluginwrapper.cpp \
    bterminaliohandler.cpp \
    btranslator.cpp

contains(CONFIG, beqt_builtin_resources) {
    RESOURCES += beqtcore.qrc
    DEFINES += BEQT_BUILTIN_RESOURCES
    contains(CONFIG, beqt_builtin_translations) {
        RESOURCES += ../../translations/beqt_translations.qrc
        DEFINES += BEQT_BUILTIN_TRANSLATIONS
    }
}

!contains(CONFIG, beqt_no_install) {

include(../../prefix.pri)

target.path = $${libsInstallsPath}
INSTALLS = target

!contains(CONFIG, beqt_builtin_resources) {
    beqtInstallsChangelog.files=$$files($${PWD}/changelog/*.txt)
    beqtInstallsChangelog.path=$${resourcesInstallsPath}/changelog
    INSTALLS += beqtInstallsChangelog
    beqtInstallsCopying.files=$$files($${PWD}/copying/*.txt)
    beqtInstallsCopying.path=$${resourcesInstallsPath}/copying
    INSTALLS += beqtInstallsCopying
    beqtInstallsDescription.files=$$files($${PWD}/description/*.txt)
    beqtInstallsDescription.path=$${resourcesInstallsPath}/description
    INSTALLS += beqtInstallsDescription
    beqtInstallsInfos.files=$$files($${PWD}/infos/*.beqt-info)
    beqtInstallsInfos.path=$${resourcesInstallsPath}/infos
    INSTALLS += beqtInstallsInfos
}

} #end !contains(CONFIG, beqt_no_install)
