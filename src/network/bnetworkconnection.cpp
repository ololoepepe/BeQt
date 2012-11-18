#include "bnetworkconnection.h"
#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bgenericsocket.h"
#include "bnetworkconnection_p.h"
#include "bnetworkoperation_p.h"

#include <BeQtCore/BTerminalIOHandler>

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include <QAbstractSocket>
#include <QThread>
#include <QDateTime>
#include <QMetaObject>

/*============================================================================
================================ Network Connection Private Object
============================================================================*/

BNetworkConnectionPrivateObject::BNetworkConnectionPrivateObject(BNetworkConnectionPrivate *p) :
    BBasePrivateObject(p)
{
    //
}

BNetworkConnectionPrivateObject::~BNetworkConnectionPrivateObject()
{
    //
}

//

void BNetworkConnectionPrivateObject::connected()
{
    p_func()->connected();
}

void BNetworkConnectionPrivateObject::disconnected()
{
    p_func()->disconnected();
}

void BNetworkConnectionPrivateObject::error(QAbstractSocket::SocketError socketError)
{
    p_func()->error(socketError);
}

void BNetworkConnectionPrivateObject::downloadProgress(const BNetworkOperationMetaData &metaData,
                                                       qint64 bytesReady, qint64 bytesTotal)
{
    p_func()->downloadProgress(metaData, bytesReady, bytesTotal);
}

void BNetworkConnectionPrivateObject::uploadProgress(const BNetworkOperationMetaData &metaData,
                                                     qint64 bytesReady, qint64 bytesTotal)
{
    p_func()->uploadProgress(metaData, bytesReady, bytesTotal);
}

void BNetworkConnectionPrivateObject::dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    p_func()->dataReceived(data, metaData);
}

void BNetworkConnectionPrivateObject::dataSent(const BNetworkOperationMetaData &metaData)
{
    p_func()->dataSent(metaData);
}

void BNetworkConnectionPrivateObject::operationDestroyed(const BNetworkOperationMetaData &metaData)
{
    p_func()->operationDestroyed(metaData);
}

/*============================================================================
================================ Network Connection Private
============================================================================*/

BNetworkConnectionPrivate::BNetworkConnectionPrivate(BNetworkConnection *q, BGenericSocket *s) :
    BBasePrivate( *q, *new BNetworkConnectionPrivateObject(this) ), UniqueId( QUuid::createUuid() )
{
    init();
    if ( !s || s->thread() != q_func()->thread() || !s->isOpen() )
        return;
    setSocket(s);
    q_func()->log( tr("Incoming connection", "log text") );
}

BNetworkConnectionPrivate::BNetworkConnectionPrivate(BNetworkConnection *q, BGenericSocket::SocketType type) :
    BBasePrivate( *q, *new BNetworkConnectionPrivateObject(this) ), UniqueId( QUuid::createUuid() )
{
    init();
    BGenericSocket *s = new BGenericSocket(type);
    if ( s->isSocketSet() )
        setSocket(s);
    else
        s->deleteLater();
}

BNetworkConnectionPrivate::~BNetworkConnectionPrivate()
{
    //
}

//

void BNetworkConnectionPrivate::init()
{
    detailedLog = false;
    autoDelete = true;
    socketWrapper = new BSocketWrapper( q_func() );
    QObject::connect( socketWrapper, SIGNAL( downloadProgress(BNetworkOperationMetaData, qint64, qint64) ),
                      o_func(), SLOT( downloadProgress(BNetworkOperationMetaData, qint64, qint64) ) );
    QObject::connect( socketWrapper, SIGNAL( uploadProgress(BNetworkOperationMetaData, qint64, qint64) ),
                      o_func(), SLOT( uploadProgress(BNetworkOperationMetaData, qint64, qint64) ) );
    QObject::connect( socketWrapper, SIGNAL( dataReceived(QByteArray, BNetworkOperationMetaData) ),
                      o_func(), SLOT( dataReceived(QByteArray, BNetworkOperationMetaData) ) );
    QObject::connect( socketWrapper, SIGNAL( dataSent(BNetworkOperationMetaData) ),
                      o_func(), SLOT( dataSent(BNetworkOperationMetaData) ) );
    QObject::connect( socketWrapper, SIGNAL( criticalBufferSizeReached() ),
                      o_func(), SIGNAL( criticalBufferSizeReached() ) );
}

void BNetworkConnectionPrivate::setSocket(BGenericSocket *s)
{
    if (!s)
        return;
    socket = s;
    socketWrapper->setSocket(s);
    socket->setParent( q_func() );
    QObject::connect( socket, SIGNAL( connected() ), o_func(), SLOT( connected() ) );
    QObject::connect( socket, SIGNAL( disconnected() ), o_func(), SLOT( disconnected() ) );
    QObject::connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ),
                      o_func(), SLOT( error(QAbstractSocket::SocketError) ) );
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
            op = new BNetworkOperation(mdat, q);
            //QObject::connect( op, SIGNAL( destroyed(QObject *) ), this, SLOT( _m_operationDestroyed(QObject *) ) );
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

void BNetworkConnectionPrivate::operationDestroyed(const BNetworkOperationMetaData &metaData)
{
    if ( metaData.isRequest() )
        requests.remove(metaData);
    else
        replies.remove(metaData);
}

BNetworkOperation *BNetworkConnectionPrivate::createOperation(const BNetworkOperationMetaData &metaData)
{
    return new BNetworkOperation( metaData, q_func() );
}

//

BNetworkConnectionPrivate::BNetworkConnectionPrivate(BNetworkConnection &q, BNetworkConnectionPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Network Connection
============================================================================*/

BNetworkConnection::BNetworkConnection(BGenericSocket *socket, QObject *parent) :
    QObject(parent), BBase( *new BNetworkConnectionPrivate(this, socket) )
{
    //
}

BNetworkConnection::BNetworkConnection(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), BBase( *new BNetworkConnectionPrivate(this, type) )
{
    //
}

//

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
    if ( !isConnected() )
        return;
    d_func()->socket->disconnectFromHost();
}

bool BNetworkConnection::disconnectFromHostBlocking(int msecs)
{
    if ( !isConnected() )
        return true;
    B_D(BNetworkConnection);
    d->socket->disconnectFromHost();
    return d->socket->waitForDisconnected(msecs);
}

void BNetworkConnection::close()
{
    if ( !isConnected() )
        return;
    d_func()->socket->close();
}

bool BNetworkConnection::isValid() const
{
    const B_D(BNetworkConnection);
    return !d->socket.isNull() && d->socket->isSocketSet();
}

bool BNetworkConnection::isConnected() const
{
    return isValid() && d_func()->socket->isOpen();
}

const QUuid BNetworkConnection::uniqueId() const
{
    return d_func()->UniqueId;
}

QAbstractSocket::SocketError BNetworkConnection::error() const
{
    return isValid() ? d_func()->socket->error() : QAbstractSocket::UnknownSocketError;
}

QString BNetworkConnection::errorString() const
{
    return isValid() ? d_func()->socket->errorString() : "";
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
    //connect( op, SIGNAL( destroyed(QObject *) ), d->_m_o, SLOT( operationDestroyed(QObject *) ) );
    d->requests.insert(dat.second, op);
    d->dataQueue.enqueue(dat);
    d->sendNext();
    return op;
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

//

BNetworkConnection::BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}

//

void BNetworkConnection::log(const QString &text)
{
    BTerminalIOHandler::writeLine("[" + QDateTime::currentDateTime().toString("dd/MMM/yyy hh:mm:ss") + "] [" +
                                  peerAddress() + "] " + text);
}
