CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = corelib

!contains(BEQT_CONFIG, no_network):SUBDIRS += network
!contains(BEQT_CONFIG, no_sql):SUBDIRS += sql
!contains(BEQT_CONFIG, no_widgets):SUBDIRS += widgets
!contains(BEQT_CONFIG, no_widgets):!contains(BEQT_CONFIG, no_codeeditor):SUBDIRS += codeeditor
!contains(BEQT_CONFIG, no_widgets):!contains(BEQT_CONFIG, no_network) {
    !contains(BEQT_CONFIG, no_networkwidgets):SUBDIRS += networkwidgets
}

TRANSLATIONS += ../translations/beqt_ru.ts
