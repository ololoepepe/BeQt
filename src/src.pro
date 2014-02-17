TEMPLATE = subdirs

SUBDIRS = corelib

!contains(BEQT_CONFIG, no_network) {
    SUBDIRS += network
    network.depends = corelib
}
!contains(BEQT_CONFIG, no_sql) {
    SUBDIRS += sql
    sql.depends = corelib
}
!contains(BEQT_CONFIG, no_widgets) {
    SUBDIRS += widgets
    widgets.depends = corelib
}
!contains(BEQT_CONFIG, no_widgets):!contains(BEQT_CONFIG, no_codeeditor) {
    SUBDIRS += codeeditor
    codeeditor.depends = corelib widgets
}
!contains(BEQT_CONFIG, no_widgets):!contains(BEQT_CONFIG, no_network)!contains(BEQT_CONFIG, no_networkwidgets) {
    SUBDIRS += networkwidgets
    networkwidgets.depends = corelib network widgets
}

TRANSLATIONS += ../translations/beqt_ru.ts
