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

/*!
\class BUuid
\inmodule BeQtCore
\brief The BUuid class stores a Universally Unique Identifier (UUID).

This class is a wrapper over the QUuid class. It is needed to fix incompatible (de)serialization of QUuid between
different versions of Qt (Qt4 and Qt5). In Qt4, QUuid is \e not registered as an internal type in meta-object system,
while in Qt5 it is. That's why QUuid serialized in Qt5-based application can not be deserialized in Qt4-based
application. BUuid wraps QUuid, and provides the same (de)serialization for both Qt4- and Qt5-based applications.

For details, please refer to QUuid documentation.
*/

/*============================== Public constructors =======================*/

/*!
Creates a UUID, initializing it with \a uuid.
*/

BUuid::BUuid(const QUuid &uuid) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = uuid;
}


/*!
Creates a UUID with the value specified by the parameters, \a l, \a w1, \a w2, \a b1, \a b2, \a b3, \a b4, \a b5,
\a b6, \a b7, \a b8.
*/

BUuid::BUuid(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7,
             uchar b8) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
}

/*!
Creates a BUuid object from the string \a text, which must be formatted as five hex fields separated by '-', e.g.,
"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" where 'x' is a hex digit. The curly braces shown here are optional, but it is
normal to include them. If the conversion fails, a null UUID is created.
*/

BUuid::BUuid(const QString &text) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(canonicalText(text));
}

/*!
Creates a QUuid object from the char array \a text, which must be formatted as five hex fields separated by '-', e.g.,
"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" where 'x' is a hex digit. The curly braces shown here are optional, but it is
normal to include them. If the conversion fails, a null UUID is created.
*/

BUuid::BUuid(const char *text) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

/*!
Creates a QUuid object from the QByteArray \a text, which must be formatted as five hex fields separated by '-', e.g.,
"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" where 'x' is a hex digit. The curly braces shown here are optional, but it is
normal to include them. If the conversion fails, a null UUID is created.
*/

BUuid::BUuid(const QByteArray &text) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(text);
}

/*!
Casts a Windows \a guid to a Qt BUuid.

\note This function is only available on Windows.
*/

#if defined(Q_OS_WIN)
BUuid::BUuid(const GUID &guid) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    d_func()->muuid = QUuid(guid);
}
#endif

/*!
Constructs a copy of \a other.
*/

BUuid::BUuid(const BUuid &other) :
    BBase(*new BUuidPrivate(this)), data1(d_func()->muuid.data1), data2(d_func()->muuid.data2),
    data3(d_func()->muuid.data3), data4(d_func()->muuid.data4)
{
    d_func()->init();
    *this = other;
}

/*!
Destroys the object, deleting the associated data object.
*/

BUuid::~BUuid()
{
    //
}

/*============================== Static public methods =====================*/

/*!
Returns a canonical form of the UUID represented by \a uuidText.

The canonical UUID text is formatted as five hex fields separated by '-', e.g.,
"{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" where 'x' is a hex digit.

\sa pureText()
*/

QString BUuid::canonicalText(const QString &uuidText)
{
    QUuid uuid(BTextTools::wrapped(uuidText, "{", "}"));
    return !uuid.isNull() ? uuid.toString() : QString();
}

/*!
On any platform other than Windows, this function returns a new UUID with variant QUuid::DCE and version QUuid::Random.
If the /dev/urandom device exists, then the numbers used to construct the UUID will be of cryptographic quality, which
will make the UUID unique. Otherwise, the numbers of the UUID will be obtained from the local pseudo-random number
generator (qrand(), which is seeded by qsrand()) which is usually not of cryptograhic quality, which means that the
UUID can't be guaranteed to be unique.

On a Windows platform, a GUID is generated, which almost certainly will be unique, on this or any other system,
networked or not.

\sa variant(), version()
*/

