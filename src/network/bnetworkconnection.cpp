#include "bnetworkconnection.h"
#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bgenericsocket.h"
#include "bnetworkconnection_p.h"
#include "bnetworkoperation_p.h"

#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BLogger>

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include <QAbstractSocket>
#include <QThread>
#include <QDateTime>
#include <QMetaObject>
#include <QVariant>

/*============================================================================
================================ BNetworkConnectionPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkConnectionPrivate::BNetworkConnectionPrivate(BNetworkConnection *q, BNetworkServer *server) :
    BBasePrivate(q), UniqueId( QUuid::createUuid() ), Server(server)
{
    //
}

BNetworkConnectionPrivate::~BNetworkConnectionPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BNetworkConnectionPrivate::init()
{
    detailedLog = false;
    autoDelete = true;
    logger = 0;
    socketWrapper = new BSocketWrapper( q_func() );
    connect( socketWrapper, SIGNAL( downloadProgress(BNetworkOperationMetaData, qint64, qint64) ),
             this, SLOT( downloadProgress(BNetworkOperationMetaData, qint64, qint64) ) );
    connect( socketWrapper, SIGNAL( uploadProgress(BNetworkOperationMetaData, qint64, qint64) ),
             this, SLOT( uploadProgress(BNetworkOperationMetaData, qint64, qint64) ) );
    connect( socketWrapper, SIGNAL( dataReceived(QByteArray, BNetworkOperationMetaData) ),
             this, SLOT( dataReceived(QByteArray, BNetworkOperationMetaData) ) );
    connect( socketWrapper, SIGNAL( dataSent(BNetworkOperationMetaData) ),
             this, SLOT( dataSent(BNetworkOperationMetaData) ) );
    connect( socketWrapper, SIGNAL( criticalBufferSizeReached() ), q_func(), SIGNAL( criticalBufferSizeReached() ) );
}

void BNetworkConnectionPrivate::setSocket(BGenericSocket *s)
{
    if (!s)
        return;
    socket = s;
    socketWrapper->setSocket(s);
    socket->setParent( q_func() );
    connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
    connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
    connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ),
             this, SLOT( error(QAbstractSocket::SocketError) ) );
}

void BNetworkConnectionPrivate::sendNext()
{
    if ( !q_func()->isConnected() || socketWrapper->isBuisy() || dataQueue.isEmpty() )
        return;
    Data data = dataQueue.dequeue();
    if ( !data.second.isValid() )
        return;
    BNetworkOperation *op = data.second.isRequest() ? requests.value(data.second) : replies.value(data.second);
    if (op)
    {
        BNetworkOperationPrivate *opd = op->d_func();
        socketWrapper->sendData(data.first, data.second) ? opd->setStarted() : opd->setError();
    }
}

BNetworkOperation *BNetworkConnectionPrivate::createOperation(const BNetworkOperationMetaData &metaData)
{
    BNetworkOperation *op = new BNetworkOperation( metaData, q_func() );
    operations.insert(op, metaData);
    connect( op, SIGNAL( destroyed(QObject *) ), this, SLOT( operationDestroyed(QObject *) ) );
    return op;
}

/*============================== Public slots ==============================*/

void BNetworkConnectionPrivate::connected()
{
    B_Q(BNetworkConnection);
    q->log( tr("Connected", "log text") );
    QMetaObject::invokeMethod(q, "connected");
}

void BNetworkConnectionPrivate::disconnected()
{
    B_Q(BNetworkConnection);
    q->log( tr("Disconnected", "log text") );
    QMetaObject::invokeMethod(q, "disconnected");
}

void BNetworkConnectionPrivate::error(QAbstractSocket::SocketError socketError)
{
    B_Q(BNetworkConnection);
    q->log( tr("Error:", "log text") + " " + q->errorString() );
    QMetaObject::invokeMethod( q, "error", Q_ARG(QAbstractSocket::SocketError, socketError) );
}

void BNetworkConnectionPrivate::downloadProgress(const BNetworkOperationMetaData &metaData,
                                                 qint64 bytesReady, qint64 bytesTotal)
{
    B_Q(BNetworkConnection);
    if ( metaData.isRequest() )
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = replies.value(mdat);
        if (op)
        {
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
        }
        else
        {
            op = createOperation(mdat);
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
            replies.insert(mdat, op);
            op->d_func()->setStarted();
            if (detailedLog)
                q->log( tr("Incoming request:", "log text") + " " + metaData.operation() );
            QMetaObject::invokeMethod( q, "incomingRequest", Q_ARG(BNetworkOperation *, op) );
        }
    }
    else
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(true);
        BNetworkOperation *op = requests.value(mdat);
        if (op)
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
    }
}

void BNetworkConnectionPrivate::uploadProgress(const BNetworkOperationMetaData &metaData,
                                               qint64 bytesReady, qint64 bytesTotal)
{
    BNetworkOperation *op = metaData.isRequest() ? requests.value(metaData) : replies.value(metaData);
    if (op)
        op->d_func()->setUploadProgress(bytesReady, bytesTotal);
}

