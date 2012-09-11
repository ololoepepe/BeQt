#include "bnetworkconnection.h"
#include "bsocketshell.h"

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QMap>
#include <QIODevice>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QTimer>

const QDataStream::Version BNetworkConnection::DefaultDataStreamVersion = QDataStream::Qt_4_7;
const QString BNetworkConnection::AuthorizeOperation = "authorize";

//

bool BNetworkConnection::checkSocket(QAbstractSocket *socket)
{
    return socket && socket->isOpen() && (socket->openMode() & QAbstractSocket::ReadWrite);
}

bool BNetworkConnection::checkInDataStream(const QDataStream &in)
{
    QIODevice *devIn = in.device();
    return devIn && (devIn->openMode() & QIODevice::ReadOnly);
}

bool BNetworkConnection::checkInOutDataStreams(const QDataStream &in, const QDataStream &out)
{
    QIODevice *devOut = out.device();
    return devOut && (devOut->openMode() & QIODevice::WriteOnly) && checkInDataStream(in);
}

//

BNetworkConnection::BNetworkConnection(QAbstractSocket *socket, int authorizationTimeout, QObject *parent) :
    QObject(parent)
{
    _m_logger = 0;
    _m_authorized = false;
    _m_dataStreamVersion = DefaultDataStreamVersion;
    _m_socket = socket;
    if (!_m_socket)
        return;
    _m_socket->setParent(this);
    _m_socketShell = new BSocketShell(this);
    _m_socketShell->setSocket(_m_socket);
    connect( _m_socket, SIGNAL( disconnected() ), this, SLOT( _m_disconnected() ) );
    connect( _m_socket, SIGNAL( error(QAbstractSocket::SocketError) ),
             this, SLOT( _m_error(QAbstractSocket::SocketError) ) );
    connect( _m_socketShell, SIGNAL( dataReceived(QByteArray) ), this, SLOT( _m_dataReceived(QByteArray) ) );
    connect( _m_socketShell, SIGNAL( dataSent() ), this, SLOT( _m_dataSent() ) );
    connect( _m_socketShell, SIGNAL( criticalBufferSizeReached() ), this, SIGNAL( criticalBufferSizeReached() ) );
    addRequestHandler(AuthorizeOperation, &BNetworkConnection::_m_handleAuthorizationRequest);
    addReplyHandler(AuthorizeOperation, &BNetworkConnection::_m_handleAuthorizationReply);
    if (authorizationTimeout > 0)
        QTimer::singleShot( authorizationTimeout, this, SLOT( _m_checkAuthorization() ) );
    if ( _m_socket->isOpen() )
        _m_address = _m_socket->peerAddress().toString();
}

//

void BNetworkConnection::setDataStreamVersion(QDataStream::Version version)
{
    _m_dataStreamVersion = version;
}

void BNetworkConnection::setCriticalBufferSize(quint64 size)
{
    _m_socketShell->setCriticalBufferSize(size);
}

void BNetworkConnection::setCloseOnCriticalBufferSizeReached(bool close)
{
    _m_socketShell->setCloseOnCriticalBufferSizeReached(close);
}

bool BNetworkConnection::connectToHost(const QString &hostName, quint16 port,
                                       const QString &login, const QString &password, int timeout)
{
    _m_address = hostName;
    if ( !_m_socket || _m_socket->isOpen() || hostName.isEmpty() )
        return false;
    _m_socket->connectToHost(hostName, port);
    log( tr("Connecting...", "log text") );
    if ( !_m_socket->waitForConnected(timeout) )
    {
        log( tr("Connection failed", "log text") );
        return false;
    }
    log( tr("Connected", "log text") );
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion( dataStreamVersion() );
    out << (int) Request;
    out << BNetworkConnection::AuthorizeOperation;
    out << login;
    out << password;
    _m_socketShell->sendData(ba);
    return true;
}

bool BNetworkConnection::disconnectFromHost(int timeout)
{
    if ( !isConnected() )
        return true;
    if (timeout > 0)
    {
        _m_socket->disconnectFromHost();
        return _m_socket->waitForDisconnected(timeout);
    }
    else
    {
        _m_socket->close();
        return true;
    }
}

bool BNetworkConnection::isValid() const
{
    return _m_socket;
}

bool BNetworkConnection::isConnected() const
{
    return checkSocket(_m_socket);
}

bool BNetworkConnection::isAuthorized() const
{
    return _m_authorized;
}

QAbstractSocket::SocketError BNetworkConnection::error() const
{
    return _m_socket ? _m_socket->error() : QAbstractSocket::UnknownSocketError;
}

