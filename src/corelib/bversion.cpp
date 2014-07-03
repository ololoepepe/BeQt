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
    static bool statusFromString(const QString &s, BVersion::Status &vstatus, qint8 *vextra = 0);
public:
    void init();
public:
    qint8 vmajor;
    qint8 vminor;
    qint8 vpatch;
    BVersion::Status vstatus;
    qint8 vextra;
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
    if (!s.left(8).compare("prealpha", Qt::CaseInsensitive))
    {
        vstatus = BVersion::PreAlpha;
        len = 8;
    }
    else if (!s.left(2).compare("pa", Qt::CaseInsensitive))
    {
        vstatus = BVersion::PreAlpha;
        len = 2;
    }
    else if (!s.left(5).compare("alpha", Qt::CaseInsensitive))
    {
        vstatus = BVersion::Alpha;
        len = 5;
    }
    else if (!s.left(1).compare("a", Qt::CaseInsensitive))
    {
        vstatus = BVersion::Alpha;
        len = 1;
    }
    else if (!s.left(4).compare("beta", Qt::CaseInsensitive))
    {
        vstatus = BVersion::Beta;
        len = 4;
    }
    else if (!s.left(1).compare("b", Qt::CaseInsensitive))
    {
        vstatus = BVersion::Beta;
        len = 1;
    }
    else if (!s.left(16).compare("releasecandidate", Qt::CaseInsensitive))
    {
        vstatus = BVersion::ReleaseCandidate;
        len = 16;
    }
    else if (!s.left(2).compare("rc", Qt::CaseInsensitive))
    {
        vstatus = BVersion::ReleaseCandidate;
        len = 2;
    }
    else
    {
        return false;
    }
    QString ss = s.mid(len);
    if (ss.isEmpty())
    {
        bSet(vextra, (qint8) -1);
    }
    else
    {
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
    vmajor = 0;
    vminor = -1;
    vpatch = -1;
    vstatus = BVersion::NoStatus;
    vextra = -1;
}

/*============================================================================
================================ BVersion ====================================
============================================================================*/

/*============================== Static public methods =====================*/

QString BVersion::statusToString(Status vs, StatusRepresentation r)
{
    if (Full == r)
    {
        switch (vs)
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
        switch (vs)
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

BVersion BVersion::fromString(const QString &s, QChar versionSeparator, QChar statusSeparator)
{
    return BVersion(s, versionSeparator, statusSeparator);
}

/*============================== Public constructors =======================*/

BVersion::BVersion(qint8 vmajor, qint8 vminor, qint8 vpatch, Status vs, qint8 vextra) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    setVersion(vmajor, vminor, vpatch, vs, vextra);
}

BVersion::BVersion(const QString &s, QChar versionSeparator, QChar statusSeparator) :
    BBase(*new BVersionPrivate(this))
{
    d_func()->init();
    setVersion(s, versionSeparator, statusSeparator);
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

void BVersion::setVersion(qint8 vmajor, qint8 vminor, qint8 vpatch, Status vs, qint8 vextra)
{
    B_D(BVersion);
    d->vmajor = vmajor >= 0 ? vmajor : -1;
    d->vminor = vminor >= 0 ? vminor : -1;
    d->vpatch = vpatch >= 0 ? vpatch : -1;
    d->vstatus = enum_cast(vs, NoStatus, ReleaseCandidate);
    d->vextra = vextra >= 0 ? vextra : -1;
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
    const B_D(BVersion);
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
            setVersion(d->vmajor, v);
            break;
        case 2:
            setVersion(d->vmajor, d->vminor, v);
            break;
        default:
            return setVersion(-1);
        }
    }
    if (parts.size() == 2)
    {
        Status vs = NoStatus;
        qint8 x = -1;
        if (!BVersionPrivate::statusFromString(parts.last(), vs, &x))
            return setVersion(-1);
        setVersion(d->vmajor, d->vminor, d->vpatch, vs, x);
    }
}

void BVersion::clear()
{
    setVersion(0);
}

qint8 BVersion::vmajor() const
{
    return d_func()->vmajor;
}

qint8 BVersion::vminor() const
{
    return d_func()->vminor;
}

qint8 BVersion::vpatch() const
{
    return d_func()->vpatch;
}

BVersion::Status BVersion::vstatus() const
{
    return d_func()->vstatus;
}

qint8 BVersion::vextra() const
{
    return d_func()->vextra;
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

int BVersion::compare(const BVersion &other) const
{
    return compare(*this, other);
}

bool BVersion::isValid() const
{
    return d_func()->vmajor >= 0;
}

/*============================== Public operators ==========================*/

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
    m.insert("major", d->vmajor);
    m.insert("minor", d->vminor);
    m.insert("patch", d->vpatch);
    m.insert("status", d->vstatus);
    m.insert("extra", d->vextra);
    stream << m;
    return stream;
}

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

QDebug operator <<(QDebug dbg, const BVersion &v)
{
    dbg.nospace() << "BVersion(" << v.toString() << ")";
    return dbg.space();
}
