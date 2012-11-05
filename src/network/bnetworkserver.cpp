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

BNetworkServerPrivateObject::BNetworkServerPrivateObject(BNetworkServerPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BNetworkServerPrivateObject::~BNetworkServerPrivateObject()
{
    //
}

void BNetworkServerPrivateObject::newConnection(int socketDescriptor)
{
    _m_p->newConnection(socketDescriptor);
}

void BNetworkServerPrivateObject::finished()
{
    _m_p->finished( static_cast<BNetworkServerThread *>( sender() ) );
}

//

BNetworkServerPrivate::BNetworkServerPrivate(BNetworkServer *q, BGenericServer::ServerType type) :
    BBasePrivate(q), _m_o( new BNetworkServerPrivateObject(this) )
{
    maxConnectionCount = 0;
    maxThreadCount = 0;
    if (BGenericServer::NoServer != type)
    {
        server = new BGenericServer( type, q_func() );
        server->setMaxPendingConnections(0);
        QObject::connect( server.data(), SIGNAL( newConnection(int) ), _m_o, SLOT( newConnection(int) ) );
    }
}

BNetworkServerPrivate::~BNetworkServerPrivate()
{
    _m_o->deleteLater();
    foreach (BNetworkServerThread *t, threads)
    {
        if (!t)
            continue;
        QObject::disconnect( t, SIGNAL( finished() ), _m_o, SLOT( finished() ) );
        t->quit();
        t->wait(1000);
    }
}

//

void BNetworkServerPrivate::newConnection(int socketDescriptor)
{
    B_Q(BNetworkServer);
    if (maxConnectionCount > 0 && q->currentConnectionCount() > maxConnectionCount)
        return;
    if (maxThreadCount > 0 && q->currentThreadCount() == maxThreadCount)
    {
        int cc = threads.first()->connectionCount();
        int ind = 0;
        for (int i = 0; i < threads.size(); ++i)
        {
            int ccc = threads.at(i)->connectionCount();
            if (ccc < cc)
            {
                cc = ccc;
                ind = i;
            }
        }
        threads[ind]->addConnection(socketDescriptor);
    }
    else
    {
        BNetworkServerThread *t = new BNetworkServerThread(this);
        QObject::connect( t, SIGNAL( finished() ), _m_o, SLOT( finished() ) );
        t->start();
        t->addConnection(socketDescriptor);
        threads << t;
    }
}

void BNetworkServerPrivate::finished(BNetworkServerThread *t)
{
    if (!t)
        return;
    threads.removeAll(t);
    t->deleteLater();
}

BNetworkConnection *BNetworkServerPrivate::createConnection(int socketDescriptor) const
{
    return q_func()->createConnection(socketDescriptor);
}

//

BNetworkServer::BNetworkServer(BGenericServer::ServerType type, QObject *parent) :
    QObject(parent), BBase( *new BNetworkServerPrivate(this, type) )
{
    //
}

BNetworkServer::~BNetworkServer()
{
    //
}

//

void BNetworkServer::setMaxConnectionCount(int count)
{
    d_func()->maxConnectionCount = count > 0 ? count : 0;
}

void BNetworkServer::setMaxThreadCount(int count)
{
    d_func()->maxThreadCount = count > 0 ? count : 0;
}

bool BNetworkServer::isValid() const
{
    const B_D(BNetworkServer);
    return d->server.isNull() && d->server->isServerSet();
}

bool BNetworkServer::isListening() const
{
    const B_D(BNetworkServer);
    return !d->server.isNull() && d->server->isListening();
}

bool BNetworkServer::listen(const QString &address, quint16 port)
{
    B_D(BNetworkServer);
    return !d->server.isNull() && d->server->listen(address, port);
}

void BNetworkServer::close()
{
    B_D(BNetworkServer);
    if ( d->server.isNull() )
        return;
    d->server->close();
}

BGenericServer::ServerType BNetworkServer::serverType() const
{
    const B_D(BNetworkServer);
    return !d->server.isNull() ? d->server->serverType() : BGenericServer::NoServer;
}

int BNetworkServer::maxConnectionCount() const
{
    return d_func()->maxConnectionCount;
}

int BNetworkServer::currentConnectionCount() const
{
    int count = 0;
    foreach (BNetworkServerThread *t, d_func()->threads)
        count += t->connectionCount();
    return count;
}

int BNetworkServer::maxThreadCount() const
{
    return d_func()->maxThreadCount;
}

int BNetworkServer::currentThreadCount() const
{
    return d_func()->threads.size();
}
