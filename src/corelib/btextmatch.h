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

#ifndef BTEXTMATCH_H
#define BTEXTMATCH_H

class BTextMatchPrivate;

class BTextMatchList;

class QDebug;
class QRegExp;
class QString;

#include "bbase.h"
#include "btexttools.h"

/*============================================================================
================================ BTextMatch ==================================
============================================================================*/

class B_CORE_EXPORT BTextMatch : public BBase
{
    B_DECLARE_PRIVATE(BTextMatch)
public:
    explicit BTextMatch();
    BTextMatch(const BTextMatch &other);
    ~BTextMatch();
private:
    explicit BTextMatch(const QString *const text, int position, int length);
public:
    bool isValid() const;
    int length() const;
    int position() const;
    QString text() const;
public:
    BTextMatch &operator =(const BTextMatch &other);
    bool operator ==(const BTextMatch &other) const;
    operator QString() const;
public:
    B_CORE_EXPORT friend BTextMatchList BTextTools::match(const QString &text, const QRegExp &what,
                                                          const QRegExp &prefixedBy, const QRegExp &postfixedBy);
public:
    friend QDebug operator <<(QDebug dbg, const BTextMatch &tm);
    //This class must NOT be serialized because it only points to a string
};

#endif // BTEXTMATCH_H
