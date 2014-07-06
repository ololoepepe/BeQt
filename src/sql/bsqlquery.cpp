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

#include "bsqlquery.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

/*============================================================================
================================ BSqlQueryPrivate ============================
============================================================================*/

class BSqlQueryPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSqlQuery)
public:

    QVariantMap bv;
    bool forward;
    QSql::NumericalPrecisionPolicy precisionPolicy;
    QString qs;
    BSqlQuery::BindingType type;
public:
    explicit BSqlQueryPrivate(BSqlQuery *q);
    ~BSqlQueryPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BSqlQueryPrivate)
};

/*============================================================================
================================ BSqlQueryPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlQueryPrivate::BSqlQueryPrivate(BSqlQuery *q) :
    BBasePrivate(q)
{
    //
}

BSqlQueryPrivate::~BSqlQueryPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSqlQueryPrivate::init()
{
    type = BSqlQuery::NamedBinding;
    forward = false;
    precisionPolicy = QSql::LowPrecisionDouble;
}

/*============================================================================
================================ BSqlDatabase ================================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlQuery::BSqlQuery(const QString &queryString, const QVariantMap &boundValues) :
    BBase(*new BSqlQueryPrivate(this))
{
    d_func()->init();
    setQueryString(queryString);
    setBoundValues(boundValues);
}

BSqlQuery::BSqlQuery(const QString &queryString, const QVariantList &boundValues) :
    BBase(*new BSqlQueryPrivate(this))
{
    d_func()->init();
    setQueryString(queryString);
    setBoundValues(boundValues);
}

BSqlQuery::BSqlQuery(const BSqlQuery &other) :
    BBase(*new BSqlQueryPrivate(this))
{
    d_func()->init();
    *this = other;
}

BSqlQuery::~BSqlQuery()
{
    //
}

/*============================== Public methods ============================*/

void BSqlQuery::addBindValue(const QVariant &val)
{
    bindValue(d_func()->bv.size(), val);
}

void BSqlQuery::addBoundValues(const QVariantMap &vals)
{
    if (vals.isEmpty())
        return;
    B_D(BSqlQuery);
    if (NamedBinding != d->type) {
        d->type = NamedBinding;
        d->bv.clear();
    }
    d->bv.unite(vals);
}

BSqlQuery::BindingType BSqlQuery::bindingType() const
{
    return d_func()->type;
}

void BSqlQuery::bindValue(const QString &placeholder, const QVariant &val)
{
    if (placeholder.isEmpty())
        return;
    B_D(BSqlQuery);
    if (PositionalBinding == d->type) {
        d->bv.clear();
        d->type = NamedBinding;
    }
    d->bv.insert(placeholder, val);
}

void BSqlQuery::bindValue(int pos, const QVariant &val)
{
    if (pos < 0)
        return;
    B_D(BSqlQuery);
    if (NamedBinding == d->type) {
        d->bv.clear();
        d->type = PositionalBinding;
    }
    d->bv.insert(QString::number(pos), val);
}

QVariant BSqlQuery::boundValue(const QString &placeholder) const
{
    if (placeholder.isEmpty() || NamedBinding != d_func()->type)
        return QVariant();
    return d_func()->bv.value(placeholder);
}

QVariant BSqlQuery::boundValue(int pos) const
{
    if (pos < 0 || PositionalBinding != d_func()->type)
        return QVariant();
    return d_func()->bv.value(QString::number(pos));
}

QVariantMap BSqlQuery::boundValues() const
{
    return d_func()->bv;
}

QSqlQuery *BSqlQuery::createQuery(const QSqlDatabase &db) const
{
    if (!isValid() || !db.isValid())
        return 0;
    const B_D(BSqlQuery);
    QSqlQuery *q = new QSqlQuery(db);
    q->prepare(d->qs);
    if (bindingType() == NamedBinding) {
        foreach (const QString &p, d->bv.keys())
            q->bindValue(p, d->bv.value(p), QSql::InOut);
    } else {
        foreach (const QVariant &v, d->bv.values())
            q->addBindValue(v, QSql::InOut);
    }
    return q;
}

bool BSqlQuery::isForwardOnly() const
{
    return d_func()->forward;
}

bool BSqlQuery::isValid() const
{
    return !d_func()->qs.isEmpty();
}

QSql::NumericalPrecisionPolicy BSqlQuery::numericalPrecisionPolicy() const
{
    return d_func()->precisionPolicy;
}

QString BSqlQuery::queryString() const
{
    return d_func()->qs;
}

void BSqlQuery::setBoundValues(const QVariantMap &vals)
{
    if (vals.isEmpty())
        return;
    B_D(BSqlQuery);
    d->type = NamedBinding;
    d->bv = vals;
}

void BSqlQuery::setBoundValues(const QVariantList &vals)
{
    if (vals.isEmpty())
        return;
    B_D(BSqlQuery);
    d->type = PositionalBinding;
    d->bv.clear();
    foreach (const QVariant &v, vals)
        addBindValue(v);
}

void BSqlQuery::setForwardOnly(bool forward)
{
    d_func()->forward = forward;
}

void BSqlQuery::setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy)
{
    d_func()->precisionPolicy = precisionPolicy;
}

void BSqlQuery::setQueryString(const QString &qs)
{
    d_func()->qs = qs;
}

/*============================== Public operators ==========================*/

BSqlQuery &BSqlQuery::operator =(const BSqlQuery &other)
{
    B_D(BSqlQuery);
    const BSqlQueryPrivate *dd = other.d_func();
    d->qs = dd->qs;
    d->bv = dd->bv;
    d->type = dd->type;
    d->forward = dd->forward;
    d->precisionPolicy = dd->precisionPolicy;
    return *this;
}

bool BSqlQuery::operator ==(const BSqlQuery &other) const
{
    const B_D(BSqlQuery);
    const BSqlQueryPrivate *dd = other.d_func();
    return d->qs == dd->qs && d->bv == dd->bv && d->type == dd->type && d->forward == dd->forward
            && d->precisionPolicy == dd->precisionPolicy;
}
