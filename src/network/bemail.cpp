/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
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

#include "bemail.h"

#include <BeQtCore/private/bbase_p.h>

#include <QDataStream>
#include <QDebug>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BEmailPrivate ===============================
============================================================================*/

class BEmailPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BEmail)
public:
    QString body;
    QStringList receivers;
    QString sender;
    QString subject;
public:
    explicit BEmailPrivate(BEmail *q);
    ~BEmailPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BEmailPrivate)
};

/*============================================================================
================================ BEmailPrivate ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BEmailPrivate::BEmailPrivate(BEmail *q) :
    BBasePrivate(q)
{
    //
}

BEmailPrivate::~BEmailPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BEmailPrivate::init()
{
    //
}

/*============================================================================
================================ BEmail ======================================
============================================================================*/

/*============================== Public constructors =======================*/

BEmail::BEmail() :
    BBase(*new BEmailPrivate(this))
{
    d_func()->init();
}

BEmail::BEmail(const BEmail &other) :
    BBase(*new BEmailPrivate(this))
{
    d_func()->init();
    *this = other;
}

BEmail::~BEmail()
{
    //
}

/*============================== Protected constructors ====================*/

BEmail::BEmail(BEmailPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

void BEmail::registerThisType()
{
    static bool registered = false;
    if (registered)
        return;
    qRegisterMetaType<BEmail>();
    qRegisterMetaTypeStreamOperators<BEmail>();
    registered = true;
}

/*============================== Public methods ============================*/

QString BEmail::body() const
{
    return d_func()->body;
}

bool BEmail::isValid() const
{
    const B_D(BEmail);
    return !d->receivers.isEmpty();
}

QString BEmail::receiver(int index) const
{
    const B_D(BEmail);
    return (!d->receivers.isEmpty() && index >= 0 && index < d->receivers.size()) ? d->receivers.at(index) : QString();
}

QStringList BEmail::receivers() const
{
    return d_func()->receivers;
}

int BEmail::receiversCount() const
{
    return d_func()->receivers.size();
}

QString BEmail::sender() const
{
    return d_func()->sender;
}

void BEmail::setBody(const QString &body)
{
    d_func()->body = body;
}

void BEmail::setReceiver(const QString &receiver)
{
    setReceivers(QStringList() << receiver);
}

void BEmail::setReceivers(const QStringList &list)
{
    B_D(BEmail);
    d->receivers.clear();
    foreach (const QString &r, list) {
        if (!r.isEmpty())
            d->receivers << r;
    }
}

void BEmail::setSender(const QString &sender)
{
    d_func()->sender = sender;
}

void BEmail::setSubject(const QString &subject)
{
    d_func()->subject = subject;
}

QString BEmail::subject() const
{
    return d_func()->subject;
}

/*============================== Public operators ==========================*/

BEmail &BEmail::operator =(const BEmail &other)
{
    B_D(BEmail);
    const BEmailPrivate *dd = other.d_func();
    d->sender = dd->sender;
    d->receivers = dd->receivers;
    d->subject = dd->subject;
    d->body = dd->body;
    return *this;
}

bool BEmail::operator ==(const BEmail &other) const
{
    const B_D(BEmail);
    const BEmailPrivate *dd = other.d_func();
    return d->sender == dd->sender && d->receivers == dd->receivers && d->subject == dd->subject
            && d->body == dd->body;
}

bool BEmail::operator !=(const BEmail &other) const
{
    return !(*this == other);
}

BEmail::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BEmail &e)
{
    const BEmailPrivate *d = e.d_func();
    QVariantMap m;
    m.insert("body", d->body);
    m.insert("receivers", d->receivers);
    m.insert("sender", d->sender);
    m.insert("subject", d->subject);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BEmail &e)
{
    BEmailPrivate *d = e.d_func();
    QVariantMap m;
    stream >> m;
    d->body = m.value("body").toString();
    d->receivers = m.value("receivers").toStringList();
    d->sender = m.value("sender").toString();
    d->subject = m.value("subject").toString();
    return stream;
}

QDebug operator <<(QDebug dbg, const BEmail &)
{
    dbg.nospace() << "BEmail(" << ")";
    return dbg.space();
}
