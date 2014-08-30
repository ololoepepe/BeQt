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

#include "bnetworkconnection.h"
#include "bnetworkconnection_p.h"

#include "bgenericsocket.h"
#include "bnetworkoperation.h"
#include "bnetworkoperation_p.h"
#include "bsocketwrapper.h"

#include <BeQtCore/BApplicationBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/BLogger>
#include <BeQtCore/BTerminal>
#include <BeQtCore/BUuid>

#include <QAbstractSocket>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVariant>

/*============================================================================
================================ BNetworkConnectionPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkConnectionPrivate::BNetworkConnectionPrivate(BNetworkConnection *q, BNetworkServer *server) :
    BBaseObjectPrivate(q), UniqueId(BUuid::createUuid()), Server(server)
{
    //
}

BNetworkConnectionPrivate::~BNetworkConnectionPrivate()
{
    //
}

/*============================== Public methods ============================*/

BNetworkOperation *BNetworkConnectionPrivate::createOperation(const BNetworkOperationMetaData &metaData)
{
    BNetworkOperation *op = new BNetworkOperation(metaData, q_func());
    operations.insert(op, metaData);
    if (autoDelete && !metaData.isRequest())
        op->setAutoDelete(true);
    connect(op, SIGNAL(canceled()), this, SLOT(operationCanceled()));
    connect(op, SIGNAL(destroyed(QObject *)), this, SLOT(operationDestroyed(QObject *)));
    return op;
}

void BNetworkConnectionPrivate::init()
{
    loggingMode = BNetworkConnection::NormalLogging;
    autoDelete = true;
    logger = 0;
    translations = false;
    socketWrapper = new BSocketWrapper(q_func());
    connect(socketWrapper, SIGNAL(downloadProgress(BNetworkOperationMetaData, qint64, qint64)),
            this, SLOT(downloadProgress(BNetworkOperationMetaData, qint64, qint64)));
    connect(socketWrapper, SIGNAL(uploadProgress(BNetworkOperationMetaData, qint64, qint64)),
            this, SLOT(uploadProgress(BNetworkOperationMetaData, qint64, qint64)));
    connect(socketWrapper, SIGNAL(dataReceived(QByteArray, BNetworkOperationMetaData)),
            this, SLOT(dataReceived(QByteArray, BNetworkOperationMetaData)));
    connect(socketWrapper, SIGNAL(dataSent(BNetworkOperationMetaData)),
            this, SLOT(dataSent(BNetworkOperationMetaData)));
    connect(socketWrapper, SIGNAL(criticalBufferSizeReached()), q_func(), SIGNAL(criticalBufferSizeReached()));
}

void BNetworkConnectionPrivate::sendNext()
{
    if (!q_func()->isConnected() || socketWrapper->isBuisy() || dataQueue.isEmpty())
        return;
    Data data = dataQueue.dequeue();
    if (!data.metaData.isValid())
        return;
    BNetworkOperation *op = data.metaData.isRequest() ? requests.value(data.metaData) : replies.value(data.metaData);
    if (op) {
        BNetworkOperationPrivate *opd = op->d_func();
        socketWrapper->sendData(data.data, data.compressionLevel, data.metaData) ? opd->setStarted() : opd->setError();
    }
}

bool BNetworkConnectionPrivate::sendReply(BNetworkOperation *op, const QByteArray &data)
{
    return sendReply(op, socketWrapper->compressionLevel(), data);
}

bool BNetworkConnectionPrivate::sendReply(BNetworkOperation *op, int compressionLevel, const QByteArray &data)
{
    if (!q_func()->isConnected() || !op || !op->isValid() || op->isRequest())
        return false;
    if (compressionLevel < 0)
        compressionLevel = -1;
    else if (compressionLevel > 9)
        compressionLevel = 9;
    Data dat;
    dat.data = data;
    dat.metaData = op->metaData();
    dataQueue.enqueue(dat);
    sendNext();
    return true;
}

BNetworkOperation *BNetworkConnectionPrivate::sendRequest(const QString &op, const QByteArray &data)
{
    return sendRequest(op, socketWrapper->compressionLevel(), data);
}

