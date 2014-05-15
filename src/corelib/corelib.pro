TEMPLATE = lib
TARGET = BeQtCore

CONFIG += release

QT = core concurrent

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
    bterminaliohandler.h \
    bterminaliohandler_p.h \
    btexttools.h \
    btranslatefunctor.h \
    btranslation.h \
    btranslator.h \
    btranslator_p.h \
    bversion.h \
    btextmatch.h \
    btextmatchlist.h \
    buuid.h

SOURCES += \
    bbase.cpp \
    bcoreapplication.cpp \
    bdirtools.cpp \
    bglobal.cpp \
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
    bterminaliohandler.cpp \
    btexttools.cpp \
    btranslatefunctor.cpp \
    btranslation.cpp \
    btranslator.cpp \
    bversion.cpp \
    btextmatch.cpp \
    btextmatchlist.cpp \
    buuid.cpp

##############################################################################
################################ Hunspell ####################################
##############################################################################

!isEmpty(HUNSPELL_PREFIX) {
    INCLUDEPATH *= $${HUNSPELL_PREFIX}/include
    DEPENDPATH *= $${HUNSPELL_PREFIX}/include
    LIBS *= -L$${HUNSPELL_PREFIX}/lib/ -lhunspell
} else {
    DEFINES += BUILDING_LIBHUNSPELL

    INCLUDEPATH *= $${PWD}/../3rdparty/hunspell
    DEPENDPATH *= $${PWD}/../3rdparty/hunspell

    HEADERS += $$files($${PWD}/../3rdparty/hunspell/*.hxx)
    HEADERS += \
    ../3rdparty/hunspell/config.h \
    ../3rdparty/hunspell/hunvisapi.h

    SOURCES += $$files($${PWD}/../3rdparty/hunspell/*.cxx)
}

##############################################################################
################################ Enca ########################################
##############################################################################

!isEmpty(ENCA_PREFIX) {
    INCLUDEPATH *= $${ENCA_PREFIX}
    DEPENDPATH *= $${ENCA_PREFIX}
    INCLUDEPATH *= $${ENCA_PREFIX}/lib
    DEPENDPATH *= $${ENCA_PREFIX}/lib
    INCLUDEPATH *= $${ENCA_PREFIX}/data
    DEPENDPATH *= $${ENCA_PREFIX}/data
    INCLUDEPATH *= $${ENCA_PREFIX}/tools
    DEPENDPATH *= $${ENCA_PREFIX}/tools
    LIBS *= -L$${ENCA_PREFIX}/lib/ -lenca
} else {
    DEFINES += BUILDING_LIBENCA

    INCLUDEPATH *= $${PWD}/../3rdparty/enca
    DEPENDPATH *= $${PWD}/../3rdparty/enca

    HEADERS += $$files($${PWD}/../3rdparty/enca/lib/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/bulgarian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/chinese/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/croatian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/czech/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/estonian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/hungarian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/latvian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/lithuanian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/polish/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/russian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/slovak/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/slovene/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/data/ukrainian/*.h)
    HEADERS += $$files($${PWD}/../3rdparty/enca/tools/*.h)

    SOURCES += $$files($${PWD}/../3rdparty/enca/lib/*.c)
}

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