void BNetworkConnectionPrivate::dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    B_Q(BNetworkConnection);
    if ( metaData.isRequest() )
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = replies.value(mdat);
        if (!op)
            return;
        BNetworkOperationPrivate *opd = op->d_func();
        opd->bytesInReady = opd->bytesInTotal;
        opd->data = data;
        if (detailedLog)
            q->log( tr("Request received:", "log text") + " " + metaData.operation() );
        QMetaObject::invokeMethod( q, "requestReceived", Q_ARG(BNetworkOperation *, op) );
        QString opp = op->metaData().operation();
        if ( internalRequestHandlers.contains(opp) )
            return ( q->*internalRequestHandlers.value(opp) )(op);
        if ( externalRequestHandlers.contains(opp) )
            return externalRequestHandlers.value(opp)(op);
        q->handleRequest(op);
    }
    else
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = requests.value(mdat);
        if (!op)
            return;
        BNetworkOperationPrivate *opd = op->d_func();
        requests.remove(mdat);
        opd->bytesInReady = opd->bytesInTotal;
        opd->setFinished(data);
        if (detailedLog)
            q->log( tr("Reply received:", "log text") + " " + metaData.operation() );
        QMetaObject::invokeMethod( q, "replyReceived", Q_ARG(BNetworkOperation *, op) );
        QString opp = op->metaData().operation();
        if ( internalReplyHandlers.contains(opp) )
            return ( q->*internalReplyHandlers.value(opp) )(op);
        if ( externalReplyHandlers.contains(opp) )
            return externalReplyHandlers.value(opp)(op);
        q->handleReply(op);
    }
}

void BNetworkConnectionPrivate::dataSent(const BNetworkOperationMetaData &metaData)
{
    B_Q(BNetworkConnection);
    if ( metaData.isRequest() )
    {
        BNetworkOperation *op = requests.value(metaData);
        if (!op)
            return;
        if (detailedLog)
            q->log( tr("Request sent:", "log text") + " " + metaData.operation() );
        QMetaObject::invokeMethod( q, "requestSent", Q_ARG(BNetworkOperation *, op) );
    }
    else
    {
        BNetworkOperation *op = replies.value(metaData);
        if (!op)
            return;
        replies.remove(metaData);
        op->d_func()->setFinished();
        if (detailedLog)
            q->log( tr("Reply sent:", "log text") + " " + metaData.operation() );
        if (autoDelete)
            op->deleteLater();
        else
            QMetaObject::invokeMethod( q, "replySent", Q_ARG(BNetworkOperation *, op) );
    }
    sendNext();
}

void BNetworkConnectionPrivate::operationDestroyed(QObject *obj)
{
    if ( !operations.contains(obj) )
        return;
    BNetworkOperationMetaData md = operations.take(obj);
    if ( md.isRequest() )
        requests.remove(md);
    else
        replies.remove(md);
}

