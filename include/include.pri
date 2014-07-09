!contains(BEQT_CONFIG, no_install):!contains(BEQT_CONFIG, no_headers) {

include(../prefix.pri)

#Gets contents of an .h header
#Returns the corresponding actual header path
defineReplace(getActualHeaderPathInternal) {
    headerContents=$${1}
    actualHeader=$$replace(headerContents, "$${LITERAL_HASH}include", "")
    actualHeader=$$replace(actualHeader, "\\.\\./\\.\\./", "")
    actualHeader=$$replace(actualHeader, "\"", "")
    return($${PWD}/../$${actualHeader})
}

#Gets include subdirectory name
#Returns a list of actual headers' paths to which headers in the given subdir point
defineReplace(getActualHeaderPaths) {
    subdirName=$${1}
    headerPaths=$$files($${PWD}/$${subdirName}/*)
    actualHeaderPaths=
    for(headerPath, headerPaths) {
        headerContents=$$cat($${headerPath})
        !isEmpty(headerContents) {
            !equals(headerContents, $$replace(headerContents, "\\.\\./\\.\\./", "")) {
                headerPath=$$getActualHeaderPathInternal($${headerContents})
            } else:!equals(headerContents, $$replace(headerContents, "\\.\\./", "")) {
                #Getting corresponding header in another include subdirectory
                headerPath=$$replace(headerContents, "$${LITERAL_HASH}include", "")
                headerPath=$$replace(headerPath, "\\.\\./", "")
                headerPath=$$replace(headerPath, "\"", "")
                #Getting the actual header
                headerContents=$$cat($${PWD}/../$${headerPath})
                headerPath=$$getActualHeaderPathInternal($${headerContents})
            }
            #TODO: !equals(headerPath, $${PWD}/../) it dirty! Fix it
            !isEmpty(headerPath):!equals(headerPath, $${PWD}/../):actualHeaderPaths+=$${headerPath}
        }
    }
    return($${actualHeaderPaths})
}

#Gets include subdirectory name
#Returns a list of actual private headers' paths to which headers in the given subdir point
defineReplace(getActualPrivateHeaderPaths) {
    subdirName=$${1}
    headerPaths=$$files($${PWD}/$${subdirName}/private/*)
    actualHeaderPaths=
    for(headerPath, headerPaths) {
        headerContents=$$cat($${headerPath})
        !isEmpty(headerContents):!equals(headerContents, $$replace(headerContents, "\\.\\./\\.\\./\\.\\./", "")) {
            headerPath=$$replace(headerContents, "$${LITERAL_HASH}include", "")
            headerPath=$$replace(headerPath, "\\.\\./\\.\\./\\.\\./", "")
            headerPath=$$replace(headerPath, "\"", "")
            headerPath=$${PWD}/../$${headerPath}
            #TODO: !equals(headerPath, $${PWD}/../) it dirty! Fix it
            !equals(headerPath, $${PWD}/../):actualHeaderPaths+=$${headerPath}
        }
    }
    return($${actualHeaderPaths})
}

#Global
beqtInstallsHeadersGlobal.files=$$getActualHeaderPaths(BeQt)
beqtInstallsHeadersGlobal.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQt
#message($$getActualHeaderPaths(BeQt))
INSTALLS += beqtInstallsHeadersGlobal

#Core
beqtInstallsHeadersCore.files=$$getActualHeaderPaths(BeQtCore)
beqtInstallsHeadersCore.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCore
INSTALLS += beqtInstallsHeadersCore
contains(BEQT_CONFIG, private_headers) {
    beqtInstallsPrivateHeadersCore.files=$$getActualPrivateHeaderPaths(BeQtCore)
    beqtInstallsPrivateHeadersCore.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCore/private
    INSTALLS += beqtInstallsPrivateHeadersCore
}

#Network
!contains(BEQT_CONFIG, no_network) {
    beqtInstallsHeadersNetwork.files=$$getActualHeaderPaths(BeQtNetwork)
    beqtInstallsHeadersNetwork.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtNetwork
    INSTALLS += beqtInstallsHeadersNetwork
    contains(BEQT_CONFIG, private_headers) {
        beqtInstallsPrivateHeadersNetwork.files=$$getActualPrivateHeaderPaths(BeQtNetwork)
        beqtInstallsPrivateHeadersNetwork.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtNetwork/private
        INSTALLS += beqtInstallsPrivateHeadersNetwork
    }
}

#Sql
!contains(BEQT_CONFIG, no_sql) {
    beqtInstallsHeadersSql.files=$$getActualHeaderPaths(BeQtSql)
    beqtInstallsHeadersSql.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtSql
    INSTALLS += beqtInstallsHeadersSql
    contains(BEQT_CONFIG, private_headers) {
        beqtInstallsPrivateHeadersSql.files=$$getActualPrivateHeaderPaths(BeQtSql)
        beqtInstallsPrivateHeadersSql.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtSql/private
        INSTALLS += beqtInstallsPrivateHeadersSql
    }
}

    #Widgets
!contains(BEQT_CONFIG, no_widgets) {
    beqtInstallsHeadersWidgets.files=$$getActualHeaderPaths(BeQtWidgets)
    beqtInstallsHeadersWidgets.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtWidgets
    INSTALLS += beqtInstallsHeadersWidgets
    contains(BEQT_CONFIG, private_headers) {
        beqtInstallsPrivateHeadersWidgets.files=$$getActualPrivateHeaderPaths(BeQtWidgets)
        beqtInstallsPrivateHeadersWidgets.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtWidgets/private
        INSTALLS += beqtInstallsPrivateHeadersWidgets
    }
}

#NetworkWidgets
!contains(BEQT_CONFIG, no_networkwidgets) {
    beqtInstallsHeadersNetworkWidgets.files=$$getActualHeaderPaths(BeQtNetworkWidgets)
    beqtInstallsHeadersNetworkWidgets.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtNetworkWidgets
    INSTALLS += beqtInstallsHeadersNetworkWidgets
    contains(BEQT_CONFIG, private_headers) {
        beqtInstallsPrivateHeadersNetworkWidgets.files=$$getActualPrivateHeaderPaths(BeQtNetworkWidgets)
        beqtInstallsPrivateHeadersNetworkWidgets.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtNetworkWidgets/private
        INSTALLS += beqtInstallsPrivateHeadersNetworkWidgets
    }
}

#CodeEditor
!contains(BEQT_CONFIG, no_codeeditor) {
    beqtInstallsHeadersCodeeditor.files=$$getActualHeaderPaths(BeQtCodeEditor)
    beqtInstallsHeadersCodeeditor.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCodeEditor
    INSTALLS += beqtInstallsHeadersCodeeditor
    contains(BEQT_CONFIG, private_headers) {
        beqtInstallsPrivateHeadersCodeEditor.files=$$getActualPrivateHeaderPaths(BeQtCodeEditor)
        beqtInstallsPrivateHeadersCodeEditor.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCodeEditor/private
        INSTALLS += beqtInstallsPrivateHeadersCodeEditor
    }
}

} #end !contains(BEQT_CONFIG, no_install):!contains(BEQT_CONFIG, no_headers)
