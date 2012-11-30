HEADERS += \
    $${PWD}/baboutdialog.h \
    $${PWD}/baboutdialog_p.h \
    $${PWD}/babstractsettingstab.h \
    $${PWD}/babstractterminaldriver.h \
    $${PWD}/bapplication.h \
    $${PWD}/bapplication_p.h \
    $${PWD}/bflowlayout.h \
    $${PWD}/bguiplugininterface.h \
    $${PWD}/bhelpbrowser.h \
    $${PWD}/bhelpbrowser_p.h \
    $${PWD}/blocalecombobox.h \
    $${PWD}/blocalterminaldriver.h \
    $${PWD}/bpasswordwidget_p.h \
    $${PWD}/bpasswordwidget.h \
    $${PWD}/bplaintextedit.h \
    $${PWD}/bplaintextedit_p.h \
    $${PWD}/bpluginssettingstab.h \
    $${PWD}/bpluginssettingstab_p.h \
    $${PWD}/bsettingsdialog.h \
    $${PWD}/bterminalwidget.h \
    $${PWD}/bterminalwidget_p.h

SOURCES += \
    $${PWD}/baboutdialog.cpp \
    $${PWD}/babstractsettingstab.cpp \
    $${PWD}/babstractterminaldriver.cpp \
    $${PWD}/bapplication.cpp \
    $${PWD}/bflowlayout.cpp \
    $${PWD}/bhelpbrowser.cpp \
    $${PWD}/blocalecombobox.cpp \
    $${PWD}/blocalterminaldriver.cpp \
    $${PWD}/bpasswordwidget.cpp \
    $${PWD}/bplaintextedit.cpp \
    $${PWD}/bpluginssettingstab.cpp \
    $${PWD}/bsettingsdialog.cpp \
    $${PWD}/bterminalwidget.cpp

contains(CONFIG, beqt_builtin_resources):RESOURCES += $${PWD}/beqtwidgets.qrc
