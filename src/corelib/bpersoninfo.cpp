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

#include "bpersoninfo.h"

#include "bbase.h"
#include "bbase_p.h"

#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BPersonInfoPrivate ==========================
============================================================================*/

class B_CORE_EXPORT BPersonInfoPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPersonInfo)
public:
    explicit BPersonInfoPrivate(BPersonInfo *q);
    virtual ~BPersonInfoPrivate();
public:
    void init();
public:
    QString image;
    QString mail;
    QString name;
    QString role;
    QString site;
private:
    Q_DISABLE_COPY(BPersonInfoPrivate)
};

/*============================================================================
================================ BPersonInfoPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BPersonInfoPrivate::BPersonInfoPrivate(BPersonInfo *q) :
    BBasePrivate(q)
{
    //
}

BPersonInfoPrivate::~BPersonInfoPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BPersonInfoPrivate::init()
{
    //
}

/*============================================================================
================================ BPersonInfo =================================
============================================================================*/

/*============================== Public constructors =======================*/

BPersonInfo::BPersonInfo(const QString &name, const QString &role, const QString &site, const QString &mail,
                         const QString &image) :
    BBase(*new BPersonInfoPrivate(this))
{
    B_D(BPersonInfo);
    d->init();
    d->name = name;
    d->role = role;
    d->site = site;
    d->mail = mail;
    d->image = image;
}

BPersonInfo::BPersonInfo(const BPersonInfo &other) :
    BBase(*new BPersonInfoPrivate(this))
{
    *this = other;
}

BPersonInfo::~BPersonInfo()
{
    //
}

/*============================== Protected constructors ====================*/

BPersonInfo::BPersonInfo(BPersonInfoPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QString BPersonInfo::image() const
{
    return d_func()->image;
}

bool BPersonInfo::isValid() const
{
    return !d_func()->name.isEmpty();
}

QString BPersonInfo::mail() const
{
    return d_func()->mail;
}

QString BPersonInfo::name() const
{
    return d_func()->name;
}

QString BPersonInfo::role() const
{
    return d_func()->role;
}

void BPersonInfo::setImage(const QString &image)
{
    d_func()->image = image;
}

void BPersonInfo::setMail(const QString &mail)
{
    d_func()->mail = mail;
}

void BPersonInfo::setName(const QString &name)
{
    d_func()->name = name;
}

void BPersonInfo::setRole(const QString &role)
{
    d_func()->role = role;
}

void BPersonInfo::setSite(const QString &site)
{
    d_func()->site = site;
}

QString BPersonInfo::site() const
{
    return d_func()->site;
}

/*============================== Public operators ==========================*/

BPersonInfo &BPersonInfo::operator =(const BPersonInfo &other)
{
    B_D(BPersonInfo);
    const BPersonInfoPrivate *dd = other.d_func();
    d->name = dd->name;
    d->role = dd->role;
    d->site = dd->site;
    d->mail = dd->mail;
    d->image = dd->image;
    return *this;
}

bool BPersonInfo::operator ==(const BPersonInfo &other) const
{
    const B_D(BPersonInfo);
    const BPersonInfoPrivate *dd = other.d_func();
    return d->name == dd->name && d->role == dd->role && d->site == dd->site && d->mail == dd->mail
            && d->image == dd->image;
}

bool BPersonInfo::operator !=(const BPersonInfo &other) const
{
    return !(other == *this);
}

BPersonInfo::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BPersonInfo &info)
{
    const BPersonInfoPrivate *d = info.d_func();
    QVariantMap m;
    m.insert("name", d->name);
    m.insert("role", d->role);
    m.insert("site", d->site);
    m.insert("mail", d->mail);
    m.insert("image", d->image);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BPersonInfo &info)
{
    BPersonInfoPrivate *d = info.d_func();
    QVariantMap m;
    stream >> m;
    d->name = m.value("name").toString();
    d->role = m.value("role").toString();
    d->site = m.value("site").toString();
    d->mail = m.value("mail").toString();
    d->image =m.value("image").toString();
    return stream;
}

QDebug operator <<(QDebug dbg, const BPersonInfo &info)
{
    const BPersonInfoPrivate *d = info.d_func();
    dbg.nospace() << "BPersonInfo(" << d->name << "," << d->role << "," << d->site << "," << d->mail << ","
                  << d->image << ")";
    return dbg.space();
}
