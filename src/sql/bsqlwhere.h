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

#ifndef BSQLWHERE_H
#define BSQLWHERE_H

class BSqlWherePrivate;

#include <BeQtCore/BBase>

#include <QString>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BSqlWhere ===================================
============================================================================*/

class B_SQL_EXPORT BSqlWhere : public BBase
{
    B_DECLARE_PRIVATE(BSqlWhere)
public:
    explicit BSqlWhere(const QString &string, const QVariantMap &values);
    explicit BSqlWhere(const QString &string = QString(), const QString &placeholder1 = QString(),
                       const QVariant &boundValue1 = QVariant(), const QString &placeholder2 = QString(),
                       const QVariant &boundValue2 = QVariant());
    BSqlWhere(const BSqlWhere &other);
    ~BSqlWhere();
public:
    QVariantMap boundValues() const;
    bool isValid() const;
    void setBoundValues(const QVariantMap &values);
    void setBoundValues(const QString &placeholder1, const QVariant &boundValue1,
                        const QString &placeholder2 = QString(), const QVariant &boundValue2 = QVariant());
    void setString(const QString &s);
    QString string() const;
public:
    BSqlWhere &operator =(const BSqlWhere &other);
    bool operator ==(const BSqlWhere &other) const;
};

#endif // BSQLWHERE_H
