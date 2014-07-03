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

#include "bsmtpsender.h"
#include "bsmtpsender_p.h"
#include "bgenericsocket.h"

#include <BeQtCore/BeQt>

#include <QObject>
#include <QEventLoop>
#include <QTimer>
#include <QSslSocket>
#include <QIODevice>
#include <QTextStream>
#include <QByteArray>
#include <QMetaObject>

/*============================================================================
================================ BSmtpSenderPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BSmtpSenderPrivate::BSmtpSenderPrivate(BSmtpSender *q) :
    BBaseObjectPrivate(q)
{
    //
}

BSmtpSenderPrivate::~BSmtpSenderPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSmtpSenderPrivate::init()
{
    port = 25;
    socketType = BGenericSocket::TcpSocket;
    buisy = false;
    success = false;
    socket = 0;
    stage = Initial;
    receiver = 0;
}

void BSmtpSenderPrivate::send(QTextStream &stream, const QString &what)
{
    stream << (what + ((what.right(1) != "\n") ? "\n" : ""));
    stream.flush();
}

void BSmtpSenderPrivate::handleInitial(QTextStream &stream)
{
    send(stream, "HELO" + QString(!hostName.isEmpty() ? " " : "") + hostName + "\n");
    if (login.isEmpty())
        stage += 3;
}

void BSmtpSenderPrivate::handleAuth(QTextStream &stream)
{
    send(stream, "AUTH LOGIN\n");
}

void BSmtpSenderPrivate::handleAuthLogin(QTextStream &stream)
{
    send(stream, QString(login.toLatin1().toBase64()) + "\n");
    if (password.isEmpty())
        ++stage;
}

void BSmtpSenderPrivate::handleAuthPassword(QTextStream &stream)
{
    send(stream, QString(password.toLatin1().toBase64()) + "\n");
}

void BSmtpSenderPrivate::handleSender(QTextStream &stream)
{
    send(stream, "MAIL FROM: <" + login + ">\n");
}

void BSmtpSenderPrivate::handleReceiver(QTextStream &stream)
{
    send(stream, "RCPT TO: <" + email.receiver(receiver) + ">\n");
    ++receiver;
    if (email.receiversCount() > receiver)
        --stage;
}

void BSmtpSenderPrivate::handleData(QTextStream &stream)
{
    send(stream, "DATA\n");
}

void BSmtpSenderPrivate::handleMail(QTextStream &stream)
{
    QString s = "Content-Type: text/plain; charset=UTF-8;\n"; //TODO
    s += "Content-Transfer-Encoding: 8bit\n"; //TODO
    s += "FROM: " + (!email.sender().isEmpty() ? email.sender() : login) + "\n";
    s += "TO: " + email.receiver() + "\n";
    if (!email.subject().isEmpty())
        s += "SUBJECT: " + email.subject() + "\n";
    s += "\n";
    s += email.body() + "\r\n.\r\n";
    send(stream, s);
}

void BSmtpSenderPrivate::handleQuit(QTextStream &stream)
{
    stream << "QUIT\n";
}

/*============================== Public slots ==============================*/


void BSmtpSenderPrivate::connected()
{
    if (socket->bytesAvailable())
        readyRead();
}

void BSmtpSenderPrivate::disconnected()
{
    socket->deleteLater();
    socket = 0;
    buisy = false;
    QMetaObject::invokeMethod(q_func(), "finished", Q_ARG(bool, success), Q_ARG(QString, err));
}

void BSmtpSenderPrivate::error(QAbstractSocket::SocketError)
{
    if (err.isEmpty())
        err = socket->errorString();
}

void BSmtpSenderPrivate::readyRead()
{
    QTextStream stream(socket->ioDevice());
    while (socket->canReadLine())
    {
        QString line = stream.readLine();
        bool ok = false;
        int code = line.left(3).toInt(&ok);
        QString text = line.mid(4);
        switch (code)
        {
        case 211:
        case 214:
            break;
        case 220:
            handleInitial(stream);
            break;
        case 221:
            success = true;
            socket->disconnectFromHost();
            break;
        case 235:
            switch (stage)
            {
            case Sender:
                handleSender(stream);
                break;
            default:
                break;
            }
            break;
        case 250:
        case 251:
            switch (stage)
            {
            case Initial:
                handleInitial(stream);
                break;
            case Auth:
                handleAuth(stream);
                break;
            case AuthLogin:
                handleAuthLogin(stream);
                break;
            case AuthPassword:
                handleAuthPassword(stream);
                break;
            case Sender:
                handleSender(stream);
                break;
            case Receiver:
                handleReceiver(stream);
                break;
            case Data:
                handleData(stream);
                break;
            case Mail:
                handleMail(stream);
                break;
            case Quit:
                handleQuit(stream);
            default:
                break;
            }
            break;
        case 334:
            switch (stage)
            {
            case Auth:
                handleAuth(stream);
                break;
            case AuthLogin:
                handleAuthLogin(stream);
                break;
            case AuthPassword:
                handleAuthPassword(stream);
                break;
            default:
                break;
            }
            break;
        case 354:
            handleMail(stream);
            break;
        case 550:
            handleReceiver(stream);
            break;
        case 421:
        case 450:
        case 451:
        case 452:
        case 500:
        case 501:
        case 502:
        case 503:
        case 504:
        case 551:
        case 552:
        case 553:
        case 554:
        default:
            err = text;
            socket->close();
            return;
        }
        ++stage;
    }
}

