#include "bnetworkconnection.h"
#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bgenericsocket.h"

#include <BeQtCore/BTerminalIOHandler>

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include <QAbstractSocket>
#include <QThread>
#include <QDateTime>

BNetworkConnection::BNetworkConnection(BGenericSocket *socket, QObject *parent) :
    QObject(parent), _m_CUniqueId( QUuid::createUuid() )
{
    _m_init();
    if ( !socket || socket->thread() != thread() || !socket->isOpen() )
        return;
    _m_setSocket(socket);
    log( tr("Incoming connection", "log text") );
}

BNetworkConnection::BNetworkConnection(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), _m_CUniqueId( QUuid::createUuid() )
{
    _m_init();
    BGenericSocket *socket = new BGenericSocket(type);
    if ( !socket->isSocketSet() )
    {
        socket->deleteLater();
        return;
    }
    _m_setSocket(socket);
}

//

void BNetworkConnection::setCriticalBufferSize(qint64 size)
{
    _m_socketWrapper->setCriticalBufferSize(size);
}

void BNetworkConnection::setCloseOnCriticalBufferSize(bool close)
{
    _m_socketWrapper->setCloseOnCriticalBufferSize(close);
}

void BNetworkConnection::setDetailedLogMode(bool enabled)
{
    _m_detailedLog = enabled;
}

void BNetworkConnection::setAutoDeleteSentReplies(bool enabled)
{
    _m_autoDelete = enabled;
}

void BNetworkConnection::connectToHost(const QString &hostName, quint16 port)
{
    if ( isConnected() || hostName.isEmpty() )
        return;
    _m_socket->connectToHost(hostName, port);
}

bool BNetworkConnection::connectToHostBlocking(const QString &hostName, quint16 port, int msecs)
{
    if ( isConnected() || hostName.isEmpty() )
        return false;
    _m_socket->connectToHost(hostName, port);
    return _m_socket->waitForConnected(msecs);
}

void BNetworkConnection::disconnectFromHost()
{
    if ( !isConnected() )
        return;
    _m_socket->disconnectFromHost();
}

bool BNetworkConnection::disconnectFromHostBlocking(int msecs)
{
    if ( !isConnected() )
        return true;
    _m_socket->disconnectFromHost();
    return _m_socket->waitForDisconnected(msecs);
}

void BNetworkConnection::close()
{
    if ( !isConnected() )
        return;
    _m_socket->close();
}

bool BNetworkConnection::isValid() const
{
    return !_m_socket.isNull() && _m_socket->isSocketSet();
}

bool BNetworkConnection::isConnected() const
{
    return isValid() && _m_socket->isOpen();
}

const QUuid &BNetworkConnection::uniqueId() const
{
    return _m_CUniqueId;
}

QAbstractSocket::SocketError BNetworkConnection::error() const
{
    return isValid() ? _m_socket->error() : QAbstractSocket::UnknownSocketError;
}

QString BNetworkConnection::errorString() const
{
    return isValid() ? _m_socket->errorString() : "";
}

qint64 BNetworkConnection::criticalBufferSize() const
{
    return _m_socketWrapper->criticalBufferSize();
}

bool BNetworkConnection::closeOnCriticalBufferSize() const
{
    return _m_socketWrapper->closeOnCriticalBufferSize();
}

bool BNetworkConnection::detailedLogMode() const
{
    return _m_detailedLog;
}

bool BNetworkConnection::autoDeleteSentReplies() const
{
    return _m_autoDelete;
}

QString BNetworkConnection::peerAddress() const
{
    return isValid() ? _m_socket->peerAddress() : "";
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &operation, const QByteArray &data)
{
    if ( !isConnected() || operation.isEmpty() )
        return 0;
    _m_Data dat;
    dat.first = data;
    dat.second.setId( QUuid::createUuid() );
    dat.second.setIsRequest(true);
    dat.second.setOperation(operation);
    BNetworkOperation *op = new BNetworkOperation(dat.second, this);
    connect( op, SIGNAL( destroyed(QObject *) ), this, SLOT( _m_operationDestroyed(QObject *) ) );
    _m_requests.insert(dat.second, op);
    _m_dataQueue.enqueue(dat);
    _m_sendNext();
    return op;
}

bool BNetworkConnection::sendReply(BNetworkOperation *operation, const QByteArray &data)
{
    if ( !isConnected() || !operation || !operation->isValid() || operation->isRequest() )
        return false;
    _m_Data dat;
    dat.first = data;
    dat.second = operation->metaData();
    _m_dataQueue.enqueue(dat);
    _m_sendNext();
    return true;
}

//

void BNetworkConnection::log(const QString &text)
{
    BTerminalIOHandler::writeLine("[" + QDateTime::currentDateTime().toString("dd/MMM/yyy hh:mm:ss") + "] [" +
                                  peerAddress() + "] " + text);
}

//

