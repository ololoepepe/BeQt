mac {
    #isEmpty(PREFIX):PREFIX=/Library
    #TODO: Add ability to create bundles
} else:unix:!mac {
    isEmpty(PREFIX):PREFIX=/usr
    equals(PREFIX, "/")|equals(PREFIX, "/usr")|equals(PREFIX, "/usr/local") {
        headersInstallsPath=$${PREFIX}/include/beqt
        libsInstallsPath=$${PREFIX}/lib
        resourcesInstallsPath=$${PREFIX}/share/beqt
    } else {
        headersInstallsPath=$${PREFIX}/include
        libsInstallsPath=$${PREFIX}/lib
        resourcesInstallsPath=$${PREFIX}
    }
} else:win32 {
    isEmpty(PREFIX):PREFIX=$$(systemdrive)/PROGRA~1/BeQt
    headersInstallsPath=$${PREFIX}/include
    libsInstallsPath=$${PREFIX}/lib
    resourcesInstallsPath=$${PREFIX}
}

export(PREFIX)
export(headersInstallsPath)
export(libsInstallsPath)
export(resourcesInstallsPath)
