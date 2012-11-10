#Include this file in your project if you are using BeQt as a submodule
#Source files will be added to your project
#No libraries will be built

#Gets module name, for example "BeQtCore", "BeQtWidgets", etc.
#Returns corresponding module sources' subdir, for example "corelib", "widgets", etc.
defineReplace(beqtModuleSubdir) {
    beqt_module_name=$${1}
    equals(beqt_module_name, BeQtCore):beqt_module_subdir=corelib
    else:equals(beqt_module_name, BeQtNetwork):beqt_module_subdir=network
    else:equals(beqt_module_name, BeQtWidgets):beqt_module_subdir=widgets
    else:equals(beqt_module_name, BeQtCodeEditor):beqt_module_subdir=codeeditor
    return($${beqt_module_subdir})
}

#Gets module short name, for example "core", "widgets", etc.
#Returns corresponding headers subdir, for example "BeQtCore", "BeQtWidgets", etc.
defineReplace(beqtHeadersSubdir) {
    beqt_module_short_name=$${1}
    beqt_headers_subdir=
    equals(beqt_module_short_name, codeeditor):beqt_headers_subdir=BeQtCodeEditor
    equals(beqt_module_short_name, core):beqt_headers_subdir=BeQtCore
    equals(beqt_module_short_name, network):beqt_headers_subdir=BeQtNetwork
    equals(beqt_module_short_name, widgets):beqt_headers_subdir=BeQtWidgets
    return($${beqt_headers_subdir})
}

#Gets headers subdir, for example "BeQtCore", "BeQtWidgets", etc.
#Appends "INCLUDEPATH" and "DEPENDPATH" with corresponding paths
defineTest(beqtAddHeaders) {
    beqt_headers_subdir=$${1}
    INCLUDEPATH *= $${PWD}/include/$${beqt_headers_subdir}
    DEPENDPATH *= $${PWD}/include/$${beqt_headers_subdir}
    beqt_sources_subdir=$$beqtModuleSubdir($${beqt_headers_subdir})
    include($${PWD}/src/$${beqt_sources_subdir}/$${beqt_sources_subdir}.pri)
    export(INCLUDEPATH)
    export(DEPENDPATH)
    export(HEADERS)
    export(SOURCES)
}

INCLUDEPATH *= $${PWD}/include
DEPENDPATH *= $${PWD}/include

#Replaces "all" meta-module with the list of all BeQt modules
contains(BEQT, all) {
    BEQT=
    BEQT *= \
        codeeditor \
        core \
        network \
        widgets
}

#Add required Qt and BeQt modules (on which other included modules depend)
contains(BEQT, codeeditor) {
    QT *= core gui #Qt4
    #QT *= core gui #Qt5
    BEQT *= core widgets
}
contains(BEQT,core ) {
    QT *= core
}
contains(BEQT, network) {
    QT *= core network
    BEQT *= core network
}
contains(BEQT, widgets) {
    QT *= core gui #Qt4
    #QT *= core gui #Qt5
    BEQT *= core widgets
}

#Adds corresponding headers' paths for each valid BeQt module contained in "BEQT" variable
for(beqt_module, BEQT) {
    beqt_headers_subdir=$$beqtHeadersSubdir($${beqt_module})
    !isEmpty(beqt_headers_subdir):beqtAddHeaders($${beqt_headers_subdir})
}