void BNetworkConnection::_m_init()
{
    _m_detailedLog = false;
    _m_autoDelete = true;
    _m_socketWrapper = new BSocketWrapper(this);
    connect(_m_socketWrapper, SIGNAL( downloadProgress(BSocketWrapper::MetaData, qint64, qint64) ),
            this, SLOT( _m_downloadProgress(BSocketWrapper::MetaData, qint64, qint64) ), Qt::DirectConnection);
    connect(_m_socketWrapper, SIGNAL( uploadProgress(BSocketWrapper::MetaData, qint64, qint64) ),
            this, SLOT( _m_uploadProgress(BSocketWrapper::MetaData, qint64, qint64) ), Qt::DirectConnection);
    connect(_m_socketWrapper, SIGNAL( dataReceived(QByteArray, BSocketWrapper::MetaData) ),
            this, SLOT( _m_dataReceived(QByteArray, BSocketWrapper::MetaData) ), Qt::DirectConnection);
    connect(_m_socketWrapper, SIGNAL( dataSent(BSocketWrapper::MetaData) ),
            this, SLOT( _m_dataSent(BSocketWrapper::MetaData) ), Qt::DirectConnection);
    connect(_m_socketWrapper, SIGNAL( criticalBufferSizeReached() ),
            this, SIGNAL( criticalBufferSizeReached() ), Qt::DirectConnection);
}

void BNetworkConnection::_m_setSocket(BGenericSocket *socket)
{
    if (!socket)
        return;
    _m_socket = socket;
    _m_socketWrapper->setSocket(socket);
    socket->setParent(this);
    connect(socket, SIGNAL( connected() ), this, SLOT( _m_connected() ), Qt::DirectConnection);
    connect(socket, SIGNAL( disconnected() ), this, SLOT( _m_disconnected() ), Qt::DirectConnection);
    connect(socket, SIGNAL( error(QAbstractSocket::SocketError) ),
            this, SLOT( _m_error(QAbstractSocket::SocketError) ), Qt::DirectConnection);
}

void BNetworkConnection::_m_sendNext()
{
    if ( !isConnected() || _m_socketWrapper->isBuisy() || _m_dataQueue.isEmpty() )
        return;
    _m_Data data = _m_dataQueue.dequeue();
    if ( !data.second.isValid() )
        return;
    BNetworkOperation *op = data.second.isRequest() ? _m_requests.value(data.second) : _m_replies.value(data.second);
    if (op)
        _m_socketWrapper->sendData(data.first, data.second) ? op->_m_setStarted() : op->_m_setError();
}

//

void BNetworkConnection::_m_connected()
{
    log( tr("Connected", "log text") );
    emit connected();
}

void BNetworkConnection::_m_disconnected()
{
    log( tr("Disconnected", "log text") );
    emit disconnected();
}

void BNetworkConnection::_m_error(QAbstractSocket::SocketError socketError)
{
    log( tr("Error:", "log text") + " " + errorString() );
    emit error(socketError);
}

void BNetworkConnection::_m_downloadProgress(const BNetworkOperationMetaData &metaData,
                                             qint64 bytesReady, qint64 bytesTotal)
{
    if ( metaData.isRequest() )
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = _m_replies.value(mdat);
        if (op)
        {
            op->_m_setDownloadProgress(bytesReady, bytesTotal);
        }
        else
        {
            op = new BNetworkOperation(mdat, this);
            connect( op, SIGNAL( destroyed(QObject *) ), this, SLOT( _m_operationDestroyed(QObject *) ) );
            _m_replies.insert(mdat, op);
            op->_m_setStarted();
            if (_m_detailedLog)
                log( tr("Incoming request:", "log text") + " " + metaData.operation() );
            emit incomingRequest(op);
        }
    }
    else
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(true);
        BNetworkOperation *op = _m_requests.value(mdat);
        if (op)
            op->_m_setDownloadProgress(bytesReady, bytesTotal);
    }
}

void BNetworkConnection::_m_uploadProgress(const BNetworkOperationMetaData &metaData,
                                           qint64 bytesReady, qint64 bytesTotal)
{
    BNetworkOperation *op = metaData.isRequest() ? _m_requests.value(metaData) : _m_replies.value(metaData);
    if (op)
        op->_m_setUploadProgress(bytesReady, bytesTotal);
}

void BNetworkConnection::_m_dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    if ( metaData.isRequest() )
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = _m_replies.value(mdat);
        if (!op)
            return;
        op->_m_bytesInReady = op->_m_bytesInTotal;
        op->_m_data = data;
        if (_m_detailedLog)
            log( tr("Request received:", "log text") + " " + metaData.operation() );
        emit requestReceived(op);
    }
    else
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = _m_requests.value(mdat);
        if (!op)
            return;
        _m_requests.remove(mdat);
        op->_m_bytesInReady = op->_m_bytesInTotal;
        op->_m_setFinished(data);
        if (_m_detailedLog)
            log( tr("Reply received:", "log text") + " " + metaData.operation() );
        emit replyReceived(op);
    }
}

void BNetworkConnection::_m_dataSent(const BNetworkOperationMetaData &metaData)
{
    if ( metaData.isRequest() )
    {
        BNetworkOperation *op = _m_requests.value(metaData);
        if (!op)
            return;
        if (_m_detailedLog)
            log( tr("Request sent:", "log text") + " " + metaData.operation() );
        emit requestSent(op);
    }
    else
    {
        BNetworkOperation *op = _m_replies.value(metaData);
        if (!op)
            return;
        _m_replies.remove(metaData);
        op->_m_setFinished();
        if (_m_detailedLog)
            log( tr("Reply sent:", "log text") + " " + metaData.operation() );
        if (_m_autoDelete)
            op->deleteLater();
        else
            emit replySent(op);
    }
    _m_sendNext();
}

void BNetworkConnection::_m_operationDestroyed(QObject *object)
{
    BNetworkOperation *operation = qobject_cast<BNetworkOperation *>(object);
    if (!operation)
        return;
    if ( operation->isRequest() )
        _m_requests.remove( operation->metaData() );
    else
        _m_replies.remove( operation->metaData() );
}
