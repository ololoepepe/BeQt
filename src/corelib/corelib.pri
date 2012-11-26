HEADERS += \
    $${PWD}/bbase.h \
    $${PWD}/bbase_p.h \
    $${PWD}/bcoreapplication.h \
    $${PWD}/bcoreapplication_p.h \
    $${PWD}/bdirtools.h \
    $${PWD}/bglobal.h \
    $${PWD}/bnamespace.h \
    $${PWD}/bpersoninfoprovider.h \
    $${PWD}/bpersoninfoprovider_p.h \
    $${PWD}/bplugininterface.h \
    $${PWD}/bpluginwrapper.h \
    $${PWD}/bpluginwrapper_p.h \
    $${PWD}/bterminaliohandler.h \
    $${PWD}/bterminaliohandler_p.h \
    $${PWD}/btranslator.h \
    $${PWD}/btranslator_p.h

SOURCES += \
    $${PWD}/bbase.cpp \
    $${PWD}/bcoreapplication.cpp \
    $${PWD}/bdirtools.cpp \
    $${PWD}/bglobal.cpp \
    $${PWD}/bpersoninfoprovider.cpp \
    $${PWD}/bpluginwrapper.cpp \
    $${PWD}/bterminaliohandler.cpp \
    $${PWD}/btranslator.cpp

contains(CONFIG, beqt_builtin_resources):RESOURCES += $${PWD}/../../beqt.qrc
