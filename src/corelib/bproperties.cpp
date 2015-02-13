/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#include "bproperties.h"

#include <QMap>
#include <QString>
#include <QVariant>

/*============================================================================
================================ BProperties =================================
============================================================================*/

/*!
\class BProperties
\inmodule BeQtCore
\brief The BProperties class provides a properties map.

BProperties inherits from QMap<QString, QString>. In addition, BProperties provides an operator for storing it in a
QVariant.

Example of a .properties file:

\snippet src/corelib/bproperties.properties 0

For details about the .properties format, see \l https://en.wikipedia.org/wiki/.properties

\sa BeQt::propertiesFromString(), BeQt::propertiesToString()
*/

/*============================== Public operators ==========================*/

/*!
Stores the properties object in a QVariant. Returns that QVariant.
*/

BProperties::operator QVariant() const
{
    return QVariant::fromValue(*this);
}
