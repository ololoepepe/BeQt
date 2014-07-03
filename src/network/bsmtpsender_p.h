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

#ifndef BSMTPSENDER_P_H
#define BSMTPSENDER_P_H

class BSmtpSenderPrivate;

class QTextStream;

#include "bsmtpsender.h"
#include "bemail.h"
#include "bgenericsocket.h"

#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>
#include <QAbstractSocket>

/*============================================================================
================================ BSmtpSenderPrivate ==========================
============================================================================*/

class B_NETWORK_EXPORT BSmtpSenderPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSmtpSender)
public:
    enum Stage
    {
        Initial = 0,
        Auth,
        AuthLogin,
        AuthPassword,
        Sender,
        Receiver,
        Data,
        Mail,
        Quit
    };
public:
    explicit BSmtpSenderPrivate(BSmtpSender *q);
    ~BSmtpSenderPrivate();
public:
    void init();
    void send(QTextStream &stream, const QString &what);
    void handleInitial(QTextStream &stream);
    void handleAuth(QTextStream &stream);
    void handleAuthLogin(QTextStream &stream);
    void handleAuthPassword(QTextStream &stream);
    void handleSender(QTextStream &stream);
    void handleReceiver(QTextStream &stream);
    void handleData(QTextStream &stream);
    void handleMail(QTextStream &stream);
    void handleQuit(QTextStream &stream);
public Q_SLOTS:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError e);
    void readyRead();
public:
    QString address;
    quint16 port;
    BGenericSocket::SocketType socketType;
    QString hostName;
    QString login;
    QString password;
    BEmail email;
    bool buisy;
    bool success;
    QString err;
    BGenericSocket *socket;
    int stage;
    int receiver;
private:
    Q_DISABLE_COPY(BSmtpSenderPrivate)
};

#endif // BSMTPSENDER_P_H
