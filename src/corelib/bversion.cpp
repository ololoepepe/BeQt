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
#include "bglobal.h"
#include "bbase_p.h"

#include <QString>
#include <QStringList>
#include <QDataStream>
#include <QDebug>
#include <QVariantMap>
#include <QVariant>

/*============================================================================
================================ BVersionPrivate =============================
============================================================================*/

class BVersionPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BVersion)
public:
    explicit BVersionPrivate(BVersion *q);
    ~BVersionPrivate();
public:
    static bool statusFromString(const QString &s, BVersion::Status &status, qint8 *extra = 0);
public:
    void init();
public:
    qint8 major;
    qint8 minor;
    qint8 patch;
    BVersion::Status status;
    qint8 extra;
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

bool BVersionPrivate::statusFromString(const QString &s, BVersion::Status &status, qint8 *extra)
{
    int len = 0;
    if (!s.left(8).compare("prealpha", Qt::CaseInsensitive))
    {
        status = BVersion::PreAlpha;
        len = 8;
    }
    else if (!s.left(2).compare("pa", Qt::CaseInsensitive))
    {
        status = BVersion::PreAlpha;
        len = 2;
    }
    else if (!s.left(5).compare("alpha", Qt::CaseInsensitive))
    {
        status = BVersion::Alpha;
        len = 5;
    }
    else if (!s.left(1).compare("a", Qt::CaseInsensitive))
    {
        status = BVersion::Alpha;
        len = 1;
    }
    else if (!s.left(4).compare("beta", Qt::CaseInsensitive))
    {
        status = BVersion::Beta;
        len = 4;
    }
    else if (!s.left(1).compare("b", Qt::CaseInsensitive))
    {
        status = BVersion::Beta;
        len = 1;
    }
    else if (!s.left(16).compare("releasecandidate", Qt::CaseInsensitive))
    {
        status = BVersion::ReleaseCandidate;
        len = 16;
    }
    else if (!s.left(2).compare("rc", Qt::CaseInsensitive))
    {
        status = BVersion::ReleaseCandidate;
        len = 2;
    }
    else
    {
        return false;
    }
    QString ss = s.mid(len);
    if (ss.isEmpty())
    {
        if (extra)
            *extra = -1;
    }
    else
    {
        bool b = false;
        qint8 x = ss.toInt(&b);
        if (!b)
            return false;
        if (extra)
            *extra = x;
    }
    return true;
}

/*============================== Public methods ============================*/

void BVersionPrivate::init()
{
    major = 0;
    minor = -1;
    patch = -1;
    status = BVersion::NoStatus;
    extra = -1;
}

/*============================================================================
================================ BVersion ====================================
============================================================================*/

/*============================== Static public methods =====================*/

