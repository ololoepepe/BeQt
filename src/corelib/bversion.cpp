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

#include "bversion.h"
#include "bbase_p.h"

#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BVersionPrivate =============================
============================================================================*/

class BVersionPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BVersion)
public:
    qint8 vmajor;
    qint8 vminor;
    qint8 vpatch;
    BVersion::Status vstatus;
    qint8 vextra;
public:
    explicit BVersionPrivate(BVersion *q);
    ~BVersionPrivate();
public:
    static bool statusFromString(const QString &s, BVersion::Status &vstatus, qint8 *vextra = 0);
public:
    void init();
private:
    Q_DISABLE_COPY(BVersionPrivate)
};

/*============================================================================
================================ BVersionPrivate =============================
============================================================================*/

/*============================== Public constructors =======================*/

BVersionPrivate::BVersionPrivate(BVersion *q) :
    BBasePrivate(q)
{
    //
}

BVersionPrivate::~BVersionPrivate()
{
    //
}

/*============================== Static public methods =====================*/

bool BVersionPrivate::statusFromString(const QString &s, BVersion::Status &vstatus, qint8 *vextra)
{
    int len = 0;
    if (!s.left(8).compare("prealpha", Qt::CaseInsensitive)) {
        vstatus = BVersion::PreAlpha;
        len = 8;
    } else if (!s.left(2).compare("pa", Qt::CaseInsensitive)) {
        vstatus = BVersion::PreAlpha;
        len = 2;
    } else if (!s.left(5).compare("alpha", Qt::CaseInsensitive)) {
        vstatus = BVersion::Alpha;
        len = 5;
    } else if (!s.left(1).compare("a", Qt::CaseInsensitive)) {
        vstatus = BVersion::Alpha;
        len = 1;
    } else if (!s.left(4).compare("beta", Qt::CaseInsensitive)) {
        vstatus = BVersion::Beta;
        len = 4;
    } else if (!s.left(1).compare("b", Qt::CaseInsensitive)) {
        vstatus = BVersion::Beta;
        len = 1;
    } else if (!s.left(16).compare("releasecandidate", Qt::CaseInsensitive)) {
        vstatus = BVersion::ReleaseCandidate;
        len = 16;
    } else if (!s.left(2).compare("rc", Qt::CaseInsensitive)) {
        vstatus = BVersion::ReleaseCandidate;
        len = 2;
    } else {
        return false;
    }
    QString ss = s.mid(len);
    if (ss.isEmpty()) {
        bSet(vextra, (qint8) -1);
    } else {
        bool b = false;
        qint8 x = ss.toInt(&b);
        if (!b)
            return false;
        bSet(vextra, x);
    }
    return true;
}

/*============================== Public methods ============================*/

void BVersionPrivate::init()
{
    vmajor = -1;
    vminor = -1;
    vpatch = -1;
    vstatus = BVersion::NoStatus;
    vextra = -1;
}

/*============================================================================
================================ BVersion ====================================
============================================================================*/

/*!
\class BVersion
\inmodule BeQtCore
\brief The BVersion class represents a version number of an application, library, etc.

The BVersion is a combination of a major version number, minor version number, patch number, application/library
development status and an extra version number.

Example: "4.1.2-beta3". Here, 4 is the major version number, 1 is the minor one, 2 is the patch number, "beta" is the
development state, and 3 is the extra number (the least significant).
*/

/*!
\enum BVersion::Status

This enum type is used to describe the development status of a library/application.

\value NoStatus
No development status (usually that means that the application status is Release).

\value PreAlpha
Pre-alpha status. The application is in active development.

\value Alpha
Alpha status. The application is still developed, but is already being tested internally.

\value Beta
Beta status. The application on the beta stage is often tested by users, but it's features may still be changed.

\value ReleaseCandidate
The application on the release candidate stage is tested by users, and it's this version is a candidate for release if
there are no bugs found.
*/

