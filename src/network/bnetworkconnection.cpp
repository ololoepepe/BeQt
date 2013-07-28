#include "bnetworkconnection.h"
#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bgenericsocket.h"
#include "bnetworkconnection_p.h"
#include "bnetworkoperation_p.h"

#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BLogger>
#include <BeQtCore/BTerminalIOHandler>

#include <QObject>
#include <QDataStream>
#include <QString>
#include <QIODevice>
#include <QAbstractSocket>
#include <QThread>
#include <QDateTime>
#include <QMetaObject>
#include <QVariant>

/*============================================================================
================================ BNetworkConnectionPrivate ===================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkConnectionPrivate::BNetworkConnectionPrivate(BNetworkConnection *q, BNetworkServer *server) :
    BBasePrivate(q), UniqueId( QUuid::createUuid() ), Server(server)
{
    //
}

BNetworkConnectionPrivate::~BNetworkConnectionPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BNetworkConnectionPrivate::init()
{
    loggingMode = BNetworkConnection::NormalLogging;
    autoDelete = true;
    logger = 0;
    translations = false;
    socketWrapper = new BSocketWrapper( q_func() );
    connect( socketWrapper, SIGNAL( downloadProgress(BNetworkOperationMetaData, qint64, qint64) ),
             this, SLOT( downloadProgress(BNetworkOperationMetaData, qint64, qint64) ) );
    connect( socketWrapper, SIGNAL( uploadProgress(BNetworkOperationMetaData, qint64, qint64) ),
             this, SLOT( uploadProgress(BNetworkOperationMetaData, qint64, qint64) ) );
    connect( socketWrapper, SIGNAL( dataReceived(QByteArray, BNetworkOperationMetaData) ),
             this, SLOT( dataReceived(QByteArray, BNetworkOperationMetaData) ) );
    connect( socketWrapper, SIGNAL( dataSent(BNetworkOperationMetaData) ),
             this, SLOT( dataSent(BNetworkOperationMetaData) ) );
    connect( socketWrapper, SIGNAL( criticalBufferSizeReached() ), q_func(), SIGNAL( criticalBufferSizeReached() ) );
}

void BNetworkConnectionPrivate::setSocket(BGenericSocket *s)
{
    if (!s)
        return;
    socket = s;
    socketWrapper->setSocket(s);
    socket->setParent( q_func() );
    connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
    connect( socket, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
    connect( socket, SIGNAL( error(QAbstractSocket::SocketError) ),
             this, SLOT( error(QAbstractSocket::SocketError) ) );
}

void BNetworkConnectionPrivate::sendNext()
{
    if ( !q_func()->isConnected() || socketWrapper->isBuisy() || dataQueue.isEmpty() )
        return;
    Data data = dataQueue.dequeue();
    if ( !data.second.isValid() )
        return;
    BNetworkOperation *op = data.second.isRequest() ? requests.value(data.second) : replies.value(data.second);
    if (op)
    {
        BNetworkOperationPrivate *opd = op->d_func();
        socketWrapper->sendData(data.first, data.second) ? opd->setStarted() : opd->setError();
    }
}

BNetworkOperation *BNetworkConnectionPrivate::createOperation(const BNetworkOperationMetaData &metaData)
{
    BNetworkOperation *op = new BNetworkOperation(metaData, q_func());
    operations.insert(op, metaData);
    if (autoDelete && !metaData.isRequest())
        op->setAutoDelete(true);
    connect(op, SIGNAL(canceled()), this, SLOT(operationCanceled()));
    connect(op, SIGNAL(destroyed(QObject *)), this, SLOT(operationDestroyed(QObject *)));
    return op;
}

/*============================== Public slots ==============================*/

void BNetworkConnectionPrivate::connected()
{
    B_Q(BNetworkConnection);
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q->log(translations ? tr("Connected", "log text") : QString("Connected"));
    QMetaObject::invokeMethod(q, "connected");
}

void BNetworkConnectionPrivate::disconnected()
{
    B_Q(BNetworkConnection);
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q->log(translations ? tr("Disconnected", "log text") : QString("Disconnected"));
    QMetaObject::invokeMethod(q, "disconnected");
    foreach (BNetworkOperation *op, QList<BNetworkOperation *>() << requests.values() << replies.values())
        if (!op->isFinished() && !op->isError())
            op->d_func()->setError();
}