QString BVersion::statusToString(Status s, StatusRepresentation r)
{
    if (Full == r)
    {
        switch (s)
        {
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
    }
    else
    {
        QString ss;
        switch (s)
        {
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

int BVersion::compare(const BVersion &v1, const BVersion &v2)
{
    if (!v1.isValid() && !v2.isValid())
        return 0;
    if (!v1.isValid() && v2.isValid())
        return -1;
    if (v1.isValid() && !v2.isValid())
        return 1;
    if (v1.major() < v2.major())
        return -1;
    else if (v1.major() > v2.major())
        return 1;
    if (v1.minor() < v2.minor())
        return -1;
    else if (v1.minor() > v2.minor())
        return 1;
    if (v1.patch() < v2.patch())
        return -1;
    else if (v1.patch() > v2.patch())
        return 1;
    if (v1.status() != NoStatus && v2.status() == NoStatus)
        return -1;
    else if (v1.status() == NoStatus && v2.status() != NoStatus)
        return 1;
    if (v1.status() < v2.status())
        return -1;
    else if (v1.status() > v2.status())
        return 1;
    if (v2.extra() != 1 && v1.extra() < v2.extra())
        return -1;
    else if (v1.extra() != 1 && v1.extra() > v2.extra())
        return 1;
    return 0;
}

/*============================== Public constructors =======================*/

BVersion::BVersion(qint8 major, qint8 minor, qint8 patch, Status s, qint8 extra) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    setVersion(major, minor, patch, s, extra);
}

BVersion::BVersion(const QString &s) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    setVersion(s);
}

BVersion::BVersion(const BVersion &other) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    *this = other;
}

BVersion::~BVersion()
{
    //
}

/*============================== Protected constructors ====================*/

BVersion::BVersion(BVersionPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BVersion::setVersion(qint8 major, qint8 minor, qint8 patch, Status s, qint8 extra)
{
    B_D(BVersion);
    d->major = major >= 0 ? major : -1;
    d->minor = minor >= 0 ? minor : -1;
    d->patch = patch >= 0 ? patch : -1;
    d->status = s;
    d->extra = extra >= 0 ? extra : -1;
}

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
    for (int i = 0; i < main.size(); ++i)
    {
        bool b = false;
        int v = main.at(i).toInt(&b);
        if (!b)
            return setVersion(-1);
        switch (i)
        {
        case 0:
            setVersion(v);
            break;
        case 1:
            setVersion(major(), v);
            break;
        case 2:
            setVersion(major(), minor(), v);
            break;
        default:
            return setVersion(-1);
        }
    }
    if (parts.size() == 2)
    {
        Status s = NoStatus;
        qint8 x = -1;
        if (!BVersionPrivate::statusFromString(parts.last(), s, &x))
            return setVersion(-1);
        setVersion(major(), minor(), patch(), s, x);
    }
}

qint8 BVersion::major() const
{
    return d_func()->major;
}

qint8 BVersion::minor() const
{
    return d_func()->minor;
}

qint8 BVersion::patch() const
{
    return d_func()->patch;
}

BVersion::Status BVersion::status() const
{
    return d_func()->status;
}

qint8 BVersion::extra() const
{
    return d_func()->extra;
}

QString BVersion::toString(StatusRepresentation r, QChar versionSeparator, QChar statusSeparator) const
{
    if (!isValid())
        return "";
    if (versionSeparator.isNull())
        versionSeparator = '.';
    if (statusSeparator.isNull())
        statusSeparator = '-';
    const B_D(BVersion);
    QString s = QString::number(d->major);
    if (d->minor >= 0)
        s += versionSeparator + QString::number(d->minor);
    if (d->patch >= 0)
        s += versionSeparator + QString::number(d->patch);
    if (NoStatus != d->status)
        s += statusSeparator + statusToString(d->status, r);
    if (d->extra >= 0)
        s += QString::number(d->extra);
    return s;
}

int BVersion::compare(const BVersion &other) const
{
    return compare(*this, other);
}

bool BVersion::isValid() const
{
    return d_func()->major >= 0;
}

/*============================== Public operators ==========================*/

BVersion &BVersion::operator =(const BVersion &other)
{
    B_D(BVersion);
    const BVersionPrivate *dd = other.d_func();
    d->major = dd->major;
    d->minor = dd->minor;
    d->patch = dd->patch;
    d->status = dd->status;
    d->extra = dd->extra;
    return *this;
}

bool BVersion::operator ==(const BVersion &other) const
{
    return compare(other) == 0;
}

bool BVersion::operator !=(const BVersion &other) const
{
    return compare(other) != 0;
}

bool BVersion::operator <(const BVersion &other) const
{
    return compare(other) < 0;
}

bool BVersion::operator >(const BVersion &other) const
{
    return compare(other) > 0;
}

bool BVersion::operator <=(const BVersion &other) const
{
    return compare(other) <= 0;
}

bool BVersion::operator >=(const BVersion &other) const
{
    return compare(other) >= 0;
}

BVersion::operator QString() const
{
    return toString();
}

BVersion::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BVersion &v)
{
    const BVersionPrivate *d = v.d_func();
    QVariantMap m;
    m.insert("major", d->major);
    m.insert("minor", d->minor);
    m.insert("patch", d->patch);
    m.insert("status", d->status);
    m.insert("extra", d->extra);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BVersion &v)
{
    BVersionPrivate *d = v.d_func();
    QVariantMap m;
    stream >> m;
    d->major = m.value("major", -1).toInt();
    d->minor = m.value("minor", -1).toInt();
    d->patch = m.value("patch", -1).toInt();
    d->status = static_cast<BVersion::Status>(m.value("status", BVersion::NoStatus).toInt());
    d->extra = m.value("extra", -1).toInt();
    return stream;
}

QDebug operator <<(QDebug dbg, const BVersion &v)
{
    dbg.nospace() << "BVersion(" << v.toString() << ")";
    return dbg.space();
}
