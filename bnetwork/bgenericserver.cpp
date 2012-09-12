#include "bgenericserver.h"
#include "bgenericsocket.h"
#include "private/btcpserver.h"
#include "private/blocalserver.h"

#include <QObject>
#include <QTcpServer>
#include <QPointer>
#include <QLocalServer>
#include <QString>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QQueue>

//#include <QDebug>

BGenericServer::BGenericServer(ServerType type, QObject *parent) :
    QObject(parent)
{
    _m_maxPending = 0;
    switch (type)
    {
    case LocalServer:
    {
        BLocalServer *server = new BLocalServer(this);
        connect(server, SIGNAL( newConnection(int) ), this, SLOT( _m_newConnection(int) ), Qt::DirectConnection);
        _m_lserver = server;
        break;
    }
    case TcpServer:
    {
        BTcpServer *server = new BTcpServer(this);
        connect(server, SIGNAL( newConnection(int) ), this, SLOT( _m_newConnection(int) ), Qt::DirectConnection);
        _m_tserver = server;
    }
    default:
        break;
    }
}

//

QTcpServer *BGenericServer::tcpServer() const
{
    return _m_tserver.data();
}

QLocalServer *BGenericServer::localServer() const
{
    return _m_lserver.data();
}

void BGenericServer::close()
{
    !_m_tserver.isNull() ? _m_tserver->close() : _m_lserver->close();
}

QString BGenericServer::errorString() const
{
    if ( !isServerSet() )
        return "";
    return !_m_tserver.isNull() ? _m_tserver->errorString() : _m_lserver->errorString();
}

bool BGenericServer::hasPendingConnections() const
{
    return !_m_socketQueue.isEmpty();
}

bool BGenericServer::isListening() const
{
    if ( !isServerSet() )
        return false;
    return !_m_tserver.isNull() ? _m_tserver->isListening() : _m_lserver->isListening();
}

bool BGenericServer::isServerSet() const
{
    return !_m_tserver.isNull() || !_m_lserver.isNull();
}

bool BGenericServer::listen(const QString &address, quint16 port)
{
    if ( !isServerSet() )
        return false;
    return !_m_tserver.isNull() ? _m_tserver->listen(QHostAddress(address), port) : _m_lserver->listen(address);
}

int BGenericServer::maxPendingConnections() const
{
    return _m_maxPending;
}

BGenericSocket *BGenericServer::nextPendingConnection()
{
    return hasPendingConnections() ? _m_socketQueue.dequeue() : 0;
}

QString BGenericServer::serverAddress() const
{
    if ( !isServerSet() )
        return "";
    return !_m_tserver.isNull() ? _m_tserver->serverAddress().toString() : _m_lserver->serverName();
}

QAbstractSocket::SocketError BGenericServer::serverError() const
{
    if ( !isServerSet() )
        return QAbstractSocket::UnknownSocketError;
    return !_m_tserver.isNull() ? _m_tserver->serverError() : _m_lserver->serverError();
}

BGenericServer::ServerType BGenericServer::serverType() const
{
    if ( !isServerSet() )
        return NoServer;
    return !_m_tserver.isNull() ? TcpServer : LocalServer;
}

void BGenericServer::setMaxPendingConnections(int numConnections)
{
    _m_maxPending = numConnections > 0 ? numConnections : 0;
}

bool BGenericServer::waitForNewConnection(int msec, bool *timedOut)
{
    if ( !isServerSet() )
        return false;
    return !_m_tserver.isNull() ? _m_tserver->waitForNewConnection(msec, timedOut) :
                                  _m_lserver->waitForNewConnection(msec, timedOut);
}

//

BGenericSocket *BGenericServer::createSocket(int socketDescriptor)
{
    BGenericSocket *socket = 0;
    switch ( serverType() )
    {
    case TcpServer:
        socket = new BGenericSocket(BGenericSocket::TcpSocket);
        socket->setSocketDescriptor(socketDescriptor);
        break;
    case LocalServer:
        socket = new BGenericSocket(BGenericSocket::LocalSocket);
        socket->setSocketDescriptor(socketDescriptor);
    default:
        break;
    }
    return socket;
}

//

void BGenericServer::_m_newConnection(int socketDescriptor)
{
    if (_m_maxPending > 0)
    {
        BGenericSocket *socket = createSocket(socketDescriptor);
        if ( !socket || !socket->isValid() || !( socket->socketType() & serverType() ) )
            return;
        socket->setParent(this);
        _m_socketQueue.enqueue(socket);
    }
    else
    {
        emit newConnection(socketDescriptor);
    }
}
