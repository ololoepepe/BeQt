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

#include "bpassword.h"

#include "bbase_p.h"
#include "bnamespace.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BPasswordPrivate ============================
============================================================================*/

class BPasswordPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPassword)
public:
    QCryptographicHash::Algorithm algorithm;
    int charCount;
    QByteArray encrypted;
    bool isEncrypted;
    BPassword::Mode mode;
    QString open;
public:
    explicit BPasswordPrivate(BPassword *q);
    ~BPasswordPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BPasswordPrivate)
};

/*============================================================================
================================ BPasswordPrivate ============================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordPrivate::BPasswordPrivate(BPassword *q) :
    BBasePrivate(q)
{
    //
}

BPasswordPrivate::~BPasswordPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BPasswordPrivate::init()
{
    mode = BPassword::FlexibleMode;
    charCount = 0;
    algorithm = QCryptographicHash::Sha1;
    isEncrypted = false;
}

/*============================================================================
================================ BPassword ===================================
============================================================================*/

/*============================== Public constructors =======================*/

BPassword::BPassword(Mode m) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setMode(m);
}

BPassword::BPassword(const QString &s) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setPassword(s);
}

BPassword::BPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setPassword(a, ba, charCountHint);
}

BPassword::BPassword(const BPassword &other) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    *this = other;
}

BPassword::~BPassword()
{
    //
}

/*============================== Protected constructors ====================*/

BPassword::BPassword(BPasswordPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QCryptographicHash::Algorithm BPassword::algorithm() const
{
    return d_func()->algorithm;
}

int BPassword::charCountHint(Mode m) const
{
    switch (m) {
    case AlwaysOpenMode:
        return d_func()->open.length();
    case AlwaysEncryptedMode:
        return d_func()->charCount;
    case FlexibleMode:
    default:
        return d_func()->isEncrypted ? d_func()->charCount : d_func()->open.length();
    }
}

void BPassword::clear()
{
    B_D(BPassword);
    d->open.clear();
    d->encrypted.clear();
    d->charCount = 0;
    d->algorithm = QCryptographicHash::Sha1;
    d->isEncrypted = false;
}

void BPassword::encrypt(QCryptographicHash::Algorithm a)
{
    if (d_func()->isEncrypted)
        return;
    int count = 0;
    QByteArray ba = encryptedPassword(a, &count);
    setPassword(a, ba, count);
}

QByteArray BPassword::encryptedPassword(int *hint) const
{
    return encryptedPassword(d_func()->algorithm, hint);
}

QByteArray BPassword::encryptedPassword(QCryptographicHash::Algorithm a, int *hint) const
{
    if (hint)
        *hint = charCountHint();
    if (!d_func()->encrypted.isEmpty())
        return d_func()->encrypted;
    return !d_func()->open.isEmpty() ? QCryptographicHash::hash(d_func()->open.toUtf8(), a) : QByteArray();
}

bool BPassword::isEncrypted() const
{
    return d_func()->isEncrypted;
}

BPassword::Mode BPassword::mode() const
{
    return d_func()->mode;
}

QString BPassword::openPassword() const
{
    return d_func()->open;
}

void BPassword::restore(const QByteArray &data)
{
    QVariantMap m = BeQt::deserialize(data).toMap();
    if (m.value("encrypted").toBool()) {
        setPassword(static_cast<QCryptographicHash::Algorithm>(m.value("algorithm").toInt()),
                    m.value("encrypted_password").toByteArray(), m.value("char_count_hint").toInt());
    } else {
        setPassword(m.value("open_password").toString());
    }
}

QByteArray BPassword::save(Mode mode) const
{
    const B_D(BPassword);
    QVariantMap m;
    bool enc = (AlwaysEncryptedMode == mode || d->isEncrypted);
    m.insert("encrypted", enc);
    if (enc) {
        m.insert("encrypted_password", encryptedPassword());
        m.insert("char_count_hint", charCountHint());
        m.insert("algorithm", d->algorithm);
    } else {
        m.insert("open_password", d->open);
    }
    return BeQt::serialize(m);
}

void BPassword::setMode(Mode m)
{
    if (m == d_func()->mode)
        return;
    d_func()->mode = m;
    switch (m) {
    case AlwaysOpenMode:
        clear();
        break;
    case AlwaysEncryptedMode:
        encrypt(d_func()->algorithm);
        break;
    case FlexibleMode:
    default:
        break;
    }
}

void BPassword::setPassword(const QString &s, QCryptographicHash::Algorithm a)
{
    clear();
    B_D(BPassword);
    d->open = s;
    if (AlwaysEncryptedMode == d->mode)
        encrypt(a);
}

void BPassword::setPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint)
{
    clear();
    B_D(BPassword);
    if (AlwaysOpenMode == d->mode)
        return;
    d->encrypted = ba;
    d->charCount = (charCountHint > 0 && !ba.isEmpty()) ? charCountHint : 0;
    d->algorithm = a;
    d->isEncrypted = true;
}

BPassword BPassword::toEncrypted(QCryptographicHash::Algorithm a) const
{
    BPassword pwd = *this;
    pwd.encrypt(a);
    return pwd;
}

/*============================== Public operators ==========================*/

BPassword &BPassword::operator =(const BPassword &other)
{
    B_D(BPassword);
    const BPasswordPrivate *dd = other.d_func();
    d->mode = dd->mode;
    d->open = dd->open;
    d->encrypted = dd->encrypted;
    d->charCount = dd->charCount;
    d->algorithm = dd->algorithm;
    d->isEncrypted = dd->isEncrypted;
    return *this;
}

bool BPassword::operator ==(const BPassword &other) const
{
    const B_D(BPassword);
    const BPasswordPrivate *dd = other.d_func();
    if (d->mode != dd->mode || d->isEncrypted != dd->isEncrypted)
        return false;
    if (d->isEncrypted)
        return d->encrypted == dd->encrypted && d->charCount == dd->charCount && d->algorithm == dd->algorithm;
    else
        return d->open == dd->open;
}

bool BPassword::operator !=(const BPassword &other) const
{
    return !(other == *this);
}

BPassword::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

QDataStream &operator <<(QDataStream &stream, const BPassword &pwd)
{
    const BPasswordPrivate *d = pwd.d_func();
    QVariantMap m;
    m.insert("mode", d->mode);
    m.insert("open", d->open);
    m.insert("encrypted", d->encrypted);
    m.insert("char_count", d->charCount);
    m.insert("algorythm", d->algorithm);
    m.insert("is_encrypted", d->isEncrypted);
    stream << m;
    return stream;
}

QDataStream &operator >>(QDataStream &stream, BPassword &pwd)
{
    BPasswordPrivate *d = pwd.d_func();
    QVariantMap m;
    stream >> m;
    d->mode = static_cast<BPassword::Mode>(m.value("mode", BPassword::FlexibleMode).toInt());
    d->open = m.value("open").toString();
    d->encrypted = m.value("encrypted").toByteArray();
    d->charCount = m.value("char_count").toInt();
    d->algorithm = static_cast<QCryptographicHash::Algorithm>(m.value("algorythm", QCryptographicHash::Sha1).toInt());
    d->isEncrypted = m.value("is_encrypted").toBool();
    return stream;
}

QDebug operator <<(QDebug dbg, const BPassword &pwd)
{
    dbg.nospace() << "BPassword(" << pwd.mode() << "," << pwd.openPassword() << ")";
    return dbg.space();
}
