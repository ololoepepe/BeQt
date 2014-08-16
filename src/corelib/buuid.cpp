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

#include "buuid.h"

#include "bbase.h"
#include "bbase_p.h"
#include "btexttools.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QUuid>
#include <QtEndian>

/*============================================================================
================================ BUuidPrivate ================================
============================================================================*/

class BUuidPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BUuid)
public:
    QUuid muuid;
public:
    explicit BUuidPrivate(BUuid *q);
    ~BUuidPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BUuidPrivate)
};

/*============================================================================
================================ BUuidPrivate ================================
============================================================================*/

/*============================== Public constructors =======================*/

BUuidPrivate::BUuidPrivate(BUuid *q) :
    BBasePrivate(q)
{
    //
}

BUuidPrivate::~BUuidPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BUuidPrivate::init()
{
    //
}

/*============================================================================
================================ BUuid =======================================
============================================================================*/

/*============================== Public constructors =======================*/

BUuid::BUuid(const QUuid &uuid) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = uuid;
}

BUuid::BUuid(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7,
             uchar b8) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
}

BUuid::BUuid(const QString &text) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(canonicalText(text));
}

BUuid::BUuid(const char *text) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

BUuid::BUuid(const QByteArray &text) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

#if defined(Q_OS_WIN)
BUuid::BUuid(const GUID &guid) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(guid);
}
#endif

BUuid::BUuid(const BUuid &other) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    *this = other;
}

BUuid::~BUuid()
{
    //
}

/*============================== Static public methods =====================*/

QString BUuid::canonicalText(const QString &uuidText)
{
    QUuid uuid(BTextTools::wrapped(uuidText, "{", "}"));
    return !uuid.isNull() ? uuid.toString() : QString();
}

BUuid BUuid::createUuid()
{
    return BUuid(QUuid::createUuid());
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
BUuid BUuid::createUuidV3(const BUuid &ns, const QByteArray &baseData)
{
    return BUuid(QUuid::createUuidV3(ns.d_func()->muuid, baseData));
}

BUuid BUuid::createUuidV3(const BUuid &ns, const QString &baseData)
{
    return BUuid(QUuid::createUuidV3(ns.d_func()->muuid, baseData));
}

BUuid BUuid::createUuidV5(const BUuid &ns, const QByteArray &baseData)
{
    return BUuid(QUuid::createUuidV5(ns.d_func()->muuid, baseData));
}

BUuid BUuid::createUuidV5(const BUuid &ns, const QString &baseData)
{
    return BUuid(QUuid::createUuidV5(ns.d_func()->muuid, baseData));
}
#endif

BUuid BUuid::fromRfc4122(const QByteArray &bytes)
{
    return BUuid(QUuid::fromRfc4122(bytes));
}

QString BUuid::pureText(const QString &uuidText)
{
    return BUuid(canonicalText(uuidText)).toString(true);
}

/*============================== Public methods ============================*/

bool BUuid::isNull() const
{
    return d_func()->muuid.isNull();
}

QByteArray BUuid::toByteArray() const
{
    return d_func()->muuid.toByteArray();
}

QByteArray BUuid::toRfc4122() const
{
    return d_func()->muuid.toRfc4122();
}

QString BUuid::toString(bool pure) const
{
    return pure ? BTextTools::unwrapped(toString(), "{", "}") : d_func()->muuid.toString();
}

QUuid BUuid::toUuid() const
{
    return d_func()->muuid;
}

QUuid::Variant BUuid::variant() const
{
    return d_func()->muuid.variant();
}

QUuid::Version BUuid::version() const
{
    return d_func()->muuid.version();
}

/*============================== Public operators ==========================*/

#if defined(Q_OS_WIN)
BUuid::operator GUID() const
{
    const QUuid &u = d_func()->muuid;
    GUID guid = {u.data1, u.data2, u.data3,
                 {u.data4[0], u.data4[1], u.data4[2], u.data4[3], u.data4[4], u.data4[5], u.data4[6], u.data4[7]}};
    return guid;
}

#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
BUuid::operator QString() const
{
    return toString();
}
#endif

BUuid::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

BUuid &BUuid::operator= (const BUuid &other)
{
    d_func()->muuid = other.d_func()->muuid;
    return *this;
}

bool BUuid::operator!= (const BUuid &other) const
{
    return d_func()->muuid != other.d_func()->muuid;
}

bool BUuid::operator< (const BUuid &other) const
{
    return d_func()->muuid < other.d_func()->muuid;
}

bool BUuid::operator== (const BUuid &other) const
{
    return d_func()->muuid == other.d_func()->muuid;
}

bool BUuid::operator> (const BUuid &other) const
{
    return d_func()->muuid > other.d_func()->muuid;
}

#if defined(Q_OS_WIN)
bool BUuid::operator!= (const GUID &guid) const
{
    return d_func()->muuid != guid;
}

BUuid &BUuid::operator= (const GUID &guid)
{
    d_func()->muuid = guid;
    return *this;
}

bool BUuid::operator== (const GUID &guid) const
{
    return d_func()->muuid == guid;
}
#endif

/*============================== Public friend operators ===================*/

QDataStream &operator<< (QDataStream &s, const BUuid &id)
{
    QByteArray bytes;
    if (s.byteOrder() == QDataStream::BigEndian) {
        bytes = id.toRfc4122();
    } else {
        bytes = QByteArray(16, Qt::Uninitialized);
        uchar *data = reinterpret_cast<uchar *>(bytes.data());
        qToLittleEndian(id.data1, data);
        data += sizeof(quint32);
        qToLittleEndian(id.data2, data);
        data += sizeof(quint16);
        qToLittleEndian(id.data3, data);
        data += sizeof(quint16);
        for (int i = 0; i < 8; ++i) {
            *(data) = id.data4[i];
            data++;
        }
    }
    if (s.writeRawData(bytes.data(), 16) != 16) {
        s.setStatus(QDataStream::WriteFailed);
    }
    return s;
}

QDataStream &operator>> (QDataStream &s, BUuid &id)
{
    QByteArray bytes(16, Qt::Uninitialized);
    if (s.readRawData(bytes.data(), 16) != 16) {
        s.setStatus(QDataStream::ReadPastEnd);
        return s;
    }
    if (s.byteOrder() == QDataStream::BigEndian) {
        id = BUuid::fromRfc4122(bytes);
    } else {
        const uchar *data = reinterpret_cast<const uchar *>(bytes.constData());
        id.data1 = qFromLittleEndian<quint32>(data);
        data += sizeof(quint32);
        id.data2 = qFromLittleEndian<quint16>(data);
        data += sizeof(quint16);
        id.data3 = qFromLittleEndian<quint16>(data);
        data += sizeof(quint16);
        for (int i = 0; i < 8; ++i) {
            id.data4[i] = *(data);
            data++;
        }
    }
    return s;
}

uint qHash(const BUuid &uuid, uint seed)
{
    const QUuid &u = uuid.d_func()->muuid;
    return u.data1 ^ u.data2 ^ (u.data3 << 16)
            ^ ((u.data4[0] << 24) | (u.data4[1] << 16) | (u.data4[2] << 8) | u.data4[3])
            ^ ((u.data4[4] << 24) | (u.data4[5] << 16) | (u.data4[6] << 8) | u.data4[7]) ^ seed;
}

QDebug operator<< (QDebug dbg, const BUuid &id)
{
    dbg.nospace() << "BUuid(" << id.toString() << ')';
    return dbg.space();
}