BNetworkOperation *BNetworkConnectionPrivate::sendRequest(const QString &op, int compressionLevel,
                                                          const QByteArray &data)
{
    if (!q_func()->isConnected() || op.isEmpty())
        return 0;
    if (compressionLevel < 0)
        compressionLevel = -1;
    else if (compressionLevel > 9)
        compressionLevel = 9;
    Data dat;
    dat.data = data;
    dat.metaData.setId(BUuid::createUuid());
    dat.metaData.setIsRequest(true);
    dat.metaData.setOperation(op);
    BNetworkOperation *nop = createOperation(dat.metaData);
    requests.insert(dat.metaData, nop);
    dataQueue.enqueue(dat);
    sendNext();
    return nop;
}

void BNetworkConnectionPrivate::setSocket(BGenericSocket *s)
{
    if (!s)
        return;
    socket = s;
    socketWrapper->setSocket(s);
    socket->setParent( q_func() );
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
}

/*============================== Public slots ==============================*/

void BNetworkConnectionPrivate::connected()
{
    B_Q(BNetworkConnection);
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q->log(translations ? tr("Connected", "log text") : QString("Connected"));
    QMetaObject::invokeMethod(q, "connected");
}

void BNetworkConnectionPrivate::dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    B_Q(BNetworkConnection);
    if (metaData.isRequest()) {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = replies.value(mdat);
        if (!op)
            return;
        BNetworkOperationPrivate *opd = op->d_func();
        opd->bytesInReady = opd->bytesInTotal;
        opd->data = data;
        if (loggingMode >= BNetworkConnection::DetailedLogging) {
            q->log((translations ? tr("Request received:", "log text") : QString("Request received:")) + " "
                   + metaData.operation());
        }
        QMetaObject::invokeMethod(q, "requestReceived", Q_ARG(BNetworkOperation *, op));
        QString opp = op->metaData().operation();
        if (internalRequestHandlers.contains(opp))
            (q->*internalRequestHandlers.value(opp))(op);
        else if (externalRequestHandlers.contains(opp))
            externalRequestHandlers.value(opp)(op);
        else
            q->handleRequest(op);
    } else {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = requests.value(mdat);
        if (!op)
            return;
        BNetworkOperationPrivate *opd = op->d_func();
        requests.remove(mdat);
        opd->bytesInReady = opd->bytesInTotal;
        opd->setFinished(data);
        if (loggingMode >= BNetworkConnection::DetailedLogging) {
            q->log((translations ? tr("Reply received:", "log text") : QString("Reply received:")) + " "
                   + metaData.operation());
        }
        QMetaObject::invokeMethod(q, "replyReceived", Q_ARG(BNetworkOperation *, op));
        QString opp = op->metaData().operation();
        if (internalReplyHandlers.contains(opp))
            (q->*internalReplyHandlers.value(opp))(op);
        else if (externalReplyHandlers.contains(opp))
            externalReplyHandlers.value(opp)(op);
        else
            q->handleReply(op);
    }
}

void BNetworkConnectionPrivate::dataSent(const BNetworkOperationMetaData &metaData)
{
    B_Q(BNetworkConnection);
    if (metaData.isRequest()) {
        BNetworkOperation *op = requests.value(metaData);
        if (!op)
            return;
        if (loggingMode >= BNetworkConnection::DetailedLogging) {
            q->log((translations ? tr("Request sent:", "log text") : QString("Request sent:")) + " "
                   + metaData.operation());
        }
        QMetaObject::invokeMethod(q, "requestSent", Q_ARG(BNetworkOperation *, op));
    } else {
        BNetworkOperation *op = replies.value(metaData);
        if (!op)
            return;
        replies.remove(metaData);
        op->d_func()->setFinished();
        if (loggingMode >= BNetworkConnection::DetailedLogging) {
            q->log((translations ? tr("Reply sent:", "log text") : QString("Reply sent:")) + " "
                   + metaData.operation());
        }
        QMetaObject::invokeMethod(q, "replySent", Q_ARG(BNetworkOperation *, op));
    }
    sendNext();
}

void BNetworkConnectionPrivate::disconnected()
{
    B_Q(BNetworkConnection);
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q->log(translations ? tr("Disconnected", "log text") : QString("Disconnected"));
    QMetaObject::invokeMethod(q, "disconnected");
    foreach (BNetworkOperation *op, QList<BNetworkOperation *>() << requests.values() << replies.values()) {
        if (!op->isFinished() && !op->isError())
            op->d_func()->setError();
    }
}

