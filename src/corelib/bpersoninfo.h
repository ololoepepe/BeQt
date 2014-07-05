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

#ifndef BPERSONINFO_H
#define BPERSONINFO_H

class BPersonInfoPrivate;

class QDataStream;
class QDebug;
class QVariant;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QMetaType>

/*============================================================================
================================ BPersonInfo =================================
============================================================================*/

class B_CORE_EXPORT BPersonInfo : BBase
{
    B_DECLARE_PRIVATE(BPersonInfo)
public:
    explicit BPersonInfo(const QString &name = QString(), const QString &role = QString(),
                         const QString &site = QString(), const QString &mail = QString(),
                         const QString &image = QString());
    BPersonInfo(const BPersonInfo &other);
    ~BPersonInfo();
protected:
    explicit BPersonInfo(BPersonInfoPrivate &d);
public:
    void setName(const QString &name);
    void setRole(const QString &role);
    void setSite(const QString &site);
    void setMail(const QString &mail);
    void setImage(const QString &image);
    QString name() const;
    QString role() const;
    QString site() const;
    QString mail() const;
    QString image() const;
    bool isValid() const;
public:
    BPersonInfo &operator=(const BPersonInfo &other);
    bool operator==(const BPersonInfo &other) const;
    bool operator!=(const BPersonInfo &other) const;
    operator QVariant() const;
public:
    friend QDataStream &operator<<(QDataStream &stream, const BPersonInfo &info);
    friend QDataStream &operator>>(QDataStream &stream, BPersonInfo &info);
    friend QDebug operator<<(QDebug dbg, const BPersonInfo &info);
};

Q_DECLARE_METATYPE(BPersonInfo)

#endif // BPERSONINFO_H
