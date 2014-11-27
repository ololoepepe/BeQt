CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = src

contains(BEQT_CONFIG, examples):SUBDIRS += examples

include(translations/translations.pri)
include(include/include.pri)
include(doc/doc.pri)

!contains(BEQT_CONFIG, no_install) {

#Depend
beqtInstallsDepend.files=$${PWD}/depend.pri
beqtInstallsDepend.path=$${BEQT_RESOURCES_INSTALLS_PATH}
INSTALLS += beqtInstallsDepend

} #end !contains(BEQT_CONFIG, no_install)
