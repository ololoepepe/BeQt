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

#ifndef BSMTPSENDER_H
#define BSMTPSENDER_H

class BSmtpSenderPrivate;

#include "bemail.h"
#include "bgenericsocket.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QObject>

/*============================================================================
================================ BSmtpSender =================================
============================================================================*/

class B_NETWORK_EXPORT BSmtpSender : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSmtpSender)
public:
    static bool send(const QString &address, const BEmail &email, int timeout = 30 * BeQt::Second,
                     const QString &userName = QString(), const QString &userPassword = QString(), quint16 port = 25,
                     BGenericSocket::SocketType type = BGenericSocket::SslSocket,
                     const QString &localHostName = QString(), QString *error = 0, bool translationsEnabled = false);
public:
    explicit BSmtpSender(QObject *parent = 0);
    ~BSmtpSender();
protected:
    explicit BSmtpSender(BSmtpSenderPrivate &d, QObject *parent = 0);
public:
    void setServer(const QString &address, quint16 port = 25);
    void setSocketType(BGenericSocket::SocketType type);
    void setLocalHostName(const QString &hostName);
    void setUser(const QString &name, const QString &password = QString());
    void setEmail(const BEmail &email);
    QString serverAddress() const;
    quint16 serverPort() const;
    BGenericSocket::SocketType socketType() const;
    QString localHostName() const;
    QString userName() const;
    QString userPassword() const;
    BEmail email() const;
    bool isValid() const;
    bool isBuisy() const;
    bool waitForFinished(int msecs = 30 * BeQt::Second);
    bool lastTransferSuccess() const;
    QString lastTransferError() const;
public Q_SLOTS:
    void send(const BEmail &email = BEmail());
Q_SIGNALS:
    void finished(bool success, const QString &error);
private:
    Q_DISABLE_COPY(BSmtpSender)
};

#endif // BSMTPSENDER_H
