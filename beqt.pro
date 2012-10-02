TEMPLATE = subdirs

SUBDIRS += \
    bcore \
    bgui \
    bnetwork

CONFIG += release

TRANSLATIONS += \
    translations/beqt_ru.ts


###############################################################################
# PREFIX
###############################################################################

unix {
isEmpty(PREFIX) {
    PREFIX = /usr
}
}
win32 {
isEmpty(PREFIX) {
    contains(QMAKE_HOST.arch, x86_64) {
        PREFIX = $$quote($$(systemdrive)/Program files (x86))
    } else {
        PREFIX = $$quote($$(systemdrive)/Program files)
    }
}
}

###############################################################################
# INSTALLS.files
###############################################################################

I_HEADERS.files = \
    bcore/*.h \ #TODO: move headers to "include" subdirectory
    bgui/*.h \ #TODO: move headers to "include" subdirectory
    bnetwork/*.h #TODO: move headers to "include" subdirectory
I_TRANSLATIONS.files = translations/*.qm

###############################################################################
# INSTALLS.path
###############################################################################

unix {
TARGET.path = $$PREFIX/lib
I_HEADERS.path = $$PREFIX/include/beqt
I_TRANSLATIONS.path = $$PREFIX/share/beqt/translations
}
win32 {
TARGET.path = $$quote($$PREFIX/BeQt/lib)
I_HEADERS.path = $$quote($$PREFIX/BeQt/include)
I_TRANSLATIONS.path = $$quote($$PREFIX/BeQt/translations)
}

###############################################################################
# INSTALLS
###############################################################################

INSTALLS = TARGET
INSTALLS += I_HEADERS
INSTALLS += I_TRANSLATIONS
