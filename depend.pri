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
    equals(shortName, sql):fullName=BeQtSql
    equals(shortName, widgets):fullName=BeQtWidgets
    equals(shortName, networkwidgets):fullName=BeQtNetworkWidgets
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
    else:equals(fullName, sql):moduleSubdir=sql
    else:equals(fullName, widgets):moduleSubdir=widgets
    else:equals(fullName, networkwidgets):moduleSubdir=networkwidgets
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

win32 {
    #If CONFIG contains "release" or "debug", set special suffix for libs' path
    releaseDebugSuffix=
    CONFIG(release, debug|release):releaseDebugSuffix=/release
    CONFIG(debug, debug|release):releaseDebugSuffix=/debug
    #Set suffix for libraries names
    libNameSuffix=3
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
        beqtFinalLibPath=$${beqtLibsPath}$${beqtModuleSubdir}$${releaseDebugSuffix}
        !exists($${beqtFinalLibPath}):beqtFinalLibPath=$${beqtLibsPath}$${beqtModuleSubdir}
        mac:contains(CONFIG, lib_bundle) {
            LIBS *= -F$${beqtFinalLibPath}/ -framework $${fullName}
        } else {
            LIBS *= -L$${beqtFinalLibPath}/ -l$${fullName}$${libNameSuffix}
        }
    } else {
        mac:LIBS *= -framework $${fullName}
        else:LIBS *= -l$${fullName}$${libNameSuffix}
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
        sql \
        widgets \
        networkwidgets
}

#Adds required Qt and BeQt modules (on which other included modules depend)
contains(BEQT, codeeditor) {
    QT *= core gui widgets concurrent
    BEQT *= core widgets
}
contains(BEQT, core) {
    QT *= core concurrent
}
contains(BEQT, network) {
    QT *= core network concurrent
    BEQT *= core
}
contains(BEQT, sql) {
    QT *= core sql concurrent
    BEQT *= core
}
contains(BEQT, widgets) {
    QT *= core gui widgets concurrent
    BEQT *= core
}
contains(BEQT, networkwidgets) {
    QT *= core concurrent network gui widgets
    BEQT *= core network widgets
}

#Workaround for proper linking when building statically
contains(BEQT, codeeditor):BEQT_ORDERED += codeeditor
contains(BEQT, networkwidgets):BEQT_ORDERED += networkwidgets
contains(BEQT, widgets):BEQT_ORDERED += widgets
contains(BEQT, sql):BEQT_ORDERED += sql
contains(BEQT, network):BEQT_ORDERED += network
contains(BEQT, core):BEQT_ORDERED += core

#Adds corresponding headers' and libs' paths for each valid BeQt module contained in BEQT variable
for(shortName, BEQT_ORDERED) {
    addBeqtModule($${shortName})
}

##############################################################################
################################ Hunspell ####################################
##############################################################################

!isEmpty(HUNSPELL_PREFIX) {
    INCLUDEPATH *= $${HUNSPELL_PREFIX}/include
    DEPENDPATH *= $${HUNSPELL_PREFIX}/include
    LIBS *= -L$${HUNSPELL_PREFIX}/lib/ -lhunspell
}
