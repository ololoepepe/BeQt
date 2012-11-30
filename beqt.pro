CONFIG += ordered
TEMPLATE = subdirs

SUBDIRS = src

contains(CONFIG, beqt_examples):SUBDIRS += examples

include(installs.pri)