void BNetworkConnectionPrivate::downloadProgress(const BNetworkOperationMetaData &metaData,
                                                 qint64 bytesReady, qint64 bytesTotal)
{
    B_Q(BNetworkConnection);
    if (metaData.isRequest()) {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = replies.value(mdat);
        if (op) {
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
        } else {
            op = createOperation(mdat);
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
            replies.insert(mdat, op);
            op->d_func()->setStarted();
            if (loggingMode >= BNetworkConnection::DetailedLogging) {
                q->log((translations ? tr("Incoming request:", "log text") : QString("Incoming request:")) + " "
                       + metaData.operation());
            }
            QMetaObject::invokeMethod(q, "incomingRequest", Q_ARG(BNetworkOperation *, op));
        }
    } else {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(true);
        BNetworkOperation *op = requests.value(mdat);
        if (op)
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
    }
}

void BNetworkConnectionPrivate::error(QAbstractSocket::SocketError socketError)
{
    B_Q(BNetworkConnection);
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q->log((translations ? tr("Error:", "log text") : QString("Error:")) + " " + q->errorString());
    QMetaObject::invokeMethod(q, "error", Q_ARG(QAbstractSocket::SocketError, socketError));
    foreach (BNetworkOperation *op, QList<BNetworkOperation *>() << requests.values() << replies.values()) {
        if (!op->isFinished() && !op->isError())
            op->d_func()->setError();
    }
}

void BNetworkConnectionPrivate::logIncomingConnection()
{
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q_func()->log(translations ? tr("Incoming connection", "log text") : QString("Incoming connection"));
}

void BNetworkConnectionPrivate::operationCanceled()
{
    q_func()->abort();
    foreach (BNetworkOperation *op, QList<BNetworkOperation *>() << requests.values() << replies.values()) {
        if (!op->isFinished() && !op->isError())
            op->d_func()->setError();
    }
}

void BNetworkConnectionPrivate::operationDestroyed(QObject *obj)
{
    if (!operations.contains(obj))
        return;
    BNetworkOperationMetaData md = operations.take(obj);
    if (md.isRequest())
        requests.remove(md);
    else
        replies.remove(md);
}

void BNetworkConnectionPrivate::uploadProgress(const BNetworkOperationMetaData &metaData,
                                               qint64 bytesReady, qint64 bytesTotal)
{
    BNetworkOperation *op = metaData.isRequest() ? requests.value(metaData) : replies.value(metaData);
    if (op)
        op->d_func()->setUploadProgress(bytesReady, bytesTotal);
}

/*============================================================================
================================ BNetworkConnection ==========================
============================================================================*/

/*============================== Public static methods =====================*/

QString BNetworkConnection::operation(StandardOperation op)
{
    switch (op) {
    case NoopOperation:
        return "noop";
    case WriteOperation:
        return "write";
    case LogOperation:
        return "log";
    default:
        return "";
    }
}

BNetworkConnection::InternalHandler BNetworkConnection::replyHandler(StandardOperation op)
{
    switch (op) {
    default:
        return 0;
    }
}

BNetworkConnection::InternalHandler BNetworkConnection::requestHandler(StandardOperation op)
{
    switch (op) {
    case NoopOperation:
        return &BNetworkConnection::handleNoopRequest;
    case WriteOperation:
        return &BNetworkConnection::handleWriteRequest;
    case LogOperation:
        return &BNetworkConnection::handleLogRequest;
    default:
        return 0;
    }
}

/*============================== Public constructors =======================*/

BNetworkConnection::BNetworkConnection(BGenericSocket *socket, QObject *parent) :
    QObject(parent), BBaseObject(*new BNetworkConnectionPrivate(this))
{
    d_func()->init();
    if (!socket || (socket->thread() != thread()) || !socket->isOpen())
        return;
    d_func()->setSocket(socket);
    QTimer::singleShot(0, d_func(), SLOT(logIncomingConnection()));
}

BNetworkConnection::BNetworkConnection(BNetworkServer *server, BGenericSocket *socket) :
    QObject(0), BBaseObject(*new BNetworkConnectionPrivate(this, server))
{
    d_func()->init();
    if (!socket || (socket->thread() != thread()) || !socket->isOpen())
        return;
    d_func()->setSocket(socket);
    QTimer::singleShot(0, d_func(), SLOT(logIncomingConnection()));
}

BNetworkConnection::BNetworkConnection(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), BBaseObject(*new BNetworkConnectionPrivate(this))
{
    d_func()->init();
    BGenericSocket *s = new BGenericSocket(type);
    if (s->isSocketSet())
        d_func()->setSocket(s);
    else
        s->deleteLater();
}

BNetworkConnection::~BNetworkConnection()
{
    //
}

/*============================== Protected constructors ====================*/