/*============================================================================
================================ BSocketWrapper ==============================
============================================================================*/

/*============================== Static public methods =====================*/

bool BSmtpSender::send(const QString &address, const BEmail &email, int timeout, const QString &userName,
                       const QString &userPassword, quint16 port, BGenericSocket::SocketType type,
                       const QString &localHostName, QString *error, bool translationsEnabled)
{
    BSmtpSender s;
    s.setServer(address, port);
    s.setSocketType(type);
    s.setLocalHostName(localHostName);
    s.setUser(userName, userPassword);
    s.setEmail(email);
    if (!s.isValid())
        return bRet(error, translationsEnabled ? tr("Invalid parameters", "errorString") :
                                                 QString("Invalid parameters"), false);
    s.send();
    if (!s.waitForFinished(timeout))
        return bRet(error, translationsEnabled ? tr("Operation timed out", "errorString") :
                                                 QString("Operation timed out"), false);
    if (error)
        *error = s.lastTransferError();
    return s.lastTransferSuccess();
}

/*============================== Public constructors =======================*/

BSmtpSender::BSmtpSender(QObject *parent) :
    QObject(parent), BBaseObject(*new BSmtpSenderPrivate(this))
{
    d_func()->init();
}

BSmtpSender::~BSmtpSender()
{
    //
}

/*============================== Protected constructors ====================*/

BSmtpSender::BSmtpSender(BSmtpSenderPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BSmtpSender::setServer(const QString &address, quint16 port)
{
    if (isBuisy())
        return;
    d_func()->address = address;
    d_func()->port = port;
}

void BSmtpSender::setSocketType(BGenericSocket::SocketType type)
{
    if (isBuisy())
        return;
    d_func()->socketType = type;
}

void BSmtpSender::setLocalHostName(const QString &hostName)
{
    if (isBuisy())
        return;
    d_func()->hostName = hostName;
}

void BSmtpSender::setUser(const QString &name, const QString &password)
{
    if (isBuisy())
        return;
    d_func()->login = name;
    d_func()->password = password;
}

void BSmtpSender::setEmail(const BEmail &email)
{
    if (isBuisy())
        return;
    d_func()->email = email;
}

QString BSmtpSender::serverAddress() const
{
    return d_func()->address;
}

quint16 BSmtpSender::serverPort() const
{
    return d_func()->port;
}

BGenericSocket::SocketType BSmtpSender::socketType() const
{
    return d_func()->socketType;
}

QString BSmtpSender::localHostName() const
{
    return d_func()->hostName;
}

QString BSmtpSender::userName() const
{
    return d_func()->login;
}

QString BSmtpSender::userPassword() const
{
    return d_func()->password;
}

BEmail BSmtpSender::email() const
{
    return d_func()->email;
}

bool BSmtpSender::isValid() const
{
    const B_D(BSmtpSender);
    return !d->address.isEmpty() && d->port && d->email.isValid();
}

bool BSmtpSender::isBuisy() const
{
    return d_func()->buisy;
}

bool BSmtpSender::waitForFinished(int msecs)
{
    if (!isBuisy())
        return true;
    QEventLoop el;
    connect(this, SIGNAL(finished(bool, QString)), &el, SLOT(quit()));
    if (msecs > 0)
        QTimer::singleShot(msecs, &el, SLOT(quit()));
    el.exec();
    return !isBuisy();
}

bool BSmtpSender::lastTransferSuccess() const
{
    return d_func()->success;
}

QString BSmtpSender::lastTransferError() const
{
    return d_func()->err;
}

/*============================== Public slots ==============================*/

void BSmtpSender::send(const BEmail &email)
{
    if (isBuisy())
        return;
    if (email.isValid())
        setEmail(email);
    if (!isValid())
        return;
    B_D(BSmtpSender);
    d->success = false;
    d->err.clear();
    d->buisy = true;
    d->socket = new BGenericSocket(d->socketType, this);
    connect(d->socket, SIGNAL(disconnected()), d, SLOT(disconnected()));
    connect(d->socket, SIGNAL(error(QAbstractSocket::SocketError)), d, SLOT(error(QAbstractSocket::SocketError)));
    connect(d->socket, SIGNAL(readyRead()), d, SLOT(readyRead()));
    if (BGenericSocket::SslSocket == d->socketType)
    {
        connect(d->socket->sslSocket(), SIGNAL(encrypted()), d, SLOT(connected()));
        d->socket->sslSocket()->connectToHostEncrypted(d->address, d->port);
    }
    else
    {
        connect(d->socket, SIGNAL(connected()), d, SLOT(connected()));
        d->socket->connectToHost(d->address, d->port);
    }
}
