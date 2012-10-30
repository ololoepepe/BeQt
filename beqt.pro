CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = src examples

include(include/install.pri)
include(images/install.pri)
include(translations/install.pri)
