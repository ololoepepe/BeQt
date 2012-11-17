#include "bnetworkserver.h"
#include "bnetworkconnection.h"
#include "bnetworkserver_p.h"
#include "bgenericserver.h"

#include <QObject>
#include <QList>
#include <QThread>
#include <QString>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>

/*========== Network Server Worker ==========*/

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerPrivate *sp) :
    QObject(0), serverPrivate(sp)
{
    //
}

BNetworkServerWorker::~BNetworkServerWorker()
{
    //
}

//

int BNetworkServerWorker::connectionCount() const
{
    QMutexLocker locker(&connectionsMutex);
    return connections.size();
}

//

void BNetworkServerWorker::addConnection(int socketDescriptor)
{
    BNetworkConnection *connection = serverPrivate->createConnection(socketDescriptor);
    if ( !connection || !connection->isValid() )
        return;
    connect( connection, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
    QMutexLocker locker(&connectionsMutex);
    connections << connection;
}

//

void BNetworkServerWorker::disconnected()
{
    BNetworkConnection *c = static_cast<BNetworkConnection *>( sender() );
    if (!c)
        return;
    QMutexLocker locker(&connectionsMutex);
    connections.removeAll(c);
    c->deleteLater();
    if ( connections.isEmpty() )
        emit ranOutOfConnections();
}

/*========== Network Server Thread ==========*/

BNetworkServerThread::BNetworkServerThread(BNetworkServerPrivate *serverPrivate) :
    QThread(0), worker( new BNetworkServerWorker(serverPrivate) )
{
    worker->moveToThread(this);
    QObject::connect(worker, SIGNAL( ranOutOfConnections() ), this, SLOT( quit() ), Qt::QueuedConnection);
}

BNetworkServerThread::~BNetworkServerThread()
{
    worker->deleteLater();
}

//

void BNetworkServerThread::addConnection(int socketDescriptor)
{
    QMetaObject::invokeMethod( worker, "addConnection", Qt::QueuedConnection, Q_ARG(int, socketDescriptor) );
}

int BNetworkServerThread::connectionCount() const
{
    return worker->connectionCount();
}

/*========== Network Server Private Object ==========*/

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

/*========== Network Server Private ==========*/

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

/*========== Network Server ==========*/

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

//

BNetworkServer::BNetworkServer(BNetworkServerPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}
