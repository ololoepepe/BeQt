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

#include "btextmatchlist.h"

#include <QStringList>

/*============================================================================
================================ BTextMatchList ==============================
============================================================================*/

/*!
\class BTextMatchList
\inmodule BeQtCore
\brief The BTextMatchList class provides the results of a matching a QRegExp against a QString.

A BTextMatchList object can be obtained by calling the BTextTools::match() function.

BTextMatchList inherits from QList<BTextMatchList>. Like QList, BTextMatchList is implicitly shared. It provides fast
index-based access as well as fast insertions and removals. Passing string lists as value parameters is both fast and
safe.

All of QList's functionality also applies to BTextMatchList. For example, you can use isEmpty() to test whether the
list is empty, and you can call functions like append(), prepend(), insert(), replace(), removeAll(), removeAt(),
removeFirst(), removeLast(), and removeOne() to modify a BTextMatchList.

See BTextTools for details.

\sa BTextMatch
*/

/*============================== Public methods ============================*/

/*!
Returns the matched texts of all BTextMatchList object elements.

\sa BTextMatch::text()
*/

QStringList BTextMatchList::texts() const
{
    QStringList list;
    foreach (const BTextMatch r, *this)
        list << r.text();
    return list;
}

/*============================== Public operators ==========================*/

/*!
Returns the matched texts of all BTextMatchList object elements.

\sa texts(), BTextMatch::text()
*/

BTextMatchList::operator QStringList() const
{
    return texts();
}
