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

#ifndef BVERSION_H
#define BVERSION_H

class BVersionPrivate;

class QDataStream;
class QDebug;
class QVariant;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QChar>
#include <QMetaType>

/*============================================================================
================================ BVersion ====================================
============================================================================*/

class B_CORE_EXPORT BVersion : public BBase
{
    B_DECLARE_PRIVATE(BVersion)
public:
    enum Status
    {
        NoStatus = 0,
        PreAlpha,
        Alpha,
        Beta,
        ReleaseCandidate
    };
    enum StatusRepresentation
    {
        ShortLowercase,
        ShortUppercase,
        Full
    };
public:
    static QString statusToString(Status vs, StatusRepresentation r = ShortLowercase);
    static int compare(const BVersion &v1, const BVersion &v2);
    static BVersion fromString(const QString &s, QChar versionSeparator = QChar('.'),
                               QChar statusSeparator = QChar('-'));
public:
    explicit BVersion(qint8 vmajor = -1, qint8 vminor = -1, qint8 vpatch = -1, Status vs = NoStatus,
                      qint8 vextra = -1);
    explicit BVersion(const QString &s, QChar versionSeparator = QChar('.'), QChar statusSeparator = QChar('-'));
    BVersion(const BVersion &other);
    ~BVersion();
protected:
    explicit BVersion(BVersionPrivate &d);
public:
    void setVersion(qint8 vmajor, qint8 vminor = -1, qint8 vpatch = -1, Status vs = NoStatus, qint8 vextra = -1);
    void setVersion(const QString &s, QChar versionSeparator = QChar('.'), QChar statusSeparator = QChar('-'));
    void clear();
    qint8 vmajor() const;
    qint8 vminor() const;
    qint8 vpatch() const;
    Status vstatus() const;
    qint8 vextra() const;
    QString toString(StatusRepresentation r = ShortLowercase, QChar versionSeparator = QChar('.'),
                     QChar statusSeparator = QChar('-')) const;
    int compare(const BVersion &other) const;
    bool isValid() const;
public:
    BVersion &operator =(const BVersion &other);
    bool operator ==(const BVersion &other) const;
    bool operator !=(const BVersion &other) const;
    bool operator <(const BVersion &other) const;
    bool operator >(const BVersion &other) const;
    bool operator <=(const BVersion &other) const;
    bool operator >=(const BVersion &other) const;
    operator QString() const;
    operator QVariant() const;
public:
    friend QDataStream &operator <<(QDataStream &stream, const BVersion &v);
    friend QDataStream &operator >>(QDataStream &stream, BVersion &v);
    friend QDebug operator <<(QDebug dbg, const BVersion &v);
};

Q_DECLARE_METATYPE(BVersion)

#endif // BVERSION_H
