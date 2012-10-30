CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = src examples

include(pri/install_headers.pri)
include(pri/install_images.pri)
include(pri/install_translations.pri)
