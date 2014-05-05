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

#ifndef BSQLRESULT_H
#define BSQLRESULT_H

class BSqlResultPrivate;

class QString;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QVariant>
#include <QVariantMap>
#include <QSqlError>
#include <QList>

/*============================================================================
================================ BSqlResult ==================================
============================================================================*/

class B_SQL_EXPORT BSqlResult : public BBase
{
    B_DECLARE_PRIVATE(BSqlResult)
public:
    typedef QList<QVariantMap> BVariantMapList;
public:
    explicit BSqlResult(bool success = false, const QSqlError &error = QSqlError());
    explicit BSqlResult(const QSqlError &error);
    BSqlResult(const BSqlResult &other);
    ~BSqlResult();
public:
    void setSuccess(bool b);
    void setValues(const BVariantMapList &list);
    void setLastInsertId(const QVariant &id);
    void setLastError(const QSqlError &e);
    bool success() const;
    BVariantMapList values() const;
    QVariantMap value() const;
    QVariant value(const QString &key, const QVariant &defValue = QVariant()) const;
    QVariant lastInsertId(QVariant::Type t = QVariant::Invalid) const;
    QSqlError lastError() const;
public:
    BSqlResult &operator =(const BSqlResult &other);
    bool operator ==(const BSqlResult &other) const;
    operator bool() const;
};

#endif // BSQLRESULT_H
