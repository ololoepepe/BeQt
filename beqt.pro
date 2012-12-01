CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = src

contains(CONFIG, beqt_examples):SUBDIRS += examples

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

#Gets include subdirectory name
#Returns a list of actual headers' paths to which headers in the given subdir points
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

!contains(CONFIG, beqt_no_headers) {
    #Global
    beqtInstallsHeadersGlobal.files=$$getActualHeaders(BeQt)
    beqtInstallsHeadersGlobal.path=$${headersInstallsPath}/BeQt
    INSTALLS += beqtInstallsHeadersGlobal
    #Core
    beqtInstallsHeadersCore.files=$$getActualHeaders(BeQtCore)
    beqtInstallsHeadersCore.path=$${headersInstallsPath}/BeQtCore
    INSTALLS += beqtInstallsHeadersCore
    #Network
    !contains(CONFIG, beqt_no_network) {
        beqtInstallsHeadersNetwork.files=$$getActualHeaders(BeQtNetwork)
        beqtInstallsHeadersNetwork.path=$${headersInstallsPath}/BeQtNetwork
        INSTALLS += beqtInstallsHeadersNetwork
    }
    #Widgets
    !contains(CONFIG, beqt_no_widgets) {
        beqtInstallsHeadersWidgets.files=$$getActualHeaders(BeQtWidgets)
        beqtInstallsHeadersWidgets.path=$${headersInstallsPath}/BeQtWidgets
        INSTALLS += beqtInstallsHeadersWidgets
    }
    #CodeEditor
    !contains(CONFIG, beqt_no_codeeditor) {
        beqtInstallsHeadersCodeeditor.files=$$getActualHeaders(BeQtCodeEditor)
        beqtInstallsHeadersCodeeditor.path=$${headersInstallsPath}/BeQtCodeEditor
        INSTALLS += beqtInstallsHeadersCodeeditor
    }
}

##############################################################################
################################ Translations ################################
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
beqtInstallsTranslations.files=$$files($${PWD}/translations/*.qm)
beqtInstallsTranslations.path=$${resourcesInstallsPath}/translations
INSTALLS += beqtInstallsTranslations

##############################################################################
################################ Other resources #############################
##############################################################################

beqtInstallsDepend.files=depend.pri
beqtInstallsDepend.path=$${resourcesInstallsPath}
INSTALLS += beqtInstallsDepend
