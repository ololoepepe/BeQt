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

#ifndef BUUID_H
#define BUUID_H

class BUuidPrivate;

class QByteArray;
class QDataStream;
class QDebug;
class QString;
class QVariant;

#include "bbase.h"

#include <QUuid>
#include <QMetaType>

#if defined(Q_OS_WIN)
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    ulong   Data1;
    ushort  Data2;
    ushort  Data3;
    uchar   Data4[8];
} GUID, *REFGUID, *LPGUID;
#endif
#endif

/*============================================================================
================================ BUuid =======================================
============================================================================*/

class B_CORE_EXPORT BUuid : public BBase
{
    B_DECLARE_PRIVATE(BUuid)
public:
    uint &data1;
    ushort &data2;
    ushort &data3;
    uchar (&data4)[8];
public:
    explicit BUuid(const QUuid &uuid = QUuid());
    explicit BUuid(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7,
                   uchar b8);
    explicit BUuid(const QString &text);
    explicit BUuid(const char *text);
    explicit BUuid(const QByteArray &text);
#if defined(Q_OS_WIN)
    explicit BUuid(const GUID &guid);
#endif
    BUuid(const BUuid &other);
    ~BUuid();
public:
    static QString canonicalText(const QString &uuidText);
    static BUuid createUuid();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static BUuid createUuidV3(const BUuid &ns, const QByteArray &baseData);
    static BUuid createUuidV3(const BUuid &ns, const QString &baseData);
    static BUuid createUuidV5(const BUuid &ns, const QByteArray &baseData);
    static BUuid createUuidV5(const BUuid &ns, const QString &baseData);
#endif
    static BUuid fromRfc4122(const QByteArray & bytes);
    static QString pureText(const QString &uuidText);
public:
    bool isNull() const;
    QByteArray toByteArray() const;
    QByteArray toRfc4122() const;
    QString toString(bool pure = false) const;
    QUuid toUuid() const;
    QUuid::Variant variant() const;
    QUuid::Version version() const;
public:
#if defined(Q_OS_WIN)
    operator GUID() const;
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    operator QString() const;
    operator QVariant() const;
#endif
    bool operator!= (const BUuid &other) const;
    BUuid &operator= (const BUuid &other);
    bool operator< (const BUuid &other) const;
    bool operator== (const BUuid &other) const;
    bool operator> (const BUuid &other) const;
#if defined(Q_OS_WIN)
    bool operator!= (const GUID &guid) const;
    BUuid &operator= (const GUID &guid);
    bool operator== (const GUID &guid) const;
#endif
public:
    B_CORE_EXPORT friend QDataStream &operator<< (QDataStream &s, const BUuid &id);
    B_CORE_EXPORT friend QDataStream &operator>> (QDataStream &s, BUuid &id);
    B_CORE_EXPORT friend uint qHash(const BUuid &uuid, uint seed = 0);
    B_CORE_EXPORT friend QDebug operator<< (QDebug dbg, const BUuid &id);
};

Q_DECLARE_METATYPE(BUuid)

#endif // BUUID_H
