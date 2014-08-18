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

#ifndef BPASSWORD_H
#define BPASSWORD_H

class BPasswordPrivate;

class QDataStream;
class QDebug;
class QString;
class QVariant;

#include "bbase.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QMetaType>

/*============================================================================
================================ BPassword ===================================
============================================================================*/

class B_CORE_EXPORT BPassword : public BBase
{
    B_DECLARE_PRIVATE(BPassword)
public:
    enum Mode
    {
        FlexibleMode = 0,
        AlwaysOpenMode,
        AlwaysEncryptedMode
    };
public:
    explicit BPassword(Mode m = FlexibleMode);
    explicit BPassword(const QString &s);
    explicit BPassword(QCryptographicHash::Algorithm a, const QByteArray &ba = QByteArray(), int charCountHint = 0);
    BPassword(const BPassword &other);
    ~BPassword();
protected:
    explicit BPassword(BPasswordPrivate &d);
public:
    QCryptographicHash::Algorithm algorithm() const;
    int charCountHint(Mode m = FlexibleMode) const;
    void clear();
    void encrypt(QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    QByteArray encryptedPassword(int *hint = 0) const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm a, int *hint = 0) const;
    bool isEncrypted() const;
    Mode mode() const;
    QString openPassword() const;
    void restore(const QByteArray &data);
    QByteArray save(Mode m = FlexibleMode) const;
    void setMode(Mode m);
    void setPassword(const QString &s, QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    void setPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint = 0);
    BPassword toEncrypted(QCryptographicHash::Algorithm a = QCryptographicHash::Sha1) const;
public:
    BPassword &operator =(const BPassword &other);
    bool operator ==(const BPassword &other) const;
    bool operator !=(const BPassword &other) const;
    operator QVariant() const;
public:
    friend QDataStream &operator <<(QDataStream &stream, const BPassword &pwd);
    friend QDataStream &operator >>(QDataStream &stream, BPassword &pwd);
    friend QDebug operator <<(QDebug dbg, const BPassword &pwd);
};

Q_DECLARE_METATYPE(BPassword)

#endif // BPASSWORD_H
