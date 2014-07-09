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

/*============================== Public operators ==========================*/

BPersonInfoList::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BPersonInfoList &list)
{
    QVariantList vlist;
    foreach (const BPersonInfo &info, list)
        vlist << info;
    stream << vlist;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BPersonInfoList &list)
{
    list.clear();
    QVariantList vlist;
    stream >> vlist;
    foreach (const QVariant &v, vlist)
        list << v.value<BPersonInfo>();
    return stream;
}