QString BNetworkConnection::errorString() const
{
    return _m_socket ? _m_socket->errorString() : "";
}

QDataStream::Version BNetworkConnection::dataStreamVersion() const
{
    return _m_dataStreamVersion;
}

quint64 BNetworkConnection::criticalBufferSize() const
{
    return _m_socketShell ? _m_socketShell->criticalBufferSize() : 0;
}

bool BNetworkConnection::closeOnCriticalBufferSizeReached() const
{
    return _m_socketShell ? _m_socketShell->closeOnCriticalBufferSizeReached() : false;
}

QString BNetworkConnection::address() const
{
    return _m_address;
}

//

void BNetworkConnection::addRequestHandler(const QString &id, RequestHandler handler)
{
    if (id.isEmpty() || !handler)
        return;
    _m_requestHandlers.insert(id, handler);
}

void BNetworkConnection::addReplyHandler(const QString &id, ReplyHandler handler)
{
    if (id.isEmpty() || !handler)
        return;
    _m_replyHandlers.insert(id, handler);
}

void BNetworkConnection::setLogger(Logger logger)
{
    _m_logger = logger;
}

void BNetworkConnection::log(const QString &text)
{
    if (_m_logger)
        (this->*_m_logger)(text);
}

bool BNetworkConnection::sendData(OperationType type, const QString &operation, const QByteArray &data)
{
    if ( !isAuthorized() || Invalid == type || operation.isEmpty() || data.isEmpty() )
        return false;
    _m_lastOperationType = type;
    _m_lastOperation = operation;
    _m_socketShell->sendData(data);
    return true;
}

bool BNetworkConnection::authorize(const QString &login, const QString &password)
{
    return true;
}

//

void BNetworkConnection::_m_handleAuthorizationRequest(QDataStream &in, QDataStream &out)
{
    QString login;
    QString password;
    in >> login;
    in >> password;
    _m_authorized = authorize(login, password);
    out << _m_authorized;
    if (_m_authorized)
    {
        if ( !login.isEmpty() )
            log(tr("Authorized as:", "log text") + " " + login);
        else
            log( tr("Authorized", "log text") );
        emit authorized();
    }
}

void BNetworkConnection::_m_handleAuthorizationReply(bool success, QDataStream &in)
{
    if (success)
    {
        _m_authorized = true;
        log( tr("Authorized", "log text") );
        emit authorized();
    }
    else
    {
        emit authorizationFailed();
    }
}

//

void BNetworkConnection::_m_checkAuthorization()
{
    if (_m_authorized)
        return;
    _m_socket->close();
    log( tr("Authorization failed", "log text")  );
    emit authorizationFailed();
}

void BNetworkConnection::_m_disconnected()
{
    log( tr("Disconnected", "log text") );
    emit disconnected();
}

void BNetworkConnection::_m_error(QAbstractSocket::SocketError error)
{
    QString err = errorString();
    log(tr("Error:", "log text") + " " + err);
    emit errorOccured(err);
}

void BNetworkConnection::_m_dataReceived(const QByteArray &data)
{
    _m_lastOperationType = Invalid;
    _m_lastOperation.clear();
    QDataStream in(data);
    in.setVersion(_m_dataStreamVersion);
    in >> _m_lastOperationType;
    in >> _m_lastOperation;
    switch (_m_lastOperationType)
    {
    case Request:
    {
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out.setVersion(_m_dataStreamVersion);
        out << (int) Reply;
        out << _m_lastOperation;
        RequestHandler h = _m_requestHandlers.value(_m_lastOperation);
        if (h)
        {
            (this->*h)(in, out);
        }
        else
        {
            log( tr("Unidentified request", "log text") );
            out << false;
            out << tr("Unidentified operation", "reply text");
        }
        _m_lastOperationType = Reply;
        _m_socketShell->sendData(ba);
        break;
    }
    case Reply:
    {
        bool success = false;
        in >> success;
        ReplyHandler h = _m_replyHandlers.value(_m_lastOperation);
        if (h)
            (this->*h)(success, in);
        else
            log( tr("Unidentified reply", "log text") );
        break;
    }
    default:
        break;
    }
}

void BNetworkConnection::_m_dataSent()
{
    switch (_m_lastOperationType)
    {
    case Request:
        log(tr("Request sent:", "log text") + " " + _m_lastOperation);
        emit requestSent(_m_lastOperation);
        break;
    case Reply:
        log(tr("Reply sent:", "log text") + " " + _m_lastOperation);
        emit replySent(_m_lastOperation);
        break;
    default:
        break;
    }
}