BUuid BUuid::createUuid()
{
    return BUuid(QUuid::createUuid());
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

/*!
This function returns a new UUID with variant QUuid::DCE and version QUuid::Md5. \a ns is the namespace and \a baseData
is the basic data as described by RFC 4122.

\note This function is only available with Qt5.

\sa variant(), version(), createUuidV5()
*/

BUuid BUuid::createUuidV3(const BUuid &ns, const QByteArray &baseData)
{
    return BUuid(QUuid::createUuidV3(ns.d_func()->muuid, baseData));
}

/*!
This function returns a new UUID with variant QUuid::DCE and version QUuid::Md5. \a ns is the namespace and \a baseData
is the basic data as described by RFC 4122.

\note This function is only available with Qt5.

\sa variant(), version(), createUuidV5()
*/

BUuid BUuid::createUuidV3(const BUuid &ns, const QString &baseData)
{
    return BUuid(QUuid::createUuidV3(ns.d_func()->muuid, baseData));
}

/*!
This function returns a new UUID with variant QUuid::DCE and version QUuid::Sha1. \a ns is the namespace and
\a baseData is the basic data as described by RFC 4122.

\note This function is only available with Qt5.

\sa variant(), version(), createUuidV3()
*/

BUuid BUuid::createUuidV5(const BUuid &ns, const QByteArray &baseData)
{
    return BUuid(QUuid::createUuidV5(ns.d_func()->muuid, baseData));
}

/*!
This function returns a new UUID with variant QUuid::DCE and version QUuid::Sha1. \a ns is the namespace and
\a baseData is the basic data as described by RFC 4122.

\note This function is only available with Qt5.

\sa variant(), version(), createUuidV3()
*/

BUuid BUuid::createUuidV5(const BUuid &ns, const QString &baseData)
{
    return BUuid(QUuid::createUuidV5(ns.d_func()->muuid, baseData));
}

#endif

/*!
Creates a BUuid object from the binary representation of the UUID given by \a bytes, as specified by RFC 4122 section
4.1.2. See toRfc4122() for a further explanation of the order of bytes required.

The byte array accepted is not a human readable format.

If the conversion fails, a null UUID is created.

\sa toRfc4122(), BUuid()
*/

BUuid BUuid::fromRfc4122(const QByteArray &bytes)
{
    return BUuid(QUuid::fromRfc4122(bytes));
}

/*!
Returns a canonical form of the UUID represented by \a uuidText.

The canonical UUID text is formatted as five hex fields separated by '-', e.g., "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
where 'x' is a hex digit.

\sa canonicalText()
*/

QString BUuid::pureText(const QString &uuidText)
{
    return BUuid(canonicalText(uuidText)).toString(true);
}

/*============================== Public methods ============================*/

/*!
Returns true if this is the null UUID {00000000-0000-0000-0000-000000000000}; otherwise returns false.
*/

bool BUuid::isNull() const
{
    return d_func()->muuid.isNull();
}

/*!
Returns the binary representation of this BUuid. The byte array is formatted as five hex fields separated by '-' and
enclosed in curly braces, i.e., "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" where 'x' is a hex digit. From left to right,
the five hex fields are obtained from the four public data members in BUuid.

\sa toString()
*/

QByteArray BUuid::toByteArray() const
{
    return d_func()->muuid.toByteArray();
}

/*!
Returns the binary representation of this BUuid. The byte array is in big endian format, and formatted according to RFC
4122, section 4.1.2 - "Layout and byte order".
*/

QByteArray BUuid::toRfc4122() const
{
    return d_func()->muuid.toRfc4122();
}

/*!
Returns the string representation of this BUuid. The string is formatted as five hex fields separated by '-' and
enclosed in curly braces, i.e., "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}" where 'x' is a hex digit. From left to right,
the five hex fields are obtained from the four public data members in BUuid.

If \a pure is true, the hex fields are \e not enclosed in curly brackets.

\sa toByteArray()
*/

QString BUuid::toString(bool pure) const
{
    return pure ? BTextTools::unwrapped(toString(), "{", "}") : d_func()->muuid.toString();
}

/*!
Returns the copy of the underlying QUuid object.
*/

QUuid BUuid::toUuid() const
{
    return d_func()->muuid;
}

/*!
Returns the value in the variant field of the UUID. If the return value is QUuid::DCE, call version() to see which
layout it uses. The null UUID is considered to be of an unknown variant.

\sa version()
*/

QUuid::Variant BUuid::variant() const
{
    return d_func()->muuid.variant();
}

/*!
Returns the version field of the UUID, if the UUID's variant field is QUuid::DCE. Otherwise it returns
QUuid::VerUnknown.

\sa variant()
*/

QUuid::Version BUuid::version() const
{
    return d_func()->muuid.version();
}

/*============================== Public operators ==========================*/

#if defined(Q_OS_WIN)

/*!
Returns a Windows GUID from a BUuid.

\note This function is only available on Windows.
*/

BUuid::operator GUID() const
{
    const QUuid &u = d_func()->muuid;
    GUID guid = {u.data1, u.data2, u.data3,
                 {u.data4[0], u.data4[1], u.data4[2], u.data4[3], u.data4[4], u.data4[5], u.data4[6], u.data4[7]}};
    return guid;
}

#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

/*!
Returns the string representation of the UUID.

\note This function is only available with Qt4.

\sa toString()
*/

BUuid::operator QString() const
{
    return toString();
}

#endif

/*!
Stores the UUID in a QVariant. Returns that QVariant.
*/

BUuid::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
Assigns \a other to this UUID and returns a reference to this UUID.
*/

BUuid &BUuid::operator= (const BUuid &other)
{
    d_func()->muuid = other.d_func()->muuid;
    return *this;
}

/*!
Returns true if this UUID is not equal to UUID \a other; otherwise returns false.
*/

bool BUuid::operator!= (const BUuid &other) const
{
    return d_func()->muuid != other.d_func()->muuid;
}

/*!
Returns true if this BUuid has the same variant field as the \a other BUuid and is lexicographically before the other
BUuid. If the other BUuid has a different variant field, the return value is determined by comparing the two variants.

\sa variant()
*/

bool BUuid::operator< (const BUuid &other) const
{
    return d_func()->muuid < other.d_func()->muuid;
}

/*!
Returns true if this UUID is equal to UUID \a other; otherwise returns false.
*/

bool BUuid::operator== (const BUuid &other) const
{
    return d_func()->muuid == other.d_func()->muuid;
}

/*!
Returns true if this BUuid has the same variant field as the \a other BUuid and is lexicographically after the other
BUuid. If the other BUuid has a different variant field, the return value is determined by comparing the two variants.

\sa variant()
*/

bool BUuid::operator> (const BUuid &other) const
{
    return d_func()->muuid > other.d_func()->muuid;
}

#if defined(Q_OS_WIN)

/*!
Returns true if this UUID is not equal to the Windows GUUID \a guid; otherwise returns false.

\note This function is only available on Windows.
*/

bool BUuid::operator!= (const GUID &guid) const
{
    return d_func()->muuid != guid;
}

/*!
Assigns a Windows GUID \a guid guid to this BUuid. Returns the referance to this BUuid.

\note This function is only available on Windows.
*/

BUuid &BUuid::operator= (const GUID &guid)
{
    d_func()->muuid = guid;
    return *this;
}

/*!
Returns true if this UUID is equal to the Windows GUUID \a guid; otherwise returns false.

\note This function is only available on Windows.
*/

bool BUuid::operator== (const GUID &guid) const
{
    return d_func()->muuid == guid;
}

#endif

/*============================== Public friend operators ===================*/

/*!
\relates BUuid
Writes the UUID \a id to the data stream \a stream.

Returns a reference to the stream.
*/

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

/*!
\relates BUuid
Reads a UUID from the data stream \a stream into \a id.

Returns a reference to the stream.
*/

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

/*!
Returns a hash of the UUID \a uuid, using \a seed to seed the calculation.
*/

uint qHash(const BUuid &uuid, uint seed)
{
    const QUuid &u = uuid.d_func()->muuid;
    return u.data1 ^ u.data2 ^ (u.data3 << 16)
            ^ ((u.data4[0] << 24) | (u.data4[1] << 16) | (u.data4[2] << 8) | u.data4[3])
            ^ ((u.data4[4] << 24) | (u.data4[5] << 16) | (u.data4[6] << 8) | u.data4[7]) ^ seed;
}

/*!
\relates BUuid
Writes the UUID \a id to the output stream for debugging information \a dbg and returns a reference to the stream.
*/

QDebug operator<< (QDebug dbg, const BUuid &id)
{
    dbg.nospace() << "BUuid(" << id.toString() << ')';
    return dbg.space();
}
