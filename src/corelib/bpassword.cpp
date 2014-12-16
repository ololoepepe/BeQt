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

/*!
\class BPassword
\brief The BPassword class provides means of storing a password in an open or encrypted form.
*/

/*!
\enum BPassword::Mode

This enum type is used to describe the password storage mode of the BPassword.

\value FlexibleMode
The password is stored in open or encrypted mode depending on the method it was set with.

\value AlwaysOpenMode
The password is stored in open mode, even it is set using the method for setting encrypted password. The password is
set to an empty QString in this case.

\value AlwaysEncryptedMode
The password is stored in open mode. It is automatically encryped if set using the method for setting open password.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a password and sets it's mode to \a m.
*/

BPassword::BPassword(Mode m) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setMode(m);
}

/*!
Constructs a password and sets it's value to \a s (the password is stored in an open form).
*/

BPassword::BPassword(const QString &s) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setPassword(s);
}

/*!
Constructs a password and sets it's value to \a ba and char count hint to \a charCountHint (the password is stored in
an encryptyed form). The default encrypting algorythm is set to \a a.
*/

BPassword::BPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    setPassword(a, ba, charCountHint);
}

/*!
Constructs a copy of \a other.
*/

BPassword::BPassword(const BPassword &other) :
    BBase(*new BPasswordPrivate(this))
{
    d_func()->init();
    *this = other;
}

/*!
Destroys the object, deleting the associated data object.
*/

BPassword::~BPassword()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it.
*/

BPassword::BPassword(BPasswordPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

/*!
Returns the algorythm used to encrypt this password.
*/

QCryptographicHash::Algorithm BPassword::algorithm() const
{
    return d_func()->algorithm;
}

/*!
Returns the character count (length) of this password.

If \a m is AlwaysOpenMode, the returned number is the length of a string representation of this password (the number
may be empty if the password is stroed in encrypted form).

If \a m is AlwaysEncryptedMode, the returned number is the one that was calculated when encrypting the password, or
passed along with the encrypted password when it was set.

If \a m is FlexibleMode, the returned number depends on this password's mode.
*/

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

/*!
Clears the password. Both open and encrypted represention are cleared. Encryption algorythm is set to
QCryptographicHash::Sha1.
*/

void BPassword::clear()
{
    B_D(BPassword);
    d->open.clear();
    d->encrypted.clear();
    d->charCount = 0;
    d->algorithm = QCryptographicHash::Sha1;
    d->isEncrypted = false;
}

/*!
Encrypts the password using algorythm \a a. The open representation is cleared.
*/

void BPassword::encrypt(QCryptographicHash::Algorithm a)
{
    if (d_func()->isEncrypted)
        return;
    int count = 0;
    QByteArray ba = encryptedPassword(a, &count);
    setPassword(a, ba, count);
}

/*!
Returns the encrypted representation of the password.

If \a hint is non-null pointer, it's value is set to number of characters in the password.
*/

QByteArray BPassword::encryptedPassword(int *hint) const
{
    return encryptedPassword(d_func()->algorithm, hint);
}

/*!
Returns the encrypted representation of the password.

If the password is already stored in an encrypted form, it is returned as is, and the \a a parameter has no effect.
Otherwise the password stored in an open form is encrypted using the algorithm \a a (the stored password is \e not
modified).

If \a hint is non-null pointer, it's value is set to number of characters in the password.
*/

QByteArray BPassword::encryptedPassword(QCryptographicHash::Algorithm a, int *hint) const
{
    if (hint)
        *hint = charCountHint();
    if (!d_func()->encrypted.isEmpty())
        return d_func()->encrypted;
    return !d_func()->open.isEmpty() ? QCryptographicHash::hash(d_func()->open.toUtf8(), a) : QByteArray();
}

/*!
Returns true if the password is stored in an encrypted form; otherwise returns false.
*/

bool BPassword::isEncrypted() const
{
    return d_func()->isEncrypted;
}

/*!
Returns the storage mode of this password.
*/

BPassword::Mode BPassword::mode() const
{
    return d_func()->mode;
}

/*!
Returns the open representation of this password.
*/

QString BPassword::openPassword() const
{
    return d_func()->open;
}

/*!
Restores the password from a serialized QByteArray \a data. Both the password (open or encrypted) and the storage mode
are restored.
*/

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

/*!
Saves the password to a serialized QByteArray. Both the password (open or encrypted) and it's state (encrypted or not)
are saved. Returns the QByteArray constructed.

If \a mode is AlwaysEncryptedMode or the password is encrypted, the encrypting algorithm is also saved.
*/

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

/*!
Sets the storage mode for this password to \a m.

If the new mode is the same as the old one, nothing is done.

If the new mode is AlwaysOpenMode, the password is just cleared.

If the new mode is AlwaysEncryptedMode, the password becomes encrypted.
*/

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

/*!
Sets the password to \a s.

If the storage mode is AlwaysEncryptedMode, the password is encrypted using the algorithm \a a.
*/

void BPassword::setPassword(const QString &s, QCryptographicHash::Algorithm a)
{
    clear();
    B_D(BPassword);
    d->open = s;
    if (AlwaysEncryptedMode == d->mode)
        encrypt(a);
}

/*!
Sets the password to \a ba and encryption algorithm to \a a. The number of characters is set to \a charCountHint.

If the storage mode is AlwaysOpenMode, nothing is done.
*/

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

/*!
Returns a copy of this password. The copy is encrypted using tha algorighm \a a.
*/

BPassword BPassword::toEncrypted(QCryptographicHash::Algorithm a) const
{
    BPassword pwd = *this;
    pwd.encrypt(a);
    return pwd;
}

/*============================== Public operators ==========================*/

/*!
Assigns \a other to this password and returns a reference to this password.
*/

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

/*!
Returns true if this password is equal to password \a other; otherwise returns false.

The passwords are equal if their storage modes are equal and if they are both encrypted (or both not encrypted). Also,
if the passwords are encrypted, their encrypted representations, number of characters and encription algorithms have to
be equal. Otherwise, if the passwords are not encrypted, their open representations have to be equal.
*/

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

/*!
Returns true if this password is not equal to password \a other; otherwise returns false.

The passwords are equal if their storage modes are equal and if they are both encrypted (or both not encrypted). Also,
if the passwords are encrypted, their encrypted representations, number of characters and encription algorithms have to
be equal. Otherwise, if the passwords are not encrypted, their open representations have to be equal.
*/

bool BPassword::operator !=(const BPassword &other) const
{
    return !(other == *this);
}

/*!
Stores the password in a QVariant. Returns that QVariant.
*/

BPassword::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*============================== Public friend operators ===================*/

/*!
\relates BPassword
Writes the password \a pwd to the data stream \a stream.

Returns a reference to the stream.
*/

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

/*!
\relates BPassword
Reads a password from the data stream \a stream into \a pwd.

Returns a reference to the stream.
*/

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

/*!
\relates BPassword
Writes the password \a pwd to the output stream for debugging information \a dbg and returns a reference to the stream.
*/

QDebug operator <<(QDebug dbg, const BPassword &pwd)
{
    dbg.nospace() << "BPassword(" << pwd.mode() << "," << pwd.openPassword() << ")";
    return dbg.space();
}
