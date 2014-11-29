!contains(BEQT_CONFIG, no_docs) {

beqtWhichCommand=
mac|unix {
    beqtWhichCommand=which
} else:win32 {
    beqtWhichCommand=WHERE
}

isEmpty(QDOC_COMMAND):QDOC_COMMAND=$$system($${beqtWhichCommand} qdoc)
isEmpty(QDOC_COMMAND):QDOC_COMMAND=$$system($${beqtWhichCommand} qdoc3)

!isEmpty(QDOC_COMMAND) {

system($${QDOC_COMMAND} beqt.qdocconf)

include(../prefix.pri)

!contains(BEQT_CONFIG, no_install) {
    beqtInstallsDocs.files=$$files($${PWD}/generated/*)
    beqtInstallsDocs.path=$${BEQT_DOCS_INSTALLS_PATH}
    INSTALLS += beqtInstallsDocs
}

} #end !isEmpty(QDOC_COMMAND)

} #end !contains(BEQT_CONFIG, no_docs)