void BNetworkConnectionPrivate::error(QAbstractSocket::SocketError socketError)
{
    B_Q(BNetworkConnection);
    if (loggingMode >= BNetworkConnection::NormalLogging)
        q->log((translations ? tr("Error:", "log text") : QString("Error:")) + " " + q->errorString());
    QMetaObject::invokeMethod( q, "error", Q_ARG(QAbstractSocket::SocketError, socketError) );
    foreach (BNetworkOperation *op, QList<BNetworkOperation *>() << requests.values() << replies.values())
        if (!op->isFinished() && !op->isError())
            op->d_func()->setError();
}

void BNetworkConnectionPrivate::downloadProgress(const BNetworkOperationMetaData &metaData,
                                                 qint64 bytesReady, qint64 bytesTotal)
{
    B_Q(BNetworkConnection);
    if ( metaData.isRequest() )
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = replies.value(mdat);
        if (op)
        {
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
        }
        else
        {
            op = createOperation(mdat);
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
            replies.insert(mdat, op);
            op->d_func()->setStarted();
            if (loggingMode >= BNetworkConnection::DetailedLogging)
                q->log((translations ? tr("Incoming request:", "log text") : QString("Incoming request:")) + " "
                       + metaData.operation());
            QMetaObject::invokeMethod( q, "incomingRequest", Q_ARG(BNetworkOperation *, op) );
        }
    }
    else
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(true);
        BNetworkOperation *op = requests.value(mdat);
        if (op)
            op->d_func()->setDownloadProgress(bytesReady, bytesTotal);
    }
}

void BNetworkConnectionPrivate::uploadProgress(const BNetworkOperationMetaData &metaData,
                                               qint64 bytesReady, qint64 bytesTotal)
{
    BNetworkOperation *op = metaData.isRequest() ? requests.value(metaData) : replies.value(metaData);
    if (op)
        op->d_func()->setUploadProgress(bytesReady, bytesTotal);
}

void BNetworkConnectionPrivate::dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData)
{
    B_Q(BNetworkConnection);
    if ( metaData.isRequest() )
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = replies.value(mdat);
        if (!op)
            return;
        BNetworkOperationPrivate *opd = op->d_func();
        opd->bytesInReady = opd->bytesInTotal;
        opd->data = data;
        if (loggingMode >= BNetworkConnection::DetailedLogging)
            q->log((translations ? tr("Request received:", "log text") : QString("Request received:")) + " "
                   + metaData.operation());
        QMetaObject::invokeMethod( q, "requestReceived", Q_ARG(BNetworkOperation *, op) );
        QString opp = op->metaData().operation();
        if (internalRequestHandlers.contains(opp))
            (q->*internalRequestHandlers.value(opp))(op);
        else if (externalRequestHandlers.contains(opp))
            externalRequestHandlers.value(opp)(op);
        else
            q->handleRequest(op);
    }
    else
    {
        BNetworkOperationMetaData mdat = metaData;
        mdat.setIsRequest(false);
        BNetworkOperation *op = requests.value(mdat);
        if (!op)
            return;
        BNetworkOperationPrivate *opd = op->d_func();
        requests.remove(mdat);
        opd->bytesInReady = opd->bytesInTotal;
        opd->setFinished(data);
        if (loggingMode >= BNetworkConnection::DetailedLogging)
            q->log((translations ? tr("Reply received:", "log text") : QString("Reply received:")) + " "
                   + metaData.operation());
        QMetaObject::invokeMethod( q, "replyReceived", Q_ARG(BNetworkOperation *, op) );
        QString opp = op->metaData().operation();
        if (internalReplyHandlers.contains(opp))
            (q->*internalReplyHandlers.value(opp))(op);
        else if (externalReplyHandlers.contains(opp))
            externalReplyHandlers.value(opp)(op);
        else
            q->handleReply(op);
    }
}

