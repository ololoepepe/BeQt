CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = corelib

!contains(CONFIG, beqt_no_network):SUBDIRS += network
!contains(CONFIG, beqt_no_widgets):SUBDIRS += widgets
!contains(CONFIG, beqt_no_codeeditor):SUBDIRS += codeeditor

TRANSLATIONS += ../translations/beqt_ru.ts
