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

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QString>

/*============================================================================
================================ BSmtpSender =================================
============================================================================*/

class B_NETWORK_EXPORT BSmtpSender : public QObject, public BBaseObject
{
    Q_OBJECT
    Q_PROPERTY(bool buisy READ isBuisy STORED false)
    Q_PROPERTY(BEmail email READ email WRITE setEmail)
    Q_PROPERTY(QString localHostName READ localHostName WRITE setLocalHostName)
    Q_PROPERTY(QString serverAddress READ serverAddress)
    Q_PROPERTY(quint16 serverPort READ serverPort)
    Q_PROPERTY(BGenericSocket::SocketType socketType READ socketType WRITE setSocketType)
    Q_PROPERTY(QString userName READ userName)
    Q_PROPERTY(QString userPassword READ userPassword)
    Q_PROPERTY(bool valid READ isValid STORED false)
    B_DECLARE_PRIVATE(BSmtpSender)
public:
    explicit BSmtpSender(QObject *parent = 0);
    ~BSmtpSender();
protected:
    explicit BSmtpSender(BSmtpSenderPrivate &d, QObject *parent = 0);
public:
    static bool send(const QString &address, const BEmail &email, int timeout = 30 * BeQt::Second,
                     const QString &userName = QString(), const QString &userPassword = QString(), quint16 port = 25,
                     BGenericSocket::SocketType type = BGenericSocket::SslSocket,
                     const QString &localHostName = QString(), QString *error = 0, bool translationsEnabled = false);
public:
    BEmail email() const;
    bool isBuisy() const;
    bool isValid() const;
    QString lastTransferError() const;
    bool lastTransferSuccess() const;
    QString localHostName() const;
    QString serverAddress() const;
    quint16 serverPort() const;
    void setEmail(const BEmail &email);
    void setLocalHostName(const QString &hostName);
    void setServer(const QString &address, quint16 port = 25);
    void setSocketType(BGenericSocket::SocketType type);
    void setUser(const QString &name, const QString &password = QString());
    BGenericSocket::SocketType socketType() const;
    QString userName() const;
    QString userPassword() const;
    bool waitForFinished(int msecs = 30 * BeQt::Second);
public Q_SLOTS:
    void send(const BEmail &email = BEmail());
Q_SIGNALS:
    void finished(bool success, const QString &error);
private:
    Q_DISABLE_COPY(BSmtpSender)
};

#endif // BSMTPSENDER_H
