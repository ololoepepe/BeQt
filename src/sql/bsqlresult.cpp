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

#include "bsqlresult.h"

#include <BeQtCore/private/bbase_p.h>

#include <QList>
#include <QObject>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BSqlResultPrivate ===========================
============================================================================*/

class BSqlResultPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSqlResult)
public:
    QSqlError error;
    QVariant insertId;
    bool success;
    BSqlResult::VariantMapList values;
public:
    explicit BSqlResultPrivate(BSqlResult *q);
    ~BSqlResultPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BSqlResultPrivate)
};

/*============================================================================
================================ BSqlResultPrivate ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlResultPrivate::BSqlResultPrivate(BSqlResult *q) :
    BBasePrivate(q)
{
    //
}

BSqlResultPrivate::~BSqlResultPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSqlResultPrivate::init()
{
    success = false;
}

/*============================================================================
================================ BSqlResult ==================================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlResult::BSqlResult(bool success, const QSqlError &error) :
    BBase(*new BSqlResultPrivate(this))
{
    d_func()->init();
    setSuccess(success);
    setLastError(error);
}

BSqlResult::BSqlResult(const QSqlError &error) :
    BBase(*new BSqlResultPrivate(this))
{
    d_func()->init();
    setSuccess(false);
    setLastError(error);
}

BSqlResult::BSqlResult(const BSqlResult &other) :
    BBase(*new BSqlResultPrivate(this))
{
    d_func()->init();
    *this = other;
}

BSqlResult::~BSqlResult()
{
    //
}

/*============================== Public methods ============================*/

QSqlError BSqlResult::lastError() const
{
    return d_func()->error;
}

QVariant BSqlResult::lastInsertId(QVariant::Type t) const
{
    const B_D(BSqlResult);
    if (QVariant::Invalid == t || !d->insertId.isNull() || !d->insertId.canConvert(t))
        return d->insertId;
    QVariant id = d->insertId;
    id.convert(t);
    return id;
}

void BSqlResult::setLastError(const QSqlError &e)
{
    d_func()->error = e;
}

void BSqlResult::setLastInsertId(const QVariant &id)
{
    d_func()->insertId = id;
}

void BSqlResult::setSuccess(bool b)
{
    d_func()->success = b;
}

void BSqlResult::setValues(const VariantMapList &list)
{
    d_func()->values = list;
}

bool BSqlResult::success() const
{
    return d_func()->success;
}

QVariantMap BSqlResult::value() const
{
    return !d_func()->values.isEmpty() ? d_func()->values.first() : QVariantMap();
}

QVariant BSqlResult::value(const QString &key, const QVariant &defValue) const
{
    return (!key.isEmpty() && !d_func()->values.isEmpty()) ? d_func()->values.first().value(key, defValue) : defValue;
}

BSqlResult::VariantMapList BSqlResult::values() const
{
    return d_func()->values;
}

/*============================== Public operators ==========================*/

BSqlResult &BSqlResult::operator =(const BSqlResult &other)
{
    B_D(BSqlResult);
    const BSqlResultPrivate *dd = other.d_func();
    d->success = dd->success;
    d->values = dd->values;
    d->insertId = dd->insertId;
    d->error = dd->error;
    return *this;
}

bool BSqlResult::operator ==(const BSqlResult &other) const
{
    const B_D(BSqlResult);
    const BSqlResultPrivate *dd = other.d_func();
    return d->success == dd->success && d->values == dd->values && d->insertId == dd->insertId
            && d->error.type() == dd->error.type();
}

BSqlResult::operator bool() const
{
    return d_func()->success;
}