void BNetworkConnectionPrivate::dataSent(const BNetworkOperationMetaData &metaData)
{
    B_Q(BNetworkConnection);
    if ( metaData.isRequest() )
    {
        BNetworkOperation *op = requests.value(metaData);
        if (!op)
            return;
        if (loggingMode >= BNetworkConnection::DetailedLogging)
            q->log((translations ? tr("Request sent:", "log text") : QString("Request sent:")) + " "
                   + metaData.operation());
        QMetaObject::invokeMethod( q, "requestSent", Q_ARG(BNetworkOperation *, op) );
    }
    else
    {
        BNetworkOperation *op = replies.value(metaData);
        if (!op)
            return;
        replies.remove(metaData);
        op->d_func()->setFinished();
        if (loggingMode >= BNetworkConnection::DetailedLogging)
            q->log((translations ? tr("Reply sent:", "log text") : QString("Reply sent:")) + " "
                   + metaData.operation());
        QMetaObject::invokeMethod(q, "replySent", Q_ARG(BNetworkOperation *, op));
    }
    sendNext();
}

void BNetworkConnectionPrivate::operationCanceled()
{
    q_func()->abort();
    foreach (BNetworkOperation *op, QList<BNetworkOperation *>() << requests.values() << replies.values())
        if (!op->isFinished() && !op->isError())
            op->d_func()->setError();
}

void BNetworkConnectionPrivate::operationDestroyed(QObject *obj)
{
    if ( !operations.contains(obj) )
        return;
    BNetworkOperationMetaData md = operations.take(obj);
    if ( md.isRequest() )
        requests.remove(md);
    else
        replies.remove(md);
}

/*============================================================================
================================ BNetworkConnection ==========================
============================================================================*/

/*============================== Public static methods =====================*/

QString BNetworkConnection::operation(StandardOperation op)
{
    switch (op)
    {
    case NoopOperation:
        return "noop";
    case WriteOperation:
        return "write";
    case LogOperation:
        return "log";
    default:
        return "";
    }
}

BNetworkConnection::InternalHandler BNetworkConnection::replyHandler(StandardOperation op)
{
    switch (op)
    {
    default:
        return 0;
    }
}

BNetworkConnection::InternalHandler BNetworkConnection::requestHandler(StandardOperation op)
{
    switch (op)
    {
    case NoopOperation:
        return &BNetworkConnection::handleNoopRequest;
    case WriteOperation:
        return &BNetworkConnection::handleWriteRequest;
    case LogOperation:
        return &BNetworkConnection::handleLogRequest;
    default:
        return 0;
    }
}

/*============================== Public constructors =======================*/

BNetworkConnection::BNetworkConnection(BGenericSocket *socket, QObject *parent) :
    QObject(parent), BBase( *new BNetworkConnectionPrivate(this) )
{
    d_func()->init();
    if ( !socket || socket->thread() != thread() || !socket->isOpen() )
        return;
    d_func()->setSocket(socket);
    if (d_func()->loggingMode >= NormalLogging)
        log(d_func()->translations ? tr("Incoming connection", "log text") : QString("Incoming connection"));
}

BNetworkConnection::BNetworkConnection(BNetworkServer *server, BGenericSocket *socket) :
    QObject(0), BBase( *new BNetworkConnectionPrivate(this, server) )
{
    d_func()->init();
    if ( !socket || socket->thread() != thread() || !socket->isOpen() )
        return;
    d_func()->setSocket(socket);
    if (d_func()->loggingMode >= NormalLogging)
        log(d_func()->translations ? tr("Incoming connection", "log text") : QString("Incoming connection"));
}

BNetworkConnection::BNetworkConnection(BGenericSocket::SocketType type, QObject *parent) :
    QObject(parent), BBase( *new BNetworkConnectionPrivate(this) )
{
    d_func()->init();
    BGenericSocket *s = new BGenericSocket(type);
    if ( s->isSocketSet() )
        d_func()->setSocket(s);
    else
        s->deleteLater();
}

BNetworkConnection::~BNetworkConnection()
{
    //
}

/*============================== Protected constructors ====================*/

BNetworkConnection::BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BNetworkConnection::setCompressionLevel(int level)
{
    d_func()->socketWrapper->setCompressionLevel(level);
}

void BNetworkConnection::setCriticalBufferSize(qint64 size)
{
    d_func()->socketWrapper->setCriticalBufferSize(size);
}

void BNetworkConnection::setCloseOnCriticalBufferSize(bool close)
{
    d_func()->socketWrapper->setCloseOnCriticalBufferSize(close);
}

void BNetworkConnection::setLoggingMode(LoggingMode m)
{
    d_func()->loggingMode = m;
}

