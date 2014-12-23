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

#include "btextmatch.h"

#include "bbase.h"
#include "bbase_p.h"

#include <QDebug>
#include <QObject>
#include <QString>

/*============================================================================
================================ BTextMatchPrivate ===========================
============================================================================*/

class BTextMatchPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTextMatch)
public:
    int len;
    int pos;
    const QString *txt;
public:
    explicit BTextMatchPrivate(BTextMatch *q);
    ~BTextMatchPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BTextMatchPrivate)
};

/*============================================================================
================================ BTextMatchPrivate ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BTextMatchPrivate::BTextMatchPrivate(BTextMatch *q) :
    BBasePrivate(q)
{
    //
}

BTextMatchPrivate::~BTextMatchPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTextMatchPrivate::init()
{
    txt = 0;
    pos = -1;
    len = -1;
}

/*============================================================================
================================ BTextMatch ==================================
============================================================================*/

/*!
\class BTextMatch
\inmodule BeQtCore
\brief The BTextMatch class provides the results of a matching a QRegExp against a QString.

A BTextMatch object can be obtained by calling the BTextTools::match() function.

\warning A BTextMatch object is only valid while the QString against which it is matched exists. Therefore, if the
string is deleted, the corresponding BTextMatch object becomes invalid and must not be used anymore. The validity
described here has nothing to do with the isValid() method.

See BTextTools for details.
*/

/*============================== Public constructors =======================*/

/*!
Constructs an invalid BTextMatch object, which does not reference any string.
*/

BTextMatch::BTextMatch() :
    BBase(*new BTextMatchPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a copy of \a other.
*/

BTextMatch::BTextMatch(const BTextMatch &other) :
    BBase(*new BTextMatchPrivate(this))
{
    d_func()->init();
    *this = other;
}

/*!
Destroys the object, deleting the associated data object. The referenced QString (if any) is not affected.
*/

BTextMatch::~BTextMatch()
{
    //
}

/*============================== Private constructors ======================*/

/*!
Constructs a BTextMatch object referencing a string \a text with the specified \a position and \a length.
*/

BTextMatch::BTextMatch(const QString *const text, int position, int length) :
    BBase(*new BTextMatchPrivate(this))
{
    B_D(BTextMatch);
    d->init();
    d->txt = text;
    d->pos = position;
    d->len = length;
}

/*============================== Public methods ============================*/

/*!
Returns true if the object references a QString and the referenced position is greater or equal to 0, and the length is
greater than 0. Ohterwise returns false.
*/

bool BTextMatch::isValid() const
{
    const B_D(BTextMatch);
    return d->txt && d->pos >= 0 && d->len >= 0;
}

/*!
Returns the length of the matching substring of the referenced string.
*/

int BTextMatch::length() const
{
    return d_func()->len;
}

/*!
Returns the position in the matching substring of the referenced string.
*/

int BTextMatch::position() const
{
    return d_func()->pos;
}

/*!
Returns the matched substring.
*/

QString BTextMatch::text() const
{
    const B_D(BTextMatch);
    return d->txt ? d->txt->mid(d->pos, d->len) : QString();
}

/*============================== Public operators ==========================*/

/*!
Assigns \a other to this BTextMatch object and returns a reference to this BTextMatch object.
*/

BTextMatch &BTextMatch::operator =(const BTextMatch &other)
{
    B_D(BTextMatch);
    const BTextMatchPrivate *dd = other.d_func();
    d->txt = dd->txt;
    d->pos = dd->pos;
    d->len = dd->len;
    return *this;
}

/*!
Returns true if this BTextMatch object is equal to BTextMatch object \a other; otherwise returns false.

The BTextMatch objects are considered equal if they reference the same string and their positions and lengths are also
equal.
*/

bool BTextMatch::operator ==(const BTextMatch &other) const
{
    const B_D(BTextMatch);
    const BTextMatchPrivate *dd = other.d_func();
    return d->txt == dd->txt && d->pos == dd->pos && d->len == dd->len;
}

/*!
Returns the matched substring.

\sa text()
*/

BTextMatch::operator QString() const
{
    return text();
}

/*============================== Public friend operators ===================*/

/*!
\relates BTextMatch
Writes the BTextMatch object \a tm to the output stream for debugging information \a dbg and returns a reference to the
stream.
*/

QDebug operator <<(QDebug dbg, const BTextMatch &tm)
{
    dbg.nospace() << "BTextMatch(" << tm.text() << ")";
    return dbg.space();
}
