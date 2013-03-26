CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = src

contains(CONFIG, beqt_examples):SUBDIRS += examples

##############################################################################
################################ Generating translations #####################
##############################################################################

#Gets a file name
#Returns the given file name.
#On Windows slash characters will be replaced by backslashes
defineReplace(nativeFileName) {
    fileName=$${1}
    win32:fileName=$$replace(fileName, "/", "\\")
    return($${fileName})
}

beqtTranslationsTs=$$files($${PWD}/translations/*.ts)
for(fileName, beqtTranslationsTs) {
    system(lrelease $$nativeFileName($${fileName}))
}

##############################################################################
################################ Installing ##################################
##############################################################################

!contains(CONFIG, beqt_no_install) {

include(prefix.pri)

##############################################################################
################################ Headers #####################################
##############################################################################

#Gets contents of an .h header
#Returns the corresponding actual header path
defineReplace(getActualHeaderInternal) {
    headerContents=$${1}
    actualHeader=$$replace(headerContents, "$${LITERAL_HASH}include", "")
    actualHeader=$$replace(actualHeader, "\\.\\./\\.\\./", "")
    actualHeader=$$replace(actualHeader, "\"", "")
    return($${PWD}/$${actualHeader})
}

#Gets path to a header (either .h or with no extension)
#Returns corresponding actual header path
defineReplace(getActualHeader) {
    headerPath=$${1}
    headerContents=$$cat($${headerPath})
    isEmpty(headerContents) {
        headerPath=
    } else:!equals(headerContents, $$replace(headerContents, "\\.\\./\\.\\./", "")) {
        headerPath=$$getActualHeaderInternal($${headerContents})
    } else:!equals(headerContents, $$replace(headerContents, "\\.\\./", "")) {
        #Getting corresponding header in another include subdirectory
        headerPath=$$replace(headerContents, "$${LITERAL_HASH}include", "")
        headerPath=$$replace(headerPath, "\\.\\./", "")
        headerPath=$$replace(headerPath, "\"", "")
        #Getting the actual header
        headerContents=$$cat($${PWD}/include/$${headerPath})
        headerPath=$$getActualHeaderInternal($${headerContents})
    }
    return($${headerPath})
}

#Gets path to a private header
#Returns corresponding actual private header path
defineReplace(getActualPrivateHeader) {
    headerPath=$${1}
    headerContents=$$cat($${headerPath})
    isEmpty(headerContents) {
        headerPath=
    } else:!equals(headerContents, $$replace(headerContents, "\\.\\./\\.\\./\\.\\./", "")) {
        headerPath=$$replace(headerContents, "$${LITERAL_HASH}include", "")
        headerPath=$$replace(headerPath, "\\.\\./\\.\\./\\.\\./", "")
        headerPath=$$replace(headerPath, "\"", "")
        headerPath=$${PWD}/$${headerPath}
    }
    return($${headerPath})
}

#Gets include subdirectory name
#Returns a list of actual headers' paths to which headers in the given subdir point
defineReplace(getActualHeaders) {
    subdirName=$${1}
    headerPaths=$$files($${PWD}/include/$${subdirName}/*)
    actualHeaderPaths=
    for(headerPath, headerPaths) {
        actualHeaderPath=$$getActualHeader($${headerPath})
        !isEmpty(actualHeaderPath):!equals(actualHeaderPath, $${PWD}/):actualHeaderPaths+=$${actualHeaderPath}
    }
    return($${actualHeaderPaths})
}

#Gets include subdirectory name
#Returns a list of actual private headers' paths to which headers in the given subdir point
defineReplace(getActualPrivateHeaders) {
    subdirName=$${1}
    headerPaths=$$files($${PWD}/include/$${subdirName}/private/*)
    actualHeaderPaths=
    for(headerPath, headerPaths) {
        actualHeaderPath=$$getActualPrivateHeader($${headerPath})
        !isEmpty(actualHeaderPath):!equals(actualHeaderPath, $${PWD}/):actualHeaderPaths+=$${actualHeaderPath}
    }
    return($${actualHeaderPaths})
}

!contains(CONFIG, beqt_no_headers) {
    #Global
    beqtInstallsHeadersGlobal.files=$$getActualHeaders(BeQt)
    beqtInstallsHeadersGlobal.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQt
    INSTALLS += beqtInstallsHeadersGlobal
    #Core
    beqtInstallsHeadersCore.files=$$getActualHeaders(BeQtCore)
    beqtInstallsHeadersCore.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCore
    INSTALLS += beqtInstallsHeadersCore
    contains(CONFIG, beqt_private_headers) {
        beqtInstallsPrivateHeadersCore.files=$$getActualPrivateHeaders(BeQtCore)
        beqtInstallsPrivateHeadersCore.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCore/private
        INSTALLS += beqtInstallsPrivateHeadersCore
    }
    #Network
    !contains(CONFIG, beqt_no_network) {
        beqtInstallsHeadersNetwork.files=$$getActualHeaders(BeQtNetwork)
        beqtInstallsHeadersNetwork.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtNetwork
        INSTALLS += beqtInstallsHeadersNetwork
        contains(CONFIG, beqt_private_headers) {
            beqtInstallsPrivateHeadersNetwork.files=$$getActualPrivateHeaders(BeQtNetwork)
            beqtInstallsPrivateHeadersNetwork.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtNetwork/private
            INSTALLS += beqtInstallsPrivateHeadersNetwork
        }
    }
    #Widgets
    !contains(CONFIG, beqt_no_widgets) {
        beqtInstallsHeadersWidgets.files=$$getActualHeaders(BeQtWidgets)
        beqtInstallsHeadersWidgets.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtWidgets
        INSTALLS += beqtInstallsHeadersWidgets
        contains(CONFIG, beqt_private_headers) {
            beqtInstallsPrivateHeadersWidgets.files=$$getActualPrivateHeaders(BeQtWidgets)
            beqtInstallsPrivateHeadersWidgets.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtWidgets/private
            INSTALLS += beqtInstallsPrivateHeadersWidgets
        }
    }
    #CodeEditor
    !contains(CONFIG, beqt_no_codeeditor) {
        beqtInstallsHeadersCodeeditor.files=$$getActualHeaders(BeQtCodeEditor)
        beqtInstallsHeadersCodeeditor.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCodeEditor
        INSTALLS += beqtInstallsHeadersCodeeditor
        contains(CONFIG, beqt_private_headers) {
            beqtInstallsPrivateHeadersCodeEditor.files=$$getActualPrivateHeaders(BeQtCodeEditor)
            beqtInstallsPrivateHeadersCodeEditor.path=$${BEQT_HEADERS_INSTALLS_PATH}/BeQtCodeEditor/private
            INSTALLS += beqtInstallsPrivateHeadersCodeEditor
        }
    }
    #Depend
    beqtInstallsDepend.files=depend.pri
    beqtInstallsDepend.path=$${BEQT_RESOURCES_INSTALLS_PATH}
    INSTALLS += beqtInstallsDepend
}
beqtInstallsTranslations.files=$$files($${PWD}/translations/*.qm)
beqtInstallsTranslations.path=$${resourcesInstallsPath}/translations
INSTALLS += beqtInstallsTranslations

##############################################################################
################################ Other resources #############################
##############################################################################

##############################################################################
################################ Translations ################################
##############################################################################

!contains(CONFIG, beqt_builtin_resources) {
    beqtInstallsTranslations.files=$$files($${PWD}/translations/*.qm)
    beqtInstallsTranslations.path=$${BEQT_RESOURCES_INSTALLS_PATH}/translations
    INSTALLS += beqtInstallsTranslations
}

} #end !contains(CONFIG, beqt_no_install)
