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

unix:isEmpty(PREFIX):PREFIX = /usr
win32:PREFX = ../BeQt

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
I_HEADERS.path = $$PREFIX/include/beqt
I_TRANSLATIONS.path = $$PREFIX/share/beqt/translations
}
win32 {
I_HEADERS.path = $$PREFIX/include
I_TRANSLATIONS.path = $$PREFIX/translations
}

###############################################################################
# INSTALLS
###############################################################################

INSTALLS += I_HEADERS
INSTALLS += I_TRANSLATIONS
