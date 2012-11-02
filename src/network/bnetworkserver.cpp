#include "bnetworkserver.h"
#include "bnetworkserverthread.h"
#include "bnetworkserverworker.h"
#include "bnetworkconnection.h"

#include "bnetworkserver_p.h"

#include "bgenericserver.h"

#include <QObject>
#include <QList>
#include <QThread>
#include <QString>

BNetworkServerPrivate::BNetworkServerPrivate(BNetworkServer *q) :
    _m_q(q)
{
    //
}

BNetworkConnection *BNetworkServerPrivate::createConnection(int socketDescriptor)
{
    return _m_q->createConnection(socketDescriptor);
}

//

BNetworkServer::BNetworkServer(BGenericServer::ServerType type, QObject *parent) :
    QObject(parent), _m_d( new BNetworkServerPrivate(this) )
{
    _m_maxConnectionCount = 0;
    _m_maxThreadCount = 0;
    if (BGenericServer::NoServer != type)
    {
        _m_server = new BGenericServer(type, this);
        _m_server->setMaxPendingConnections(0);
        connect( _m_server.data(), SIGNAL( newConnection(int) ), this, SLOT( _m_newConnection(int) ) );
    }
}

BNetworkServer::~BNetworkServer()
{
    delete _m_d;
    for (int i = 0; i < _m_threads.size(); ++i)
    {
        QThread *t = _m_threads.at(i);
        if (!t)
            continue;
        disconnect( t, SIGNAL( finished() ), this, SLOT( _m_finished() ) );
        t->quit();
        t->wait(1000);
    }
}

//

void BNetworkServer::setMaxConnectionCount(int count)
{
    _m_maxConnectionCount = count > 0 ? count : 0;
}

void BNetworkServer::setMaxThreadCount(int count)
{
    _m_maxThreadCount = count > 0 ? count : 0;
}

bool BNetworkServer::isValid() const
{
    return _m_server.isNull() && _m_server->isServerSet();
}

bool BNetworkServer::isListening() const
{
    return !_m_server.isNull() && _m_server->isListening();
}

bool BNetworkServer::listen(const QString &address, quint16 port)
{
    return !_m_server.isNull() && _m_server->listen(address, port);
}

void BNetworkServer::close()
{
    if ( _m_server.isNull() )
        return;
    _m_server->close();
}

BGenericServer::ServerType BNetworkServer::serverType() const
{
    return !_m_server.isNull() ? _m_server->serverType() : BGenericServer::NoServer;
}

int BNetworkServer::maxConnectionCount() const
{
    return _m_maxConnectionCount;
}

int BNetworkServer::currentConnectionCount() const
{
    int count = 0;
    for (int i = 0; i < _m_threads.size(); ++i)
        count += _m_threads.at(i)->connectionCount();
    return count;
}

int BNetworkServer::maxThreadCount() const
{
    return _m_maxThreadCount;
}

int BNetworkServer::currentThreadCount() const
{
    return _m_threads.size();
}

//

BNetworkConnection *BNetworkServer::createConnection(int socketDescriptor)
{
    BGenericSocket *socket = new BGenericSocket(BGenericSocket::TcpSocket);
    if ( !socket->setSocketDescriptor(socketDescriptor) || !socket->isValid() )
    {
        socket->deleteLater();
        return 0;
    }
    return new BNetworkConnection(socket);
}

//

void BNetworkServer::_m_newConnection(int socketDescriptor)
{
    if (_m_maxConnectionCount > 0 && currentConnectionCount() > _m_maxConnectionCount)
        return;
    if (_m_maxThreadCount > 0 && currentThreadCount() == _m_maxThreadCount)
    {
        int cc = _m_threads.first()->connectionCount();
        int ind = 0;
        for (int i = 0; i < _m_threads.size(); ++i)
        {
            int ccc = _m_threads.at(i)->connectionCount();
            if (ccc < cc)
            {
                cc = ccc;
                ind = i;
            }
        }
        _m_threads[ind]->addConnection(socketDescriptor);
    }
    else
    {
        BNetworkServerThread *t = new BNetworkServerThread(new BNetworkServerWorker( d_func() ), this);
        if ( !t->isValid() )
        {
            t->deleteLater();
            return;
        }
        connect(t, SIGNAL( finished() ), this, SLOT( _m_finished() ), Qt::DirectConnection);
        t->start();
        t->addConnection(socketDescriptor);
        _m_threads << t;
    }
}

void BNetworkServer::_m_finished()
{
    BNetworkServerThread *t = qobject_cast<BNetworkServerThread *>( sender() );
    if (!t)
        return;
    _m_threads.removeAll(t);
    t->deleteLater();
}
