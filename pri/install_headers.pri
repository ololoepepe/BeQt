#Gets contents of a normal .h header
#Returns the corresponding header (included into the given header)
defineReplace(beqtRealHeaderInternal) {
    beqt_header_cat=$${1}
    beqt_header=$$replace(beqt_header_cat, "$${LITERAL_HASH}include", "")
    beqt_header=$$replace(beqt_header, "\\.\\./\\.\\./", "")
    beqt_header=$$replace(beqt_header, "\"", "")
    return($${PWD}/../$${beqt_header})
}

#Gets contents of a header (either .h or with no extension)
#Returns corresponding header (included into the given header)
#If the included header points to yet another header, returns that source header
defineReplace(beqtRealHeader) {
    beqt_header=$${1}
    beqt_header_cat=$$cat($${beqt_header})
    !equals(beqt_header_cat, $$replace(beqt_header_cat, "\\.\\./\\.\\./", "")) {
        beqt_header=$$beqtRealHeaderInternal($${beqt_header_cat})
    } else:!equals(beqt_header_cat, $$replace(beqt_header_cat, "\\.\\./", "")) {
        #Getting corresponding header in another include subdirectory
        beqt_header=$$replace(beqt_header_cat, "$${LITERAL_HASH}include", "")
        beqt_header=$$replace(beqt_header, "\\.\\./", "")
        beqt_header=$$replace(beqt_header, "\"", "")
        #Getting the real header
        beqt_header_cat=$$cat($${PWD}/$${beqt_header})
        beqt_header=$$beqtRealHeaderInternal($${beqt_header_cat})
    }
    return($${beqt_header})
}

#Gets include subdirectory name
#Returns a list of "real" headers to which headers in the given subdir points
defineReplace(beqtRealHeaders) {
    beqt_headers_subdir=$${1}
    beqt_headers=$$files($${PWD}/$${beqt_headers_subdir}/*)
    beqt_real_headers=
    for(beqt_header, beqt_headers) {
        beqt_real_headers += $$beqtRealHeader($${beqt_header})
    }
    return($${beqt_real_headers})
}

#TODO: Move to a separate .pri file
#Gets include subdirectory name
#Returns corresponding installation path
defineReplace(beqtHeadersInstallsPath) {
    beqt_headers_subdir=$${1}
    #test
    return(/home/darkangel/tmp/x_beqt/include/$${beqt_headers_subdir})
    #end test
}

#Headers
#Global
beqt_installs_headers_global.files=$$beqtRealHeaders(BeQt)
beqt_installs_headers_global.path=$$beqtHeadersInstallsPath(BeQt)
INSTALLS += beqt_installs_headers_global
#CodeEditor
beqt_installs_headers_codeeditor.files=$$beqtRealHeaders(BeQtCodeEditor)
beqt_installs_headers_codeeditor.path=$$beqtHeadersInstallsPath(BeQtCodeEditor)
INSTALLS += beqt_installs_headers_codeeditor
#Core
beqt_installs_headers_core.files=$$beqtRealHeaders(BeQtCore)
beqt_installs_headers_core.path=$$beqtHeadersInstallsPath(BeQtCore)
INSTALLS += beqt_installs_headers_core
#Network
beqt_installs_headers_network.files=$$beqtRealHeaders(BeQtNetwork)
beqt_installs_headers_network.path=$$beqtHeadersInstallsPath(BeQtNetwork)
INSTALLS += beqt_installs_headers_network
#Widgets
beqt_installs_headers_widgets.files=$$beqtRealHeaders(BeQtWidgets)
beqt_installs_headers_widgets.path=$$beqtHeadersInstallsPath(BeQtWidgets)
INSTALLS += beqt_installs_headers_widgets