/*!
\enum BVersion::StatusRepresentation

This enum type is used to describe the Status text representation.

\value Full
Verbose representation. E.g. "ReleaseCandidate".

\value ShortLowercase
Short representation. E.g. "pa" (pre-alpha).

\value ShortUppercase
Short representation. E.g. "A" (alpha).
*/

/*============================== Public constructors =======================*/

/*!
Constructs a version and sets it's major version number to \a vmajor, minor version number to \a vminor, patch number
to \a vpatch, version status to \a vs, and extra version nubmer to \a vextra.
*/

BVersion::BVersion(qint8 vmajor, qint8 vminor, qint8 vpatch, Status vs, qint8 vextra) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    setVersion(vmajor, vminor, vpatch, vs, vextra);
}

/*!
Constructs a version object. The major version nubmer, the minor version number, etc. are retrieved by parsing \a s.

\a versionSeparator is used as a character that separates major version, minir version and patch number.

\a statusSeparator is used as a character that separates the main part (major.minor.patch) form the version status and
extra version number.

\sa fromString()
*/

BVersion::BVersion(const QString &s, QChar versionSeparator, QChar statusSeparator) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    setVersion(s, versionSeparator, statusSeparator);
}

/*!
Constructs a copy of \a other.
*/

BVersion::BVersion(const BVersion &other) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    *this = other;
}

/*!
Destroys the object, deleting the associated data object.
*/

BVersion::~BVersion()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it.
*/

