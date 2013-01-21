mac {
    #isEmpty(BEQT_PREFIX):BEQT_PREFIX=/Library
    #TODO: Add ability to create bundles
} else:unix:!mac {
    isEmpty(BEQT_PREFIX)BEQT_:PREFIX=/usr
    equals(BEQT_PREFIX, "/")|equals(BEQT_PREFIX, "/usr")|equals(BEQT_PREFIX, "/usr/local") {
        headersInstallsPath=$${BEQT_PREFIX}/include/beqt
        libsInstallsPath=$${BEQT_PREFIX}/lib
        resourcesInstallsPath=$${BEQT_PREFIX}/share/beqt
    } else {
        headersInstallsPath=$${BEQT_PREFIX}/include
        libsInstallsPath=$${BEQT_PREFIX}/lib
        resourcesInstallsPath=$${BEQT_PREFIX}
    }
} else:win32 {
    isEmpty(BEQT_PREFIX):BEQT_PREFIX=$$(systemdrive)/PROGRA~1/BeQt
    headersInstallsPath=$${BEQT_PREFIX}/include
    libsInstallsPath=$${BEQT_PREFIX}/lib
    resourcesInstallsPath=$${BEQT_PREFIX}
}

export(BEQT_PREFIX)
export(headersInstallsPath)
export(libsInstallsPath)
export(resourcesInstallsPath)
