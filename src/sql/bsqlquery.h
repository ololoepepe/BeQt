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

#ifndef BSQLQUERY_H
#define BSQLQUERY_H

class BSqlQueryPrivate;

class QSqlDatabase;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QSqlQuery>
#include <QVariantList>

/*============================================================================
================================ BSqlQuery ===================================
============================================================================*/

class B_SQL_EXPORT BSqlQuery : public BBase
{
    B_DECLARE_PRIVATE(BSqlQuery)
public:
    enum BindingType
    {
        NamedBinding,
        PositionalBinding
    };
public:
    explicit BSqlQuery(const QString &queryString = QString(), const QVariantMap &boundValues = QVariantMap());
    explicit BSqlQuery(const QString &queryString, const QVariantList &boundValues);
    BSqlQuery(const BSqlQuery &other);
    ~BSqlQuery();
public:
    void setQueryString(const QString &qs);
    void addBindValue(const QVariant &val);
    void bindValue(const QString &placeholder, const QVariant &val);
    void bindValue(int pos, const QVariant &val);
    void addBoundValues(const QVariantMap &vals);
    void setBoundValues(const QVariantMap &vals);
    void setBoundValues(const QVariantList &vals);
    void setForwardOnly(bool forward);
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);
    const QString queryString() const;
    QVariant boundValue(const QString &placeholder) const;
    QVariant boundValue(int pos) const;
    QVariantMap boundValues() const;
    BindingType bindingType() const;
    bool isForwardOnly() const;
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
    bool isValid() const;
    QSqlQuery *createQuery(const QSqlDatabase &db) const;
public:
    BSqlQuery &operator =(const BSqlQuery &other);
    bool operator ==(const BSqlQuery &other) const;
};

#endif // BSQLQUERY_H