BVersion::BVersion(BVersionPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

/*!
Compares the versions \a v1 and \a v2. Returns -1 if \a v1 is less than \a v2, 1 if \a v1 is greater than \a v2, and 0
if the versions are equal.
*/

int BVersion::compare(const BVersion &v1, const BVersion &v2)
{
    if (!v1.isValid() && !v2.isValid())
        return 0;
    if (!v1.isValid() && v2.isValid())
        return -1;
    if (v1.isValid() && !v2.isValid())
        return 1;
    const BVersionPrivate *d1 = v1.d_func();
    const BVersionPrivate *d2 = v2.d_func();
    if (d1->vmajor < d2->vmajor)
        return -1;
    else if (d1->vmajor > d2->vmajor)
        return 1;
    if (d1->vminor < d2->vminor)
        return -1;
    else if (d1->vminor > d2->vminor)
        return 1;
    if (d1->vpatch < d2->vpatch)
        return -1;
    else if (d1->vpatch > d2->vpatch)
        return 1;
    if (NoStatus != d1->vstatus && NoStatus == d2->vstatus)
        return -1;
    else if (NoStatus == d1->vstatus && NoStatus != d2->vstatus)
        return 1;
    if (d1->vstatus < d2->vstatus)
        return -1;
    else if (d1->vstatus > d2->vstatus)
        return 1;
    if (d2->vextra != 1 && d1->vextra < d2->vextra)
        return -1;
    else if (d1->vextra != 1 && d1->vextra > d2->vextra)
        return 1;
    return 0;
}

/*!
Creates a version object. The major version nubmer, the minor version number, etc. are retrieved by parsing \a s.

\a versionSeparator is used as a character that separates major version, minir version and patch number.

\a statusSeparator is used as a character that separates the main part (major.minor.patch) form the version status and
extra version number.

Returns the created object.
*/

BVersion BVersion::fromString(const QString &s, QChar versionSeparator, QChar statusSeparator)
{
    return BVersion(s, versionSeparator, statusSeparator);
}

/*!
Returns the string representation of vsersion status \a vs. Status representation depends on \a r.

\sa Status, StatusRepresentation
*/

QString BVersion::statusToString(Status vs, StatusRepresentation r)
{
    if (Full == r) {
        switch (vs) {
        case PreAlpha:
            return "PreAlpha";
        case Alpha:
            return "Alpha";
        case Beta:
            return "Beta";
        case ReleaseCandidate:
            return "ReleaseCandidate";
        default:
            return "";
        }
    } else {
        QString ss;
        switch (vs) {
        case PreAlpha:
            ss = "pa";
            break;
        case Alpha:
            ss = "a";
            break;
        case Beta:
            ss = "b";
            break;
        case ReleaseCandidate:
            ss = "rc";
            break;
        default:
            break;
        }
        return (ShortLowercase == r) ? ss : ss.toUpper();
    }
}

/*============================== Public methods ============================*/

/*!
Sets the major version number to 0, and other version numbers to -1. Sets the version status to NoStatus.
*/

void BVersion::clear()
{
    setVersion(0);
}

/*!
Compares this version to \a other. Returns -1 if this version is less than \a other, 1 if this version is greater than
\a other, and 0 if the versions are equal.
*/

int BVersion::compare(const BVersion &other) const
{
    return compare(*this, other);
}

/*!
Returns true if this version's major version number is greater or equal to 0.
*/

bool BVersion::isValid() const
{
    return d_func()->vmajor >= 0;
}

/*!
Sets major version number to \a vmajor, minor version number to \a vminor, patch number to \a vpatch, version status to
\a vs, and extra version nubmer to \a vextra.
*/

void BVersion::setVersion(qint8 vmajor, qint8 vminor, qint8 vpatch, Status vs, qint8 vextra)
{
    B_D(BVersion);
    d->vmajor = vmajor >= 0 ? vmajor : -1;
    d->vminor = vminor >= 0 ? vminor : -1;
    d->vpatch = vpatch >= 0 ? vpatch : -1;
    d->vstatus = enum_cast(vs, NoStatus, ReleaseCandidate);
    d->vextra = vextra >= 0 ? vextra : -1;
}

/*!
Sets the major version nubmer, the minor version number, etc. to the values retrieved by parsing \a s.

\a versionSeparator is used as a character that separates major version, minir version and patch number.

\a statusSeparator is used as a character that separates the main part (major.minor.patch) form the version status and
extra version number.

\sa fromString()
*/

void BVersion::setVersion(const QString &s, QChar versionSeparator, QChar statusSeparator)
{
    if (s.isEmpty())
        return setVersion(-1);
    if (versionSeparator.isNull())
        versionSeparator = '.';
    if (statusSeparator.isNull())
        statusSeparator = '-';
    QStringList parts = s.split(statusSeparator);
    if (parts.size() > 2)
        return setVersion(-1);
    QStringList main = parts.first().split(versionSeparator);
    if (main.size() > 3)
        return setVersion(-1);
    const B_D(BVersion);
    for (int i = 0; i < main.size(); ++i) {
        bool b = false;
        int v = main.at(i).toInt(&b);
        if (!b)
            return setVersion(-1);
        switch (i) {
        case 0:
            setVersion(v);
            break;
        case 1:
            setVersion(d->vmajor, v);
            break;
        case 2:
            setVersion(d->vmajor, d->vminor, v);
            break;
        default:
            return setVersion(-1);
        }
    }
    if (parts.size() == 2) {
        Status vs = NoStatus;
        qint8 x = -1;
        if (!BVersionPrivate::statusFromString(parts.last(), vs, &x))
            return setVersion(-1);
        setVersion(d->vmajor, d->vminor, d->vpatch, vs, x);
    }
}

/*!
Returns the string representation of this version.

Version status is converted to string using the status representatoon mode \a r.

The major version, minor version and patch number are separated with \a versionSeparator.

The main version part (major.minor.path) is separated form the version status with \a statusSeparator.
*/

QString BVersion::toString(StatusRepresentation r, QChar versionSeparator, QChar statusSeparator) const
{
    if (!isValid())
        return "";
    if (versionSeparator.isNull())
        versionSeparator = '.';
    if (statusSeparator.isNull())
        statusSeparator = '-';
    const B_D(BVersion);
    QString s = QString::number(d->vmajor);
    if (d->vminor >= 0)
        s += versionSeparator + QString::number(d->vminor);
    if (d->vpatch >= 0)
        s += versionSeparator + QString::number(d->vpatch);
    if (NoStatus != d->vstatus)
        s += statusSeparator + statusToString(d->vstatus, r);
    if (d->vextra >= 0)
        s += QString::number(d->vextra);
    return s;
}

/*!
Returns the extra version number.
*/

qint8 BVersion::vextra() const
{
    return d_func()->vextra;
}

/*!
Returns the major version number.
*/

qint8 BVersion::vmajor() const
{
    return d_func()->vmajor;
}

/*!
Returns the minor version number.
*/

qint8 BVersion::vminor() const
{
    return d_func()->vminor;
}

/*!
Returns the patch number.
*/

qint8 BVersion::vpatch() const
{
    return d_func()->vpatch;
}

/*!
Returns the version status.
*/

BVersion::Status BVersion::vstatus() const
{
    return d_func()->vstatus;
}

/*============================== Public operators ==========================*/

/*!
Assigns \a other to this version and returns a reference to this version.
*/

BVersion &BVersion::operator =(const BVersion &other)
{
    B_D(BVersion);
    const BVersionPrivate *dd = other.d_func();
    d->vmajor = dd->vmajor;
    d->vminor = dd->vminor;
    d->vpatch = dd->vpatch;
    d->vstatus = dd->vstatus;
    d->vextra = dd->vextra;
    return *this;
}

/*!
Returns true if this version is equal to version \a other; otherwise returns false.

The versions are equal if their version numbers and version status are identical.
*/

bool BVersion::operator ==(const BVersion &other) const
{
    return compare(other) == 0;
}

/*!
Returns true if this version is not equal to version \a other; otherwise returns false.

The versions are equal if their version numbers and version status are identical.
*/

bool BVersion::operator !=(const BVersion &other) const
{
    return compare(other) != 0;
}

/*!
Returns true if this version is less than version \a other; otherwise returns false.
*/

bool BVersion::operator <(const BVersion &other) const
{
    return compare(other) < 0;
}

/*!
Returns true if this version is greater than version \a other; otherwise returns false.
*/

bool BVersion::operator >(const BVersion &other) const
{
    return compare(other) > 0;
}

/*!
Returns true if this version is less or equal than version \a other; otherwise returns false.
*/

bool BVersion::operator <=(const BVersion &other) const
{
    return compare(other) <= 0;
}

/*!
Returns true if this version is greater or equal than version \a other; otherwise returns false.
*/

bool BVersion::operator >=(const BVersion &other) const
{
    return compare(other) >= 0;
}

/*!
Converts the version to string. Returns that string.

\sa toString()
*/

BVersion::operator QString() const
{
    return toString();
}

/*!
Stores the version in a QVariant. Returns that QVariant.
*/

BVersion::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

/*!
\relates BVersion
Writes the version \a v to the data stream \a stream.

Returns a reference to the stream.
*/

QDataStream &operator <<(QDataStream &stream, const BVersion &v)
{
    const BVersionPrivate *d = v.d_func();
    QVariantMap m;
    m.insert("major", d->vmajor);
    m.insert("minor", d->vminor);
    m.insert("patch", d->vpatch);
    m.insert("status", d->vstatus);
    m.insert("extra", d->vextra);
    stream << m;
    return stream;
}

/*!
\relates BVersion
Reads a version from the data stream \a stream into \a v.

Returns a reference to the stream.
*/

QDataStream &operator >>(QDataStream &stream, BVersion &v)
{
    BVersionPrivate *d = v.d_func();
    QVariantMap m;
    stream >> m;
    d->vmajor = m.value("major", -1).toInt();
    d->vminor = m.value("minor", -1).toInt();
    d->vpatch = m.value("patch", -1).toInt();
    d->vstatus = enum_cast(m.value("status", BVersion::NoStatus).toInt(), BVersion::NoStatus,
                           BVersion::ReleaseCandidate);
    d->vextra = m.value("extra", -1).toInt();
    return stream;
}

/*!
\relates BVersion
Writes theversion \a v to the output stream for debugging information \a dbg and returns a reference to the stream.
*/

QDebug operator <<(QDebug dbg, const BVersion &v)
{
    dbg.nospace() << "BVersion(" << v.toString() << ")";
    return dbg.space();
}
