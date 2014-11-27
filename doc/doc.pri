!contains(BEQT_CONFIG, no_docs) {

isEmpty(DOXYGEN_COMMAND):DOXYGEN_COMMAND=doxygen

beqtDoxygenExists=
mac|unix {
    beqtDoxygenExists=system(which $${DOXYGEN_COMMAND})
} else:win32 {
    beqtDoxygenExists=system(WHERE $${DOXYGEN_COMMAND})
}

!isEmpty(beqtDoxygenExists) {

system($${DOXYGEN_COMMAND})

include(../prefix.pri)

!contains(BEQT_CONFIG, no_install) {
    beqtInstallsDocs.files=$$files($${PWD}/doxygen/*)
    beqtInstallsDocs.path=$${BEQT_DOCS_INSTALLS_PATH}
    INSTALLS += beqtInstallsDocs
}

} #end !isEmpty(beqtDoxygenExists)

} #end !contains(BEQT_CONFIG, no_docs)
