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

const int BApplicationServerPrivate::OperationTimeout = 5 * BeQt::Second;
const QDataStream::Version BApplicationServerPrivate::DSVersion = QDataStream::Qt_4_8;

/*============================== Public constructors =======================*/

BApplicationServerPrivate::BApplicationServerPrivate(BApplicationServer *q) :
    BBasePrivate(q)
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
    //TODO: In Qt5, set socket options
    //server->localServer()->setSocketOptions(QLocalServer::WorldAccessOption);
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

BApplicationServer::BApplicationServer() :
    BBase( *new BApplicationServerPrivate(this) )
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

bool BApplicationServer::tryListen(const QString &serverName)
{
    if ( !QCoreApplication::instance() )
        return false;
    if ( serverName.isEmpty() )
        return false;
    B_D(BApplicationServer);
    if ( d->server->isListening() )
        return true;
    if ( d->server->listen(serverName) )
        return true;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(BApplicationServerPrivate::DSVersion);
    out << false;
    BGenericSocket s(BGenericSocket::LocalSocket);
    s.connectToHost(serverName);
    bool b = s.waitForConnected(BApplicationServerPrivate::OperationTimeout) && s.write(data);
    b = b && s.waitForBytesWritten(BApplicationServerPrivate::OperationTimeout);
    b = b && s.waitForReadyRead(BApplicationServerPrivate::OperationTimeout);
    return b ? false : QLocalServer::removeServer(serverName) && d->server->listen(serverName);
}

bool BApplicationServer::sendMessage(const QString &serverName, int &argc, char **argv)
{
    if ( argc < 1 || !argv || serverName.isEmpty() )
        return false;
    QStringList args;
    for (int i = 1; i < argc; ++i)
        args << argv[i];
    return sendMessage(serverName, args);
}

bool BApplicationServer::sendMessage(const QString &serverName, const QStringList &arguments)
{
    if ( d_func()->server->isListening() )
        return false;
    if ( serverName.isEmpty() )
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
    s.connectToHost(serverName);
    if ( !s.waitForConnected(BApplicationServerPrivate::OperationTimeout) )
        return false;
    if ( !s.write(ba) )
        return false;
    return s.waitForBytesWritten(BApplicationServerPrivate::OperationTimeout);
}

/*============================== Protected methods =========================*/

void BApplicationServer::handleMessage(const QStringList &arguments)
{
    //
}
