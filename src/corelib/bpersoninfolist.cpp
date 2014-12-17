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

#include "bpersoninfolist.h"

#include "bpersoninfo.h"

#include <QDataStream>
#include <QList>
#include <QVariant>
#include <QVariantList>

/*============================================================================
================================ BPersonInfoList =============================
============================================================================*/

/*!
\class BPersonInfoList
\inmodule BeQtCore
\brief The BPersonInfoList class provides a list of BPersonInfo.

BPersonInfoList inherits from QList<BPersonInfo>. Like QList, BPersonInfoList is implicitly shared. It provides fast
index-based access as well as fast insertions and removals. Passing string lists as value parameters is both fast and
safe.

All of QList's functionality also applies to BPersonInfoList. For example, you can use isEmpty() to test whether the
list is empty, and you can call functions like append(), prepend(), insert(), replace(), removeAll(), removeAt(),
removeFirst(), removeLast(), and removeOne() to modify a BPersonInfoList.

In addition, BPersonInfoList provides an operator for storing it in a QVariant, and operators for (de)serializing it to
and from data streams.

\sa BPersonInfo, BPersonInfoProvider
*/

/*============================== Public operators ==========================*/

/*!
Stores the person info list in a QVariant. Returns that QVariant.
*/

BPersonInfoList::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

/*!
\relates BPersonInfoList
Writes the person info list \a list to the data stream \a stream.

Returns a reference to the stream.
*/

QDataStream &operator <<(QDataStream &stream, const BPersonInfoList &list)
{
    QVariantList vlist;
    foreach (const BPersonInfo &info, list)
        vlist << info;
    stream << vlist;
    return stream;
}

/*!
\relates BPersonInfoList
Reads a person info list from the data stream \a stream into \a list.

Returns a reference to the stream.
*/

QDataStream &operator >>(QDataStream &stream, BPersonInfoList &list)
{
    list.clear();
    QVariantList vlist;
    stream >> vlist;
    foreach (const QVariant &v, vlist)
        list << v.value<BPersonInfo>();
    return stream;
}
