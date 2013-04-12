#Include this file in your project if you are using BeQt as an external library set
#No source files will be added to your project

#Gets module short name, for example "core", "widgets", etc.
#Returns corresponding full module name, for example "BeQtCore", "BeQtWidgets", etc.
defineReplace(fullBeqtModuleName) {
    shortName=$${1}
    fullName=
    equals(shortName, codeeditor):fullName=BeQtCodeEditor
    equals(shortName, core):fullName=BeQtCore
    equals(shortName, network):fullName=BeQtNetwork
    equals(shortName, widgets):fullName=BeQtWidgets
    return($${fullName})
}

#Gets short module name, for example "core", "widgets", etc.
#Returns corresponding module subdir, for example "corelib", "widgets", etc.
defineReplace(beqtModuleSubdir) {
    fullName=$${1}
    moduleSubdir=
    equals(fullName, codeeditor):moduleSubdir=codeeditor
    else:equals(fullName, core):moduleSubdir=corelib
    else:equals(fullName, network):moduleSubdir=network
    else:equals(fullName, widgets):moduleSubdir=widgets
    return($${moduleSubdir})
}

#Defining BeQt subdir name
isEmpty(BEQT_SUBDIR_NAME):BEQT_SUBDIR_NAME=beqt

#Searching for headers
beqtHeadersPath=
mac:exists($${PWD}/../Headers):beqtHeadersPath=$${PWD}/../Headers
else:unix:!mac:exists($${PWD}/../../include/beqt):beqtHeadersPath=$${PWD}/../../include/beqt
else:exists($${PWD}/include):beqtHeadersPath=$${PWD}/include
isEmpty(beqtHeadersPath):error("BeQt headers not found")
#Searching for libraries
beqtLibsPath=
beqtLibsOneFolder=
mac:exists($${PWD}/../Frameworks) {
    beqtLibsPath=$${PWD}/../Frameworks
    beqtLibsOneFolder=true
}
else:exists($${PWD}/lib) {
    beqtLibsPath=$${PWD}/lib
    beqtLibsOneFolder=true
}
else:exists($${OUT_PWD}/$${BEQT_SUBDIR_NAME}/src):beqtLibsPath=$${OUT_PWD}/$${BEQT_SUBDIR_NAME}/src
else:exists($${OUT_PWD}/../$${BEQT_SUBDIR_NAME}/src):beqtLibsPath=$${OUT_PWD}/../$${BEQT_SUBDIR_NAME}/src
else:exists($${OUT_PWD}/../../$${BEQT_SUBDIR_NAME}/src):beqtLibsPath=$${OUT_PWD}/../../$${BEQT_SUBDIR_NAME}/src
else:exists($${OUT_PWD}/../../../$${BEQT_SUBDIR_NAME}/src):beqtLibsPath=$${OUT_PWD}/../../../$${BEQT_SUBDIR_NAME}/src
else:beqtLibsOneFolder=true

#If CONFIG contains "release" or "debug", set special suffix for libs' path
win32 {
    releaseDebugSuffix=
    CONFIG(release, debug|release):releaseDebugSuffix=/release
    CONFIG(debug, debug|release):releaseDebugSuffix=/debug
}

#Gets short module name, for example "core", "widgets", etc.
#Appends corresponding paths to DEPENDPATH, INCLUDEPATH and LIBS variables
defineTest(addBeqtModule) {
    shortName=$${1}
    fullName=$$fullBeqtModuleName($${shortName})
    INCLUDEPATH *= $${beqtHeadersPath}/$${fullName}
    DEPENDPATH *= $${beqtHeadersPath}/$${fullName}
    !isEmpty(beqtLibsPath) {
        beqtModuleSubdir=
        isEmpty(beqtLibsOneFolder):beqtModuleSubdir=/$$beqtModuleSubdir($${shortName})
        beqtFinalLibPah=$${beqtLibsPath}$${beqtModuleSubdir}$${releaseDebugSuffix}
        !exists($${beqtFinalLibPah}):beqtFinalLibPah=$${beqtLibsPath}$${beqtModuleSubdir}
        mac:contains(CONFIG, lib_bundle) {
            LIBS *= -F$${beqtFinalLibPah}/ -framework $${fullName}
        } else {
            LIBS *= -L$${beqtFinalLibPah}/ -l$${fullName}
        }
    } else {
        mac:LIBS *= -framework $${fullName}
        else:LIBS *= -l$${fullName}
    }
    export(INCLUDEPATH)
    export(DEPENDPATH)
    export(LIBS)
}

#Appending headers base dir to INCLUDEPATH and DEPENDPATH variables
INCLUDEPATH *= $${beqtHeadersPath}
DEPENDPATH *= $${beqtHeadersPath}

#Replaces "all" meta-module with the list of all BeQt modules
contains(BEQT, all) {
    BEQT *= \
        codeeditor \
        core \
        network \
        widgets
}

#Adds required Qt and BeQt modules (on which other included modules depend)
contains(BEQT, codeeditor) {
    QT *= core gui widgets
    BEQT *= core widgets
}
contains(BEQT,core) {
    QT *= core
}
contains(BEQT, network) {
    QT *= core network
    BEQT *= core network
}
contains(BEQT, widgets) {
    QT *= core gui widgets
    BEQT *= core widgets
}

#Workaround for proper linking when building statically
contains(BEQT, codeeditor):BEQT_ORDERED += codeeditor
contains(BEQT, widgets):BEQT_ORDERED += widgets
contains(BEQT, network):BEQT_ORDERED += network
contains(BEQT, core):BEQT_ORDERED += core

#Adds corresponding headers' and libs' paths for each valid BeQt module contained in BEQT variable
for(shortName, BEQT_ORDERED) {
    addBeqtModule($${shortName})
}
