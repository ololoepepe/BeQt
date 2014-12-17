/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the documentation of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#! [0]
TEMPLATE = lib

TARGET = pretexeditormodule

VERSION = 1.0.0
VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0

CONFIG += plugin release

isEmpty(BEQT_PREFIX) {
    mac|unix {
        BEQT_PREFIX=/usr
    } else:win32 {
        BEQT_PREFIX=$$(systemdrive)/PROGRA~1/BeQt
    }
}
include($${BEQT_PREFIX}/share/beqt/depend.pri)

BEQT = core

TRANSLATIONS += \
    translations/myplugin/myplugin_ru.ts

system(lrelease translations/myplugin/myplugin_ru.ts)

HEADERS += \
    myplugin.cpp
#! [0]
