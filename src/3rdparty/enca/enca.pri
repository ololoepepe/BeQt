!isEmpty(ENCA_PREFIX) {
    INCLUDEPATH *= $${ENCA_PREFIX}
    DEPENDPATH *= $${ENCA_PREFIX}
    INCLUDEPATH *= $${ENCA_PREFIX}/lib
    DEPENDPATH *= $${ENCA_PREFIX}/lib
    INCLUDEPATH *= $${ENCA_PREFIX}/data
    DEPENDPATH *= $${ENCA_PREFIX}/data
    INCLUDEPATH *= $${ENCA_PREFIX}/tools
    DEPENDPATH *= $${ENCA_PREFIX}/tools
    LIBS *= -L$${ENCA_PREFIX}/lib/ -lenca
} else {
    DEFINES += BUILDING_LIBENCA

    INCLUDEPATH *= $${PWD}
    DEPENDPATH *= $${PWD}

    HEADERS += $$files($${PWD}/lib/*.h)
    HEADERS += $$files($${PWD}/data/bulgarian/*.h)
    HEADERS += $$files($${PWD}/data/chinese/*.h)
    HEADERS += $$files($${PWD}/data/croatian/*.h)
    HEADERS += $$files($${PWD}/data/czech/*.h)
    HEADERS += $$files($${PWD}/data/estonian/*.h)
    HEADERS += $$files($${PWD}/data/hungarian/*.h)
    HEADERS += $$files($${PWD}/data/latvian/*.h)
    HEADERS += $$files($${PWD}/data/lithuanian/*.h)
    HEADERS += $$files($${PWD}/data/polish/*.h)
    HEADERS += $$files($${PWD}/data/russian/*.h)
    HEADERS += $$files($${PWD}/data/slovak/*.h)
    HEADERS += $$files($${PWD}/data/slovene/*.h)
    HEADERS += $$files($${PWD}/data/ukrainian/*.h)
    HEADERS += $$files($${PWD}/tools/*.h)

    SOURCES += $$files($${PWD}/lib/*.c)
}
