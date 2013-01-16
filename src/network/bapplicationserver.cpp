#include "bapplicationserver.h"
#include "bapplicationserver_p.h"
#include "bgenericserver.h"
#include "bgenericsocket.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QLocalServer>
#include <QScopedPointer>

/*============================================================================
================================ BApplicationServerPrivate ===================
============================================================================*/

/*============================== Static public constants ===================*/

const QDataStream::Version BApplicationServerPrivate::DSVersion = QDataStream::Qt_5_0;

/*============================== Public constructors =======================*/

BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q, const QString &serverName, int timeout) :
    BBasePrivate(q), ServerName(serverName), OperationTimeout(timeout)
{
    //
}

BApplicationServerPrivate::~BApplicationServerPrivate()
{
    if ( server->isListening() )
        server->close();
    server->deleteLater();
}

/*============================== Public methods ============================*/

void BApplicationServerPrivate::init()
{
    bTest(QCoreApplication::instance(), "BApplicationServer", "There must be a QCoreApplication instance");
    server = new BGenericServer(BGenericServer::LocalServer);
    server->localServer()->setSocketOptions(QLocalServer::WorldAccessOption);
    connect( server, SIGNAL( newPendingConnection() ), this, SLOT( newPendingConnection() ) );
}

/*============================== Public slots ==============================*/

void BApplicationServerPrivate::newPendingConnection()
{
    QScopedPointer<BGenericSocket> s ( server->nextPendingConnection() );
    if ( s.isNull() )
        return;
    if ( !s->waitForReadyRead(OperationTimeout) )
        return;
    QByteArray ba = s->readAll();
    QDataStream in(ba);
    in.setVersion(DSVersion);
    bool message = false;
    QStringList args;
    in >> message;
    if (message)
    {
        in >> args;
        q_func()->handleMessage(args);
    }
    else
    {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(BApplicationServerPrivate::DSVersion);
        out << true;
        s->write(data);
        s->waitForBytesWritten(OperationTimeout);
    }
    s->close();
}

/*============================================================================
================================ BApplicationServer ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BApplicationServer::BApplicationServer(const QString &serverName, int operationTimeout) :
    BBase( *new BApplicationServerPrivate(this, serverName, operationTimeout) )
{
    d_func()->init();
}

BApplicationServer::~BApplicationServer()
{
    //
}

/*============================== Protected constructors ====================*/

BApplicationServer::BApplicationServer(BApplicationServerPrivate &d) :
    BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BApplicationServer::isValid() const
{
    return !d_func()->ServerName.isEmpty();
}

bool BApplicationServer::testServer() const
{
    const B_D(BApplicationServer);
    if ( d->ServerName.isEmpty() )
        return false;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(BApplicationServerPrivate::DSVersion);
    out << false;
    BGenericSocket s(BGenericSocket::LocalSocket);
    s.connectToHost(d->ServerName);
    bool b = s.waitForConnected(d->OperationTimeout) && s.write(data) &&
            s.waitForBytesWritten(d->OperationTimeout) && s.waitForReadyRead(d->OperationTimeout);
    if (!b)
        QLocalServer::removeServer(d->ServerName);
    return b;
}

bool BApplicationServer::listen()
{
    if ( !QCoreApplication::instance() )
        return false;
    B_D(BApplicationServer);
    if ( d->ServerName.isEmpty() )
        return false;
    if ( d->server->isListening() )
        return true;
    return d->server->listen(d->ServerName);
}

bool BApplicationServer::sendMessage(int &argc, char **argv)
{
    if ( argc < 1 || !argv || d_func()->ServerName.isEmpty() )
        return false;
    QStringList args;
    for (int i = 1; i < argc; ++i)
        args << argv[i];
    return sendMessage(args);
}

bool BApplicationServer::sendMessage(const QStringList &arguments)
{
    B_D(BApplicationServer);
    if ( d->server->isListening() )
        return false;
    if ( d->ServerName.isEmpty() )
        return false;
    QStringList args = !arguments.isEmpty() ? arguments : QStringList( QCoreApplication::arguments().mid(1) );
    if ( args.isEmpty() )
        return false;
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BApplicationServerPrivate::DSVersion);
    out << true;
    out << args;
    BGenericSocket s(BGenericSocket::LocalSocket);
    s.connectToHost(d->ServerName);
    if ( !s.waitForConnected(d->OperationTimeout) )
        return false;
    if ( !s.write(ba) )
        return false;
    return s.waitForBytesWritten(d->OperationTimeout);
}

/*============================== Protected methods =========================*/

void BApplicationServer::handleMessage(const QStringList &arguments)
{
    //
}
