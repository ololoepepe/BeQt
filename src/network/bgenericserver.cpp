#include "bgenericserver.h"
#include "bgenericsocket.h"
#include "btcpserver.h"
#include "blocalserver.h"
#include "bgenericserver_p.h"

#include <QObject>
#include <QTcpServer>
#include <QPointer>
#include <QLocalServer>
#include <QString>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QQueue>
#include <QMetaObject>

BGenericServerPrivateObject::BGenericServerPrivateObject(BGenericServerPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BGenericServerPrivateObject::~BGenericServerPrivateObject()
{
    //
}

//

void BGenericServerPrivateObject::newConnection(int socketDescriptor)
{
    _m_p->newConnection(socketDescriptor);
}

//

BGenericServerPrivate::BGenericServerPrivate(BGenericServer *q, BGenericServer::ServerType type) :
    BBasePrivate(q), _m_o( new BGenericServerPrivateObject(this) )
{
    maxPending = 0;
    switch (type)
    {
    case BGenericServer::LocalServer:
    {
        BLocalServer *server = new BLocalServer( q_func() );
        QObject::connect(server, SIGNAL( newConnection(int) ), _m_o, SLOT( newConnection(int) ), Qt::DirectConnection);
        lserver = server;
        break;
    }
    case BGenericServer::TcpServer:
    {
        BTcpServer *server = new BTcpServer( q_func() );
        QObject::connect(server, SIGNAL( newConnection(int) ), _m_o, SLOT( newConnection(int) ), Qt::DirectConnection);
        tserver = server;
    }
    default:
        break;
    }
}

BGenericServerPrivate::~BGenericServerPrivate()
{
    _m_o->deleteLater();
}

//

void BGenericServerPrivate::newConnection(int socketDescriptor)
{
    B_Q(BGenericServer);
    if (maxPending > 0)
    {
        BGenericSocket *socket = q->createSocket(socketDescriptor);
        if ( !socket || !socket->isValid() || !( socket->socketType() & q->serverType() ) )
            return;
        socket->setParent(q);
        socketQueue.enqueue(socket);
    }
    else
    {
        QMetaObject::invokeMethod( q, "newConnection", Q_ARG(int, socketDescriptor) );
    }
}

//

BGenericServer::BGenericServer(ServerType type, QObject *parent) :
    QObject(parent), BBase( *new BGenericServerPrivate(this, type) )
{
    //
}

//

QTcpServer *BGenericServer::tcpServer() const
{
    return d_func()->tserver.data();
}

QLocalServer *BGenericServer::localServer() const
{
    return d_func()->lserver.data();
}

void BGenericServer::close()
{
    B_D(BGenericServer);
    !d->tserver.isNull() ? d->tserver->close() : d->lserver->close();
}

QString BGenericServer::errorString() const
{
    if ( !isServerSet() )
        return "";
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->errorString() : d->lserver->errorString();
}

bool BGenericServer::hasPendingConnections() const
{
    return !d_func()->socketQueue.isEmpty();
}

bool BGenericServer::isListening() const
{
    if ( !isServerSet() )
        return false;
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->isListening() : d->lserver->isListening();
}

bool BGenericServer::isServerSet() const
{
    const B_D(BGenericServer);
    return !d->tserver.isNull() || !d->lserver.isNull();
}

bool BGenericServer::listen(const QString &address, quint16 port)
{
    if ( !isServerSet() )
        return false;
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->listen(QHostAddress(address), port) : d->lserver->listen(address);
}

int BGenericServer::maxPendingConnections() const
{
    return d_func()->maxPending;
}

BGenericSocket *BGenericServer::nextPendingConnection()
{
    return hasPendingConnections() ? d_func()->socketQueue.dequeue() : 0;
}

QString BGenericServer::serverAddress() const
{
    if ( !isServerSet() )
        return "";
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->serverAddress().toString() : d->lserver->serverName();
}

QAbstractSocket::SocketError BGenericServer::serverError() const
{
    if ( !isServerSet() )
        return QAbstractSocket::UnknownSocketError;
    const B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->serverError() : d->lserver->serverError();
}

BGenericServer::ServerType BGenericServer::serverType() const
{
    if ( !isServerSet() )
        return NoServer;
    return !d_func()->tserver.isNull() ? TcpServer : LocalServer;
}

void BGenericServer::setMaxPendingConnections(int numConnections)
{
    d_func()->maxPending = numConnections > 0 ? numConnections : 0;
}

bool BGenericServer::waitForNewConnection(int msec, bool *timedOut)
{
    if ( !isServerSet() )
        return false;
    B_D(BGenericServer);
    return !d->tserver.isNull() ? d->tserver->waitForNewConnection(msec, timedOut) :
                                  d->lserver->waitForNewConnection(msec, timedOut);
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

BGenericServer::BGenericServer(BGenericServerPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}
