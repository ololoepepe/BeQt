TEMPLATE = subdirs

SUBDIRS += \
    bcore \
    bgui \
    bnetwork

TRANSLATIONS += \
    translations/beqt_ru.ts

unix {
### Translations ###
trans.files = translations/*.qm
trans.path = /usr/share/beqt/translations
INSTALLS += trans
}
win32 {
isEmpty(PREFIX) {
    PREFIX = $$(systemdrive)/Program files/BeQt
}
### Translations ###
trans.files = translations/*.qm
trans.path = $$PREFIX/translations
INSTALLS += trans
}
