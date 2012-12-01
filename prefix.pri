mac {
    #isEmpty(PREFIX):PREFIX=/Library
    #TODO: Add ability to create bundles
} else:unix:!mac {
    isEmpty(PREFIX):PREFIX=/usr
    headersInstallsPath=$${PREFIX}/include/beqt
    libsInstallsPath=$${PREFIX}/lib
    rescourcesInstallsPath=$${PREFIX}/share/beqt/resources
} else:win32 {
    isEmpty(PREFIX):PREFIX=$$(systemdrive)/PROGRA~1/BeQt
    headersInstallsPath=$${PREFIX}/include
    libsInstallsPath=$${PREFIX}/lib
    rescourcesInstallsPath=$${PREFIX}
}

export(PREFIX)
export(headersInstallsPath)
export(libsInstallsPath)
export(resourcesInstallsPath)