void BNetworkConnection::setAutoDeleteSentReplies(bool enabled)
{
    d_func()->autoDelete = enabled;
}

void BNetworkConnection::setLogger(BLogger *l)
{
    B_D(BNetworkConnection);
    if (d->logger && (!d->logger->parent() || d->logger->parent() == this))
        d->logger->deleteLater();
    d->logger = l;
    if (l && !l->parent())
        l->setParent(this);
}

void BNetworkConnection::setTranslationsEnabled(bool enabled)
{
    d_func()->translations = enabled;
}

void BNetworkConnection::connectToHost(const QString &hostName, quint16 port)
{
    if ( isConnected() || hostName.isEmpty() )
        return;
    d_func()->socket->connectToHost(hostName, port);
}

bool BNetworkConnection::connectToHostBlocking(const QString &hostName, quint16 port, int msecs)
{
    if ( isConnected() || hostName.isEmpty() )
        return false;
    B_D(BNetworkConnection);
    d->socket->connectToHost(hostName, port);
    return d->socket->waitForConnected(msecs);
}

bool BNetworkConnection::disconnectFromHostBlocking(int msecs)
{
    B_D(BNetworkConnection);
    d->socket->disconnectFromHost();
    return d->socket->waitForDisconnected(msecs);
}

bool BNetworkConnection::waitForConnected(int msecs)
{
    return d_func()->socket->waitForConnected(msecs);
}

bool BNetworkConnection::waitForDisconnected(int msecs)
{
    return d_func()->socket->waitForDisconnected(msecs);
}

void BNetworkConnection::installReplyHandler(const QString &operation, InternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->internalReplyHandlers.contains(operation) )
        return;
    d->internalReplyHandlers.insert(operation, handler);
}

void BNetworkConnection::installReplyHandler(const QString &operation, ExternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->externalReplyHandlers.contains(operation) )
        return;
    d->externalReplyHandlers.insert(operation, handler);
}

void BNetworkConnection::installReplyHandler(StandardOperation op)
{
    installReplyHandler(operation(op), replyHandler(op));
}

void BNetworkConnection::installRequestHandler(const QString &operation, InternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->internalRequestHandlers.contains(operation) )
        return;
    d->internalRequestHandlers.insert(operation, handler);
}

void BNetworkConnection::installRequestHandler(const QString &operation, ExternalHandler handler)
{
    if (operation.isEmpty() || !handler)
        return;
    B_D(BNetworkConnection);
    if ( d->externalRequestHandlers.contains(operation) )
        return;
    d->externalRequestHandlers.insert(operation, handler);
}

void BNetworkConnection::installRequestHandler(StandardOperation op)
{
    installRequestHandler(operation(op), requestHandler(op));
}

void BNetworkConnection::installRequestHandler(StandardOperation op, InternalHandler handler)
{
    installRequestHandler(operation(op), handler);
}

void BNetworkConnection::installRequestHandler(StandardOperation op, ExternalHandler handler)
{
    installRequestHandler(operation(op), handler);
}

bool BNetworkConnection::isValid() const
{
    const B_D(BNetworkConnection);
    return !d->socket.isNull() && d->socket->isSocketSet();
}

bool BNetworkConnection::isConnected() const
{
    return isValid() && d_func()->socket->state() == QAbstractSocket::ConnectedState;
}

const QUuid BNetworkConnection::uniqueId() const
{
    return d_func()->UniqueId;
}

BNetworkServer *BNetworkConnection::server() const
{
    return d_func()->Server;
}

QAbstractSocket::SocketError BNetworkConnection::error() const
{
    return isValid() ? d_func()->socket->error() : QAbstractSocket::UnknownSocketError;
}

QString BNetworkConnection::errorString() const
{
    return isValid() ? d_func()->socket->errorString() : "";
}

int BNetworkConnection::compressionLevel() const
{
    return d_func()->socketWrapper->compressionLevel();
}

qint64 BNetworkConnection::criticalBufferSize() const
{
    return d_func()->socketWrapper->criticalBufferSize();
}

bool BNetworkConnection::closeOnCriticalBufferSize() const
{
    return d_func()->socketWrapper->closeOnCriticalBufferSize();
}

