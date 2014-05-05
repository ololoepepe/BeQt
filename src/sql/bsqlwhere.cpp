/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtSql module of the BeQt library.
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

#include "bsqlwhere.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QString>

/*============================================================================
================================ BSqlWherePrivate ============================
============================================================================*/

class BSqlWherePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSqlWhere)
public:
    explicit BSqlWherePrivate(BSqlWhere *q);
    ~BSqlWherePrivate();
public:
    void init();
public:
    QString string;
    QVariantMap values;
private:
    Q_DISABLE_COPY(BSqlWherePrivate)
};

/*============================================================================
================================ BSqlWherePrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlWherePrivate::BSqlWherePrivate(BSqlWhere *q) :
    BBasePrivate(q)
{
    //
}

BSqlWherePrivate::~BSqlWherePrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSqlWherePrivate::init()
{
    //
}

/*============================================================================
================================ BSqlWhere ===================================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlWhere::BSqlWhere(const QString &string, const QVariantMap &values) :
    BBase(*new BSqlWherePrivate(this))
{
    d_func()->init();
    setString(string);
    setBoundValues(values);
}

BSqlWhere::BSqlWhere(const QString &string, const QString &placeholder1, const QVariant &boundValue1,
                     const QString &placeholder2, const QVariant &boundValue2) :
    BBase(*new BSqlWherePrivate(this))
{
    d_func()->init();
    setString(string);
    setBoundValues(placeholder1, boundValue1, placeholder2, boundValue2);
}

BSqlWhere::BSqlWhere(const BSqlWhere &other) :
    BBase(*new BSqlWherePrivate(this))
{
    d_func()->init();
    *this = other;
}

BSqlWhere::~BSqlWhere()
{
    //
}

/*============================== Public methods ============================*/

void BSqlWhere::setString(const QString &s)
{
    d_func()->string = s;
}

void BSqlWhere::setBoundValues(const QVariantMap &values)
{
    d_func()->values = values;
}

void BSqlWhere::setBoundValues(const QString &placeholder1, const QVariant &boundValue1, const QString &placeholder2,
                               const QVariant &boundValue2)
{
    QVariantMap m;
    if (!placeholder1.isEmpty() && !boundValue1.isNull())
        m.insert(placeholder1, boundValue1);
    if (!placeholder2.isEmpty() && !boundValue2.isNull())
        m.insert(placeholder2, boundValue2);
    setBoundValues(m);
}

QString BSqlWhere::string() const
{
    return d_func()->string;
}

QVariantMap BSqlWhere::boundValues() const
{
    return d_func()->values;
}

bool BSqlWhere::isValid() const
{
    return !d_func()->string.isEmpty();
}

/*============================== Public operators ==========================*/

BSqlWhere &BSqlWhere::operator =(const BSqlWhere &other)
{
    B_D(BSqlWhere);
    const BSqlWherePrivate *dd = other.d_func();
    d->string = dd->string;
    d->values = dd->values;
    return *this;
}

bool BSqlWhere::operator ==(const BSqlWhere &other) const
{
    const B_D(BSqlWhere);
    const BSqlWherePrivate *dd = other.d_func();
    return d->string == dd->string && d->values == dd->values;
}
