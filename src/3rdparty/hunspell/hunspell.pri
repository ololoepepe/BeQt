!isEmpty(HUNSPELL_PREFIX) {
    INCLUDEPATH *= $${HUNSPELL_PREFIX}/include
    DEPENDPATH *= $${HUNSPELL_PREFIX}/include
    LIBS *= -L$${HUNSPELL_PREFIX}/lib/ -lhunspell
} else {
    DEFINES += BUILDING_LIBHUNSPELL

    INCLUDEPATH *= $${PWD}
    DEPENDPATH *= $${PWD}

    HEADERS += $$files($${PWD}/*.hxx)
    HEADERS += \
    $${PWD}/config.h \
    $${PWD}/hunvisapi.h

    SOURCES += $$files($${PWD}/*.cxx)
}