BNetworkConnection::LoggingMode BNetworkConnection::loggingMode() const
{
    return d_func()->loggingMode;
}

bool BNetworkConnection::autoDeleteSentReplies() const
{
    return d_func()->autoDelete;
}

BLogger *BNetworkConnection::logger() const
{
    return d_func()->logger;
}

bool BNetworkConnection::translationsEnabled() const
{
    return d_func()->translations;
}

QString BNetworkConnection::peerAddress() const
{
    return isValid() ? d_func()->socket->peerAddress() : "";
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &op, const QByteArray &data)
{
    if (!isConnected() || op.isEmpty())
        return 0;
    B_D(BNetworkConnection);
    BNetworkConnectionPrivate::Data dat;
    dat.first = data;
    dat.second.setId(QUuid::createUuid());
    dat.second.setIsRequest(true);
    dat.second.setOperation(op);
    BNetworkOperation *nop = d->createOperation(dat.second);
    d->requests.insert(dat.second, nop);
    d->dataQueue.enqueue(dat);
    d->sendNext();
    return nop;
}

BNetworkOperation *BNetworkConnection::sendRequest(const QString &op, const QVariant &variant)
{
    return sendRequest(op, BeQt::variantToData(variant));
}

bool BNetworkConnection::sendReply(BNetworkOperation *op, const QByteArray &data)
{
    if (!isConnected() || !op || !op->isValid() || op->isRequest())
        return false;
    B_D(BNetworkConnection);
    BNetworkConnectionPrivate::Data dat;
    dat.first = data;
    dat.second = op->metaData();
    d->dataQueue.enqueue(dat);
    d->sendNext();
    return true;
}

bool BNetworkConnection::sendReply(BNetworkOperation *op, const QVariant &variant)
{
    return sendReply(op, BeQt::variantToData(variant));
}

/*============================== Public slots ==============================*/

void BNetworkConnection::disconnectFromHost()
{
    d_func()->socket->disconnectFromHost();
}

void BNetworkConnection::close()
{
    d_func()->socket->close();
}

void BNetworkConnection::abort()
{
    d_func()->socket->abort();
}

/*============================== Protected methods =========================*/

bool BNetworkConnection::handleReply(BNetworkOperation *)
{
    if (d_func()->loggingMode >= DetailedLogging)
        log(d_func()->translations ? tr("Unknown reply", "log text") : QString("Unknown reply"));
    return false;
}

bool BNetworkConnection::handleRequest(BNetworkOperation *)
{
    if (d_func()->loggingMode >= DetailedLogging)
        log(d_func()->translations ? tr("Unknown request", "log text") : QString("Unknown request"));
    return false;
}

void BNetworkConnection::log(const QString &text, BLogger::Level lvl)
{
    QString msg = "[" + peerAddress() + "] " + text;
    BLogger *logger = d_func()->logger ? d_func()->logger : BCoreApplication::logger();
    if (logger)
        logger->log(msg, lvl);
    else if (BLogger::isStderrLevel(lvl))
        BTerminalIOHandler::writeLineErr(msg);
    else
        BTerminalIOHandler::writeLine(msg);
}

BGenericSocket *BNetworkConnection::socket() const
{
    return d_func()->socket.data();
}

BSocketWrapper *BNetworkConnection::socketWrapper() const
{
    return d_func()->socketWrapper.data();
}

bool BNetworkConnection::handleNoopRequest(BNetworkOperation *op)
{
    op->reply();
    op->setAutoDelete(true);
    op->setStartTimeout();
    op->setFinishTimeout();
    return true;
}

bool BNetworkConnection::handleWriteRequest(BNetworkOperation *op)
{
    QVariantMap m = op->variantData().toMap();
    QString text = m.value("text").toString();
    BTerminalIOHandler::write(text);
    op->reply();
    op->setAutoDelete(true);
    op->setStartTimeout();
    op->setFinishTimeout();
    return true;
}

bool BNetworkConnection::handleLogRequest(BNetworkOperation *op)
{
    QVariantMap m = op->variantData().toMap();
    QString text = m.value("text").toString();
    BLogger::Level lvl = static_cast<BLogger::Level>(m.value("level").toInt());
    log(text, lvl);
    op->reply();
    op->setAutoDelete(true);
    op->setStartTimeout();
    op->setFinishTimeout();
    return true;
}