BNetworkConnection::BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BNetworkConnection::autoDeleteSentReplies() const
{
    return d_func()->autoDelete;
}

bool BNetworkConnection::closeOnCriticalBufferSize() const
{
    return d_func()->socketWrapper->closeOnCriticalBufferSize();
}

int BNetworkConnection::compressionLevel() const
{
    return d_func()->socketWrapper->compressionLevel();
}

void BNetworkConnection::connectToHost(const QString &hostName, quint16 port)
{
    if (isConnected() || hostName.isEmpty())
        return;
    d_func()->socket->connectToHost(hostName, port);
}

bool BNetworkConnection::connectToHostBlocking(const QString &hostName, quint16 port, int msecs)
{
    if (isConnected() || hostName.isEmpty())
        return false;
    B_D(BNetworkConnection);
    d->socket->connectToHost(hostName, port);
    return d->socket->waitForConnected(msecs);
}

qint64 BNetworkConnection::criticalBufferSize() const
{
    return d_func()->socketWrapper->criticalBufferSize();
}

bool BNetworkConnection::disconnectFromHostBlocking(int msecs)
{
    B_D(BNetworkConnection);
    d->socket->disconnectFromHost();
    return d->socket->waitForDisconnected(msecs);
}

QAbstractSocket::SocketError BNetworkConnection::error() const
{
    return isValid() ? d_func()->socket->error() : QAbstractSocket::UnknownSocketError;
}

QString BNetworkConnection::errorString() const
{
    return isValid() ? d_func()->socket->errorString() : "";
}

void BNetworkConnection::installReplyHandler(const QString &operation, InternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if (d->internalReplyHandlers.contains(operation))
        return;
    d->internalReplyHandlers.insert(operation, handler);
}

void BNetworkConnection::installReplyHandler(const QString &operation, ExternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if (d->externalReplyHandlers.contains(operation))
        return;
    d->externalReplyHandlers.insert(operation, handler);
}

void BNetworkConnection::installReplyHandler(StandardOperation op)
{
    installReplyHandler(operation(op), replyHandler(op));
}

void BNetworkConnection::installRequestHandler(const QString &operation, InternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if (d->internalRequestHandlers.contains(operation))
        return;
    d->internalRequestHandlers.insert(operation, handler);
}

void BNetworkConnection::installRequestHandler(const QString &operation, ExternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if (d->externalRequestHandlers.contains(operation))
        return;
    d->externalRequestHandlers.insert(operation, handler);
}

void BNetworkConnection::installRequestHandler(StandardOperation op)
{
    installRequestHandler(operation(op), requestHandler(op));
}

void BNetworkConnection::installRequestHandler(StandardOperation op, InternalHandler handler)
{
    installRequestHandler(operation(op), handler);
}

void BNetworkConnection::installRequestHandler(StandardOperation op, ExternalHandler handler)
{
    installRequestHandler(operation(op), handler);
}

bool BNetworkConnection::isConnected() const
{
    return (isValid() && (d_func()->socket->state() == QAbstractSocket::ConnectedState));
}

bool BNetworkConnection::isValid() const
{
    const B_D(BNetworkConnection);
    return (!d->socket.isNull() && d->socket->isSocketSet());
}

BLogger *BNetworkConnection::logger() const
{
    return d_func()->logger;
}

BNetworkConnection::LoggingMode BNetworkConnection::loggingMode() const
{
    return d_func()->loggingMode;
}

QString BNetworkConnection::peerAddress() const
{
    return isValid() ? d_func()->socket->peerAddress() : "";
}

bool BNetworkConnection::sendReply(BNetworkOperation *op, const QByteArray &data)
{
    return d_func()->sendReply(op, data);
}

bool BNetworkConnection::sendReply(BNetworkOperation *op, const QVariant &variant)
{
    return d_func()->sendReply(op, BeQt::serialize(variant));
}

bool BNetworkConnection::sendReply(BNetworkOperation *op, int compressionLevel, const QByteArray &data)
{
    return d_func()->sendReply(op, compressionLevel, data);
}

bool BNetworkConnection::sendReply(BNetworkOperation *op, int compressionLevel, const QVariant &variant)
{
    return d_func()->sendReply(op, compressionLevel, BeQt::serialize(variant));
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &op, const QByteArray &data)
{
    return d_func()->sendRequest(op, data);
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &op, const QVariant &variant)
{
    return d_func()->sendRequest(op, BeQt::serialize(variant));
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &op, int compressionLevel, const QByteArray &data)
{
    return d_func()->sendRequest(op, compressionLevel, data);
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &op, int compressionLevel, const QVariant &variant)
{
    return d_func()->sendRequest(op, compressionLevel, BeQt::serialize(variant));
}

