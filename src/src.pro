CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = corelib

!contains(BEQT_CONFIG, beqt_no_network):SUBDIRS += network
!contains(BEQT_CONFIG, beqt_no_widgets):SUBDIRS += widgets
!contains(BEQT_CONFIG, beqt_no_widgets):!contains(BEQT_CONFIG, beqt_no_codeeditor):SUBDIRS += codeeditor

TRANSLATIONS += ../translations/beqt_ru.ts