/*============================================================================
================================ BNetworkConnection ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkConnection::BNetworkConnection(BGenericSocket *socket, QObject *parent) :
    QObject(parent), BBase( *new BNetworkConnectionPrivate(this) )
{
    d_func()->init();
    if ( !socket || socket->thread() != thread() || !socket->isOpen() )
        return;
    d_func()->setSocket(socket);
    log( tr("Incoming connection", "log text") );
}

BNetworkConnection::BNetworkConnection(BNetworkServer *server, BGenericSocket *socket) :
    QObject(0), BBase( *new BNetworkConnectionPrivate(this, server) )
{
    d_func()->init();
    if ( !socket || socket->thread() != thread() || !socket->isOpen() )
        return;
    d_func()->setSocket(socket);
    log( tr("Incoming connection", "log text") );
}

BNetworkConnection::BNetworkConnection(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), BBase( *new BNetworkConnectionPrivate(this) )
{
    d_func()->init();
    BGenericSocket *s = new BGenericSocket(type);
    if ( s->isSocketSet() )
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
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BNetworkConnection::setCompressionLevel(int level)
{
    d_func()->socketWrapper->setCompressionLevel(level);
}

void BNetworkConnection::setCriticalBufferSize(qint64 size)
{
    d_func()->socketWrapper->setCriticalBufferSize(size);
}

void BNetworkConnection::setCloseOnCriticalBufferSize(bool close)
{
    d_func()->socketWrapper->setCloseOnCriticalBufferSize(close);
}

void BNetworkConnection::setDetailedLogMode(bool enabled)
{
    d_func()->detailedLog = enabled;
}

void BNetworkConnection::setAutoDeleteSentReplies(bool enabled)
{
    d_func()->autoDelete = enabled;
}

void BNetworkConnection::connectToHost(const QString &hostName, quint16 port)
{
    if ( isConnected() || hostName.isEmpty() )
        return;
    d_func()->socket->connectToHost(hostName, port);
}

bool BNetworkConnection::connectToHostBlocking(const QString &hostName, quint16 port, int msecs)
{
    if ( isConnected() || hostName.isEmpty() )
        return false;
    B_D(BNetworkConnection);
    d->socket->connectToHost(hostName, port);
    return d->socket->waitForConnected(msecs);
}

void BNetworkConnection::disconnectFromHost()
{
    d_func()->socket->disconnectFromHost();
}

bool BNetworkConnection::disconnectFromHostBlocking(int msecs)
{
    B_D(BNetworkConnection);
    d->socket->disconnectFromHost();
    return d->socket->waitForDisconnected(msecs);
}

void BNetworkConnection::close()
{
    d_func()->socket->close();
}

void BNetworkConnection::abort()
{
    d_func()->socket->abort();
}

void BNetworkConnection::installReplyHandler(const QString &operation, InternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->internalReplyHandlers.contains(operation) )
        return;
    d->internalReplyHandlers.insert(operation, handler);
}

void BNetworkConnection::installReplyHandler(const QString &operation, ExternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->externalReplyHandlers.contains(operation) )
        return;
    d->externalReplyHandlers.insert(operation, handler);
}

void BNetworkConnection::installRequestHandler(const QString &operation, InternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->internalRequestHandlers.contains(operation) )
        return;
    d->internalRequestHandlers.insert(operation, handler);
}

void BNetworkConnection::installRequestHandler(const QString &operation, ExternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->externalRequestHandlers.contains(operation) )
        return;
    d->externalRequestHandlers.insert(operation, handler);
}

bool BNetworkConnection::isValid() const
{
    const B_D(BNetworkConnection);
    return !d->socket.isNull() && d->socket->isSocketSet();
}

bool BNetworkConnection::isConnected() const
{
    return isValid() && d_func()->socket->state() == QAbstractSocket::ConnectedState;
}

const QUuid BNetworkConnection::uniqueId() const
{
    return d_func()->UniqueId;
}

BNetworkServer *BNetworkConnection::server() const
{
    return d_func()->Server;
}

QAbstractSocket::SocketError BNetworkConnection::error() const
{
    return isValid() ? d_func()->socket->error() : QAbstractSocket::UnknownSocketError;
}

QString BNetworkConnection::errorString() const
{
    return isValid() ? d_func()->socket->errorString() : "";
}

int BNetworkConnection::compressionLevel() const
{
    return d_func()->socketWrapper->compressionLevel();
}

qint64 BNetworkConnection::criticalBufferSize() const
{
    return d_func()->socketWrapper->criticalBufferSize();
}

bool BNetworkConnection::closeOnCriticalBufferSize() const
{
    return d_func()->socketWrapper->closeOnCriticalBufferSize();
}

bool BNetworkConnection::detailedLogMode() const
{
    return d_func()->detailedLog;
}

bool BNetworkConnection::autoDeleteSentReplies() const
{
    return d_func()->autoDelete;
}

QString BNetworkConnection::peerAddress() const
{
    return isValid() ? d_func()->socket->peerAddress() : "";
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &operation, const QByteArray &data)
{
    if ( !isConnected() || operation.isEmpty() )
        return 0;
    B_D(BNetworkConnection);
    BNetworkConnectionPrivate::Data dat;
    dat.first = data;
    dat.second.setId( QUuid::createUuid() );
    dat.second.setIsRequest(true);
    dat.second.setOperation(operation);
    BNetworkOperation *op = d->createOperation(dat.second);
    d->requests.insert(dat.second, op);
    d->dataQueue.enqueue(dat);
    d->sendNext();
    return op;
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &operation, const QVariant &variant)
{
    return sendRequest( operation, BSocketWrapper::variantToData(variant) );
}

bool BNetworkConnection::sendReply(BNetworkOperation *operation, const QByteArray &data)
{
    if ( !isConnected() || !operation || !operation->isValid() || operation->isRequest() )
        return false;
    B_D(BNetworkConnection);
    BNetworkConnectionPrivate::Data dat;
    dat.first = data;
    dat.second = operation->metaData();
    d->dataQueue.enqueue(dat);
    d->sendNext();
    return true;
}

bool BNetworkConnection::sendReply(BNetworkOperation *operation, const QVariant &variant)
{
    return sendReply( operation, BSocketWrapper::variantToData(variant) );
}

/*============================== Protected methods =========================*/

void BNetworkConnection::handleReply(BNetworkOperation *)
{
    //
}

void BNetworkConnection::handleRequest(BNetworkOperation *)
{
    //
}

void BNetworkConnection::log(const QString &text, bool noLevel)
{
    BLogger *logger = d_func()->logger;
    if (!logger)
        logger = BCoreApplication::logger();
    if (!logger)
        return;
    QString msg = "[" + peerAddress() + "] " + text;
    logger->log(msg, noLevel ? BLogger::NoLevel : BLogger::InfoLevel);
}

BGenericSocket *BNetworkConnection::socket() const
{
    return d_func()->socket.data();
}

BSocketWrapper *BNetworkConnection::socketWrapper() const
{
    return d_func()->socketWrapper.data();
}
