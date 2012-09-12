#include "bnetworkconnection.h"
#include "bsocketshell.h"
#include "bgenericsocket.h"
#include "bnetworkoperation.h"

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
    _m_socketShell->setCriticalBufferSize(size);
}

void BNetworkConnection::setCloseOnCriticalBufferSize(bool close)
{
    _m_socketShell->setCloseOnCriticalBufferSize(close);
}

void BNetworkConnection::setDetailedLogMode(bool enabled)
{
    _m_detailedLog = enabled;
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
    return _m_socketShell->criticalBufferSize();
}

bool BNetworkConnection::closeOnCriticalBufferSize() const
{
    return _m_socketShell->closeOnCriticalBufferSize();
}

bool BNetworkConnection::detailedLogMode() const
{
    return _m_detailedLog;
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
    _m_requests.insert(dat.second, op);
    _m_dataQueue.enqueue(dat);
    _m_sendNext();
    return op;
}

//

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

void BNetworkConnection::log(const QString &text)
{
    "[" + QDateTime::currentDateTime().toString("dd/MMM/yyy hh:mm:ss") + "] " + text + "\n";
}

//

void BNetworkConnection::_m_init()
{
    _m_detailedLog = false;
    _m_socketShell = new BSocketShell(this);
    connect(_m_socketShell, SIGNAL( downloadProgress(BSocketShell::MetaData, qint64, qint64) ),
            this, SLOT( _m_downloadProgress(BSocketShell::MetaData, qint64, qint64) ), Qt::DirectConnection);
    connect(_m_socketShell, SIGNAL( uploadProgress(BSocketShell::MetaData, qint64, qint64) ),
            this, SLOT( _m_uploadProgress(BSocketShell::MetaData, qint64, qint64) ), Qt::DirectConnection);
    connect(_m_socketShell, SIGNAL( dataReceived(QByteArray, BSocketShell::MetaData) ),
            this, SLOT( _m_dataReceived(QByteArray, BSocketShell::MetaData) ), Qt::DirectConnection);
    connect(_m_socketShell, SIGNAL( dataSent(BSocketShell::MetaData) ),
            this, SLOT( _m_dataSent(BSocketShell::MetaData) ), Qt::DirectConnection);
    connect(_m_socketShell, SIGNAL( criticalBufferSizeReached() ),
            this, SIGNAL( criticalBufferSizeReached() ), Qt::DirectConnection);
}

void BNetworkConnection::_m_setSocket(BGenericSocket *socket)
{
    if (!socket)
        return;
    _m_socket = socket;
    _m_socketShell->setSocket(socket);
    socket->setParent(this);
    connect( socket, SIGNAL( disconnected() ), this, SLOT( _m_disconnected() ), Qt::DirectConnection);
    connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ),
             this, SLOT( _m_error(QAbstractSocket::SocketError) ), Qt::DirectConnection);
}

void BNetworkConnection::_m_sendNext()
{
    if ( !isConnected() || _m_socketShell->isBuisy() || _m_dataQueue.isEmpty() )
        return;
    _m_Data data = _m_dataQueue.dequeue();
    if ( !data.second.isValid() )
        return;
    bool b = _m_socketShell->sendData(data.first, data.second);
    BNetworkOperation *op = data.second.isRequest() ? _m_requests.value(data.second) : _m_replies.value(data.second);
    if (op)
        b ? op->_m_setStarted() : op->_m_setError();
}

QString BNetworkConnection::_m_operation(bool request) const
{
    return request ? tr("Request", "log text") : tr("Reply", "log text");
}

//

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

void BNetworkConnection::_m_downloadProgress(const BSocketShell::MetaData &metaData,
                                             qint64 bytesReady, qint64 bytesTotal)
{
    if ( metaData.isRequest() )
    {
        BSocketShell::MetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = _m_replies.value(mdat);
        if (op)
        {
            op->_m_setDownloadProgress(bytesReady, bytesTotal);
        }
        else
        {
            op = new BNetworkOperation(mdat, this);
            _m_replies.insert(mdat, op);
            op->_m_setStarted();
            emit incomingRequest(op);
        }
    }
    else
    {
        BSocketShell::MetaData mdat = metaData;
        mdat.setIsRequest(true);
        BNetworkOperation *op = _m_requests.value(mdat);
        if (op)
            op->_m_setDownloadProgress(bytesReady, bytesTotal);
    }
}

void BNetworkConnection::_m_uploadProgress(const BSocketShell::MetaData &metaData,
                                           qint64 bytesReady, qint64 bytesTotal)
{
    BNetworkOperation *op = metaData.isRequest() ? _m_requests.value(metaData) : _m_replies.value(metaData);
    if (op)
        op->_m_setUploadProgress(bytesReady, bytesTotal);
}

void BNetworkConnection::_m_dataReceived(const QByteArray &data, const BSocketShell::MetaData &metaData)
{
    if ( metaData.isRequest() )
    {
        BSocketShell::MetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = _m_replies.value(mdat);
        if (!op)
            return;
        op->_m_bytesInReady = op->_m_bytesInTotal;
        op->_m_data = data;
        emit requestReceived(op);
    }
    else
    {
        BSocketShell::MetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = _m_requests.value(mdat);
        if (!op)
            return;
        _m_requests.remove(mdat);
        op->_m_bytesInReady = op->_m_bytesInTotal;
        op->_m_setFinished(data);
        emit replyReceived(op);
    }
}

void BNetworkConnection::_m_dataSent(const BSocketShell::MetaData &metaData)
{
    if ( metaData.isRequest() )
    {
        BNetworkOperation *op = _m_requests.value(metaData);
        if (!op)
            return;
        emit requestSent(op);
    }
    else
    {
        BNetworkOperation *op = _m_replies.value(metaData);
        if (!op)
            return;
        _m_replies.remove(metaData);
        op->_m_setFinished();
        emit replySent(op);
    }
    _m_sendNext();
}
