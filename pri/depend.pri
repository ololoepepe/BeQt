isEmpty(BEQT_PART) {
    exists("$${PWD}/../include"):exists("$${PWD}/../lib"):beqt_standalone_essential=true
    exists("$${PWD}/../images"):exists("$${PWD}/../translations"):beqt_standalone_resources=true
    isEmpty(beqt_standalone_essential) {
        mac {
            exists("$${PWD}/../../Headers"):exists("$${PWD}/../../Frameworks") {
                beqt_system_essential=true
                beqt_libs_prefix=$${PWD}/../../Frameworks
                beqt_include_prefix=$${PWD}/../../Headers
            }
            exists("$${PWD}/../images"):exists("$${PWD}/../translations"):beqt_system_resources=true
        }
        unix:!mac {
            exists("$${PWD}/../../../include/beqt") {
                beqt_system_essential=true
                beqt_libs_prefix=$${PWD}/../../../lib
                beqt_include_prefix=$${PWD}/../../../include/beqt
            }
            exists("$${PWD}/../../../share/beqt/images") {
                exists("$${PWD}/../../../share/beqt/translations"):beqt_system_resources=true
            }
        }
        win32 {
            #Nothing to be done
        }
        isEmpty(beqt_system_essential):error("BeQt headers or libs not found")
        isEmpty(beqt_system_resources):warning("BeQt resources not found")
    } else {
        isEmpty(beqt_standalone_resources):warning("BeQt resources not found")
        beqt_libs_prefix=$${PWD}/../lib
        beqt_include_prefix=$${PWD}../include
    }
} else {
    equals(BEQT_PART, module):beqt_libs_prefix=$${OUT_PWD}/..
    equals(BEQT_PART, example):beqt_libs_prefix=$${OUT_PWD}/../../../src
    beqt_include_prefix=$${PWD}/../include
}

#win32 CONFIG
win32 {
    CONFIG(release, debug|release):beqt_project_config_postfix=release
    CONFIG(debug, debug|release):beqt_project_config_postfix=debug
}

defineTest(beqtAddModule) {
    beqt_module_name=$${1}
    #Libs
    !isEmpty(BEQT_PART) {
        beqt_module_build_subdir=$${2}
        LIBS += -L$${beqt_libs_prefix}/$${beqt_module_build_subdir}$${beqt_project_config_postfix}/ -l$${beqt_module_name}
    } else {
        mac {
            !isEmpty(beqt_standalone_essential):LIBS += -L$${beqt_libs_prefix}/ -l$${beqt_module_name}
            !isEmpty(beqt_system_essential):LIBS += -F$${beqt_libs_prefix}/$${beqt_module_name}.framework/ -framework $${beqt_module_name}
        } else {
            LIBS += -L$${beqt_libs_prefix}/ -l$${beqt_module_name}
        }
    }
    #Headers
    INCLUDEPATH += $${beqt_include_prefix}/$${beqt_module_name}
    DEPENDPATH += $${beqt_include_prefix}/$${beqt_module_name}
    #Export
    export(LIBS)
    export(INCLUDEPATH)
    export(DEPENDPATH)
}

contains(BEQT, core):beqtAddModule(BeQtCore, corelib)
contains(BEQT, network):beqtAddModule(BeQtNetwork, network)
contains(BEQT, widgets):beqtAddModule(BeQtWidgets, widgets)
contains(BEQT, codeeditor):beqtAddModule(BeQtCodeEditor, codeeditor)