BNetworkServer *BNetworkConnection::server() const
{
    return d_func()->Server;
}

void BNetworkConnection::setAutoDeleteSentReplies(bool enabled)
{
    d_func()->autoDelete = enabled;
}

void BNetworkConnection::setCloseOnCriticalBufferSize(bool close)
{
    d_func()->socketWrapper->setCloseOnCriticalBufferSize(close);
}

void BNetworkConnection::setCompressionLevel(int level)
{
    d_func()->socketWrapper->setCompressionLevel(level);
}

void BNetworkConnection::setCriticalBufferSize(qint64 size)
{
    d_func()->socketWrapper->setCriticalBufferSize(size);
}

void BNetworkConnection::setLogger(BLogger *l)
{
    B_D(BNetworkConnection);
    if (d->logger && (!d->logger->parent() || (d->logger->parent() == this)))
        d->logger->deleteLater();
    d->logger = l;
    if (l && !l->parent())
        l->setParent(this);
}

void BNetworkConnection::setLoggingMode(LoggingMode m)
{
    d_func()->loggingMode = m;
}

void BNetworkConnection::setTranslationsEnabled(bool enabled)
{
    d_func()->translations = enabled;
}

bool BNetworkConnection::translationsEnabled() const
{
    return d_func()->translations;
}

BUuid BNetworkConnection::uniqueId() const
{
    return d_func()->UniqueId;
}

bool BNetworkConnection::waitForConnected(int msecs)
{
    return d_func()->socket->waitForConnected(msecs);
}

bool BNetworkConnection::waitForDisconnected(int msecs)
{
    return d_func()->socket->waitForDisconnected(msecs);
}

/*============================== Public slots ==============================*/

void BNetworkConnection::abort()
{
    d_func()->socket->abort();
}

void BNetworkConnection::close()
{
    d_func()->socket->close();
}

void BNetworkConnection::disconnectFromHost()
{
    d_func()->socket->disconnectFromHost();
}

/*============================== Protected methods =========================*/

bool BNetworkConnection::handleNoopRequest(BNetworkOperation *op)
{
    op->reply();
    op->setAutoDelete(true);
    op->setStartTimeout();
    op->setFinishTimeout();
    return true;
}

bool BNetworkConnection::handleLogRequest(BNetworkOperation *op)
{
    QVariantMap m = op->variantData().toMap();
    QString text = m.value("text").toString();
    BLogger::Level lvl = enum_cast<BLogger::Level>(m.value("level"), BLogger::NoLevel, BLogger::FatalLevel);
    log(text, lvl);
    op->reply();
    op->setAutoDelete(true);
    op->setStartTimeout();
    op->setFinishTimeout();
    return true;
}

bool BNetworkConnection::handleReply(BNetworkOperation *)
{
    if (d_func()->loggingMode >= DetailedLogging)
        log(d_func()->translations ? tr("Unknown reply", "log text") : QString("Unknown reply"));
    return false;
}

bool BNetworkConnection::handleRequest(BNetworkOperation *)
{
    if (d_func()->loggingMode >= DetailedLogging)
        log(d_func()->translations ? tr("Unknown request", "log text") : QString("Unknown request"));
    return false;
}

bool BNetworkConnection::handleWriteRequest(BNetworkOperation *op)
{
    QVariantMap m = op->variantData().toMap();
    QString text = m.value("text").toString();
    BTerminal::write(text);
    op->reply();
    op->setAutoDelete(true);
    op->setStartTimeout();
    op->setFinishTimeout();
    return true;
}

void BNetworkConnection::log(const QString &text, BLogger::Level lvl)
{
    QString msg = "[" + peerAddress() + "] " + text;
    BLogger *logger = d_func()->logger ? d_func()->logger : BApplicationBase::logger();
    if (logger)
        logger->log(msg, lvl);
    else if (BLogger::isStderrLevel(lvl))
        BTerminal::writeLineErr(msg);
    else
        BTerminal::writeLine(msg);
}

BGenericSocket *BNetworkConnection::socket() const
{
    return d_func()->socket.data();
}

BSocketWrapper *BNetworkConnection::socketWrapper() const
{
    return d_func()->socketWrapper.data();
}
