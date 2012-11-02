#Gets "standalone" or anything
#Returns true if essential BeQt files exist, otherwise retruns false
defineTest(beqtTestEssentials) {
    equals($${1}, standalone):beqt_is_standalone=true
    beqt_test_ok=false
    win32|!isEmpty(beqt_is_standalone) {
        exists("$${PWD}/include"):exists("$${PWD}/lib"):beqt_test_ok=true
    } else:mac {
        exists("$${PWD}/../Headers"):exists("$${PWD}/../Frameworks"):beqt_test_ok=true
    } else:unix:!mac {
        exists("$${PWD}/../../include/beqt"):beqt_test_ok=true
    }
    equals(beqt_test_ok, true):return(true)
    else:return(false)
}

#Gets "standalone" or anything
#Returns true if BeQt resource files exist, otherwise retruns false
defineTest(beqtTestResources) {
    equals($${1}, standalone):beqt_is_standalone=true
    beqt_test_ok=false
    exists("$${PWD}/images"):exists("$${PWD}/translations"):beqt_test_ok=true
    equals(beqt_test_ok, true):return(true)
    else:return(false)
}

#Gets "standalone" or anything
#Returns BeQt headers base path
defineReplace(beqtHeadersPrefix) {
    equals($${1}, standalone) {
        beqt_headers_prefix=$${PWD}/include
    } else {
        mac:beqt_headers_prefix=$${PWD}/../Headers
        unix:!macx:beqt_headers_prefix=$${PWD}/../../include/beqt
        #No system headers prefix on Windows
    }
    return($${beqt_headers_prefix})
}

#Gets "standalone" or anything
#Returns BeQt libs base path
defineReplace(beqtLibsPrefix) {
    equals($${1}, standalone) {
        beqt_libs_prefix=$${PWD}/lib
    } else {
        mac:beqt_libs_prefix=$${PWD}/../Frameworks
        unix:!macx:beqt_libs_prefix=$${PWD}/../../lib #Not sure if necessary
        #No system libs prefix on Windows
    }
    return($${beqt_libs_prefix})
}

#Gets module short name, for example "core", "widgets", etc.
#Returns corresponding full module name, for example "BeQtCore", "BeQtWidgets", etc.
defineReplace(beqtModuleName) {
    beqt_module_short_name=$${1}
    beqt_module_name=
    equals(beqt_module_short_name, codeeditor):beqt_module_name=BeQtCodeEditor
    equals(beqt_module_short_name, core):beqt_module_name=BeQtCore
    equals(beqt_module_short_name, network):beqt_module_name=BeQtNetwork
    equals(beqt_module_short_name, widgets):beqt_module_name=BeQtWidgets
    return($${beqt_module_name})
}

#Gets module name, for example "BeQtCore", "BeQtWidgets", etc.
#Returns corresponding build subdir, for example "corelib", "widgets", etc.
defineReplace(beqtModuleBuildSubdir) {
    beqt_module_name=$${1}
    equals(beqt_module_name, BeQtCore):beqt_module_build_subdir=corelib
    else:equals(beqt_module_name, BeQtNetwork):beqt_module_build_subdir=network
    else:equals(beqt_module_name, BeQtWidgets):beqt_module_build_subdir=widgets
    else:equals(beqt_module_name, BeQtCodeEditor):beqt_module_build_subdir=codeeditor
    return($${beqt_module_build_subdir})
}

#Gets anything
#Throws the "headers or libs not found" error, causing qmake to exit immediately
defineTest(beqtThrowEssentialsError) {
    error("BeQt headers or libs not found")
}

#Gets anything
#Shows the "resources not found" warning
defineTest(beqtShowResourcesWarning) {
    warning("BeQt resources not found")
}

#Determines how BeQt is installed (normal or standalone), or if it is being built
#Determines the headers' and libs' base paths
isEmpty(BEQT_PART) {
    beqtTestEssentials(standalone):beqt_path_type=standalone
    else:beqtTestEssentials(normal):beqt_path_type=normal
    else:beqtThrowEssentialsError()
    #Checks
    win32:equals(beqt_path_type, normal):$$beqtThrowEssentialsError() #No system path on Windows
    !beqtTestResources($${beqt_path_type}):beqtShowResourcesWarning()
    #Headers' and libs' base paths
    beqt_headers_prefix=$$beqtHeadersPrefix($${beqt_path_type})
    beqt_libs_prefix=$$beqtLibsPrefix($${beqt_path_type})
} else {
    beqt_headers_prefix=$${PWD}/include
    equals(BEQT_PART, module):beqt_libs_prefix=$${OUT_PWD}/..
    equals(BEQT_PART, example):beqt_libs_prefix=$${OUT_PWD}/../../../src
}

#If CONFIG contains "release" or "debug", sets special suffix for libs' path
win32 {
    CONFIG(release, debug|release):beqt_release_debug_suffix=/release
    CONFIG(debug, debug|release):beqt_release_debug_suffix=/debug
}

#Gets module name, for example "BeQtCore", "BeQtWidgets", etc.
#Appends "INCLUDEPATH" and "DEPENDPATH" with corresponding paths
#If "BEQT_HEADERS_ONLY" is not set, appends "LIBS" with corresponding libraries
defineTest(beqtAddModule) {
    beqt_module_name=$${1}
    beqt_headers_only=$${2}
    #Headers
    INCLUDEPATH *= $${beqt_headers_prefix}/$${beqt_module_name}
    DEPENDPATH *= $${beqt_headers_prefix}/$${beqt_module_name}
    export(INCLUDEPATH)
    export(DEPENDPATH)
    #Libs
    isEmpty(beqt_headers_only) {
        !isEmpty(BEQT_PART) {
            beqt_module_build_subdir=$$beqtModuleBuildSubdir($${beqt_module_name})
            beqt_libs_path=$${beqt_libs_prefix}/$${beqt_module_build_subdir}$${beqt_release_debug_suffix}
            LIBS *= -L$${beqt_libs_path}/ -l$${beqt_module_name}
        } else {
            mac:$$equals(beqt_path_type, normal) {
                LIBS *= -F$${beqt_libs_prefix}/$${beqt_module_name}.framework/ -framework $${beqt_module_name}
            } else {
                LIBS *= -L$${beqt_libs_path}/ -l$${beqt_module_name}
            }
        }
        export(LIBS)
    }
}

#Appending "INCLUDEPATH" and "DEPENDPATH" with BeQt headers base dir
INCLUDEPATH *= $${beqt_headers_prefix}
DEPENDPATH *= $${beqt_headers_prefix}

#Replaces "all" meta-module with the list of all BeQt modules
contains(BEQT, all) {
    BEQT=
    BEQT *= \
        codeeditor \
        core \
        network \
        widgets
}

#Adds corresponding headers' and libs' paths for each valid BeQt module contained in "BEQT" variable
for(beqt_module, BEQT) {
    beqt_module_noh=$$replace(beqt_module, "\\_h", "")
    beqt_headers_only=
    !equals(beqt_module, $${beqt_module_noh}):beqt_headers_only=true
    beqt_module_name=$$beqtModuleName($${beqt_module_noh})
    !isEmpty(beqt_module_name):beqtAddModule($${beqt_module_name}, $${beqt_headers_only})
}
