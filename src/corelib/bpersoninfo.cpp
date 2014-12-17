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

/*!
\class BPersonInfo
\inmodule BeQtCore
\brief The BPersonInfo class stores information about a person (or organization).

The BPersonInfo class represents a person or an organization. It is used by BeQt to provide information about
application authors, translators, etc.

\sa BPersonInfoList, BPersonInfoProvider
*/

/*============================== Public constructors =======================*/

/*!
Constructs a person info object and sets the person's name to \a name, role to \a role, website to \a site, e-mail to
\a mail, and path to image (avatar) to \a image.
*/

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

/*!
Constructs a copy of \a other.
*/

BPersonInfo::BPersonInfo(const BPersonInfo &other) :
    BBase(*new BPersonInfoPrivate(this))
{
    *this = other;
}

/*!
Destroys the object, deleting the associated data object.
*/

BPersonInfo::~BPersonInfo()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it.
*/

BPersonInfo::BPersonInfo(BPersonInfoPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

/*!
Returns the path to the image (avatar) for this person info.

\sa setImage()
*/

QString BPersonInfo::image() const
{
    return d_func()->image;
}

/*!
Returns true if this person info is valid.

The person info is considered valid if it's name is not empty.

\sa name()
*/

bool BPersonInfo::isValid() const
{
    return !d_func()->name.isEmpty();
}

/*!
Returns the e-mail address of the person/organization represented by this person info.

\sa setMail()
*/

QString BPersonInfo::mail() const
{
    return d_func()->mail;
}

/*!
Returns the name of the person/organization represented by this person info.

\sa setName()
*/

QString BPersonInfo::name() const
{
    return d_func()->name;
}

/*!
Returns the role of the person/organization represented by this person info.

A role is the contribution a person/organization made for the application.

\sa setRole()
*/

QString BPersonInfo::role() const
{
    return d_func()->role;
}

/*!
Sets the path to the image (avatar) for the person/organization represented by this person info to \a image.

\sa image()
*/

void BPersonInfo::setImage(const QString &image)
{
    d_func()->image = image;
}

/*!
Sets the e-mail of the person/organization represented by this person info to \a mail.

\sa mail()
*/

void BPersonInfo::setMail(const QString &mail)
{
    d_func()->mail = mail;
}

/*!
Sets the name of the person/organization represented by this person info to \a name.

\sa name()
*/

void BPersonInfo::setName(const QString &name)
{
    d_func()->name = name;
}

/*!
Sets the role of the person/organization represented by this person info to \a role.

\sa role()
*/

void BPersonInfo::setRole(const QString &role)
{
    d_func()->role = role;
}

/*!
Sets the website of the person/organization represented by this person info to \a site.

\sa site()
*/

void BPersonInfo::setSite(const QString &site)
{
    d_func()->site = site;
}

/*!
Returns the website of the person/organization represented by this person info.

\sa setSite()
*/

QString BPersonInfo::site() const
{
    return d_func()->site;
}

/*============================== Public operators ==========================*/

/*!
Assigns \a other to this person info and returns a reference to this person info.
*/

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

/*!
Returns true if this person info is equal to password \a other; otherwise returns false.

The person infos are equal if their names, roles, websites, e-mails and paths to avatar images are equal.
*/

bool BPersonInfo::operator ==(const BPersonInfo &other) const
{
    const B_D(BPersonInfo);
    const BPersonInfoPrivate *dd = other.d_func();
    return d->name == dd->name && d->role == dd->role && d->site == dd->site && d->mail == dd->mail
            && d->image == dd->image;
}

/*!
Returns true if this person info is not equal to person info \a other; otherwise returns false.

The person infos are equal if their names, roles, websites, e-mails and paths to avatar images are equal.
*/

bool BPersonInfo::operator !=(const BPersonInfo &other) const
{
    return !(other == *this);
}

/*!
Stores the person info in a QVariant. Returns that QVariant.
*/

BPersonInfo::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

/*!
\relates BPersonInfo
Writes the person info \a info to the data stream \a stream.

Returns a reference to the stream.
*/

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

/*!
\relates BPersonInfo
Reads a person info from the data stream \a stream into \a info.

Returns a reference to the stream.
*/

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

/*!
\relates BPersonInfo
Writes the person info \a info to the output stream for debugging information \a dbg and returns a reference to the
stream.
*/

QDebug operator <<(QDebug dbg, const BPersonInfo &info)
{
    const BPersonInfoPrivate *d = info.d_func();
    dbg.nospace() << "BPersonInfo(" << d->name << "," << d->role << "," << d->site << "," << d->mail << ","
                  << d->image << ")";
    return dbg.space();
}
