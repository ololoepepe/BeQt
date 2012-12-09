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

/*============================================================================
================================ BNetworkServerWorker ========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerWorker::BNetworkServerWorker(BNetworkServerPrivate *sp) :
    QObject(0), ServerPrivate(sp)
{
    //
}

BNetworkServerWorker::~BNetworkServerWorker()
{
    //
}

/*============================== Public slots ==============================*/

void BNetworkServerWorker::addConnection(int socketDescriptor)
{
    BGenericSocket *s = ServerPrivate->createSocket();
    if ( !s || !s->setSocketDescriptor(socketDescriptor) || !s->isValid() )
        return;
    BNetworkConnection *c = ServerPrivate->createConnection(s);
    if ( !c || !c->isValid() )
        return;
    connect( c, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
    emit connectionAdded(c);
}

void BNetworkServerWorker::disconnected()
{
    emit disconnected( sender() );
}

/*============================================================================
================================ BNetworkServerThread ========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerThread::BNetworkServerThread(BNetworkServerPrivate *serverPrivate) :
    QThread(0), Worker( new BNetworkServerWorker(serverPrivate) )
{
    Worker->moveToThread(this);
    connect(Worker, SIGNAL( connectionAdded(QObject *) ),
            this, SLOT( connectionAdded(QObject *) ), Qt::QueuedConnection);
    connect(Worker, SIGNAL( disconnected(QObject *) ), this, SLOT( disconnected(QObject *) ), Qt::QueuedConnection);
}

BNetworkServerThread::~BNetworkServerThread()
{
    Worker->deleteLater();
}

/*============================== Public methods ============================*/

void BNetworkServerThread::addConnection(int socketDescriptor)
{
    QMetaObject::invokeMethod( Worker, "addConnection", Qt::QueuedConnection, Q_ARG(int, socketDescriptor) );
}

int BNetworkServerThread::connectionCount() const
{
    return connections.size();
}

/*============================== Public slots ==============================*/

void BNetworkServerThread::connectionAdded(QObject *obj)
{
    BNetworkConnection *c = static_cast<BNetworkConnection *>(obj);
    if (!c)
        return;
    connections << c;
    Worker->ServerPrivate->emitConnectionAdded(c);
}

void BNetworkServerThread::disconnected(QObject *obj)
{
    BNetworkConnection *c = static_cast<BNetworkConnection *>(obj);
    if (!c)
        return;
    Worker->ServerPrivate->emitConnectionAboutToBeRemoved(c);
    connections.removeAll(c);
    c->deleteLater();
    if ( connections.isEmpty() )
        quit();
}

/*============================================================================
================================ BNetworkServerPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServerPrivate::BNetworkServerPrivate(BNetworkServer *q) :
    BBasePrivate(q)
{
    //
}

BNetworkServerPrivate::~BNetworkServerPrivate()
{
    foreach (BNetworkServerThread *t, threads)
    {
        disconnect( t, SIGNAL( finished() ), this, SLOT( finished() ) );
        t->quit();
        t->wait(1000);
    }
}

/*============================== Public methods ============================*/

void BNetworkServerPrivate::init()
{
    maxConnectionCount = 0;
    maxThreadCount = 0;
}

BNetworkConnection *BNetworkServerPrivate::createConnection(BGenericSocket *socket) const
{
    return q_func()->createConnection(socket);
}

BGenericSocket *BNetworkServerPrivate::createSocket() const
{
    return q_func()->createSocket();
}

BNetworkServerThread *BNetworkServerPrivate::getOptimalThread()
{
    int ccount = connectionCount();
    if (maxConnectionCount > 0 && ccount > maxConnectionCount)
        return 0;
    if (maxThreadCount > 0 && ccount == maxThreadCount)
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
        return threads[ind];
    }
    else
    {
        BNetworkServerThread *t = new BNetworkServerThread(this);
        QObject::connect( t, SIGNAL( finished() ), this, SLOT( finished() ) );
        t->start();
        threads << t;
        return t;
    }
}

int BNetworkServerPrivate::connectionCount() const
{
    int count = 0;
    foreach (BNetworkServerThread *t, threads)
        count += t->connectionCount();
    return count;
}

void BNetworkServerPrivate::emitConnectionAdded(BNetworkConnection *connection)
{
    QMetaObject::invokeMethod( q_func(), "connectionAdded", Q_ARG(BNetworkConnection *, connection) );
}

void BNetworkServerPrivate::emitConnectionAboutToBeRemoved(BNetworkConnection *connection)
{
    QMetaObject::invokeMethod( q_func(), "connectionAboutToBeRemoved", Q_ARG(BNetworkConnection *, connection) );
}

/*============================== Public slots ==============================*/

void BNetworkServerPrivate::newConnection(int socketDescriptor)
{
    BNetworkServerThread *t = getOptimalThread();
    if (t)
    {
        t->addConnection(socketDescriptor);
    }
    else
    {
        //If connection limit is reached, every new connection is automatically removed.
        //No notifications are sent to clients, they are just disconnected.
        //If you want to send a notification to a client before disconnecting, set unlimited connection count,
        //and then check current connection count from within the newly connected client.
        //Then you may send some notifications and disconnect the client.
        BGenericSocket *s = new BGenericSocket(BGenericSocket::TcpSocket);
        s->setSocketDescriptor(socketDescriptor);
        s->disconnectFromHost();
        s->deleteLater();
    }
}

void BNetworkServerPrivate::finished()
{
    BNetworkServerThread *t = static_cast<BNetworkServerThread *>( sender() );
    if (!t)
        return;
    threads.removeAll(t);
    t->deleteLater();
}

/*============================================================================
================================ BNetworkServer ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkServer::BNetworkServer(BGenericServer::ServerType type, QObject *parent) :
    QObject(parent), BBase( *new BNetworkServerPrivate(this) )
{
    d_func()->init();
    B_D(BNetworkServer);
    if (BGenericServer::NoServer != type)
    {
        d->server = new BGenericServer(type, this);
        d->server->setMaxPendingConnections(0);
        connect( d->server.data(), SIGNAL( newConnection(int) ), d, SLOT( newConnection(int) ) );
    }
}

BNetworkServer::~BNetworkServer()
{
    //
}

/*============================== Protected constructors ====================*/

BNetworkServer::BNetworkServer(BNetworkServerPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

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
    QMutexLocker locker(&d_func()->connectionMutex);
    int count = d_func()->connectionCount();
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

QList<BNetworkConnection *> BNetworkServer::connections() const
{
    QMutexLocker locker(&d_func()->connectionMutex);
    QList<BNetworkConnection *> list;
    foreach (BNetworkServerThread *t, d_func()->threads)
        list << t->connections;
    return list;
}

/*============================== Protected methods =========================*/

BGenericSocket *BNetworkServer::createSocket() const
{
    return new BGenericSocket(BGenericSocket::TcpSocket);
}
