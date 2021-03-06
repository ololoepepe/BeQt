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

#ifndef BPERSONINFOLIST_H
#define BPERSONINFOLIST_H

class BPersonInfoPrivate;

class QDataStream;
class QVariant;

#include "bpersoninfo.h"

#include <QList>
#include <QMetaType>

/*============================================================================
================================ BPersonInfoList =============================
============================================================================*/

class B_CORE_EXPORT BPersonInfoList : public QList<BPersonInfo>
{
public:
    operator QVariant() const;
public:
    B_CORE_EXPORT friend QDataStream &operator <<(QDataStream &stream, const BPersonInfoList &list);
    B_CORE_EXPORT friend QDataStream &operator >>(QDataStream &stream, BPersonInfoList &list);
};

Q_DECLARE_METATYPE(BPersonInfoList)

#endif // BPERSONINFOLIST_H
