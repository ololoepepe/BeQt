#Gets a file name
#Returns the given file name.
#On Windows slash characters will be replaced by backslashes
defineReplace(nativeFileName) {
    fileName=$${1}
    win32:fileName=$$replace(fileName, "/", "\\")
    return($${fileName})
}

beqtTranslationsTs=$$files($${PWD}/*.ts)
for(fileName, beqtTranslationsTs) {
    system(lrelease $$nativeFileName($${fileName}))
}

!contains(BEQT_CONFIG, builtin_resources) {

include(../prefix.pri)
beqtInstallsTranslations.files=$$files($${PWD}/*.qm)
beqtInstallsTranslations.path=$${BEQT_RESOURCES_INSTALLS_PATH}/translations
INSTALLS += beqtInstallsTranslations

}  #end !contains(BEQT_CONFIG, builtin_resources)
