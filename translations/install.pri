#Gets a file name
#Returns the given file name.
#On Windows slash characters will be replaced by backslashes
defineReplace(beqtNativeFileName) {
    beqt_file_name=$${1}
    win32:beqt_native_file_name=$$replace(beqt_file_name, "/", "\\")
    else:beqt_native_file_name=$${beqt_file_name}
    return($${beqt_native_file_name})
}

beqt_translations_ts=$$files($${PWD}/*.ts)
for(beqt_translation_ts, beqt_translations_ts) {
    beqt_translation_ts_native=$$beqtNativeFileName($${beqt_translation_ts})
    system(lrelease $${beqt_translation_ts_native})
}
beqt_installs_translations.files=$$files($${PWD}/*.qm)
#TODO: Paths should be defined in a separate .pri file
#test
beqt_installs_translations.path=/home/darkangel/tmp/x_beqt/translations
#end test
INSTALLS += beqt_installs_translations
