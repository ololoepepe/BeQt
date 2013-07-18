#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bnetworkoperation_p.h"
#include "bnetworkconnection.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BeQt>

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QTimer>
#include <QVariant>
#include <QDataStream>

/*============================================================================
================================ BNetworkOperationPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkOperationPrivate::BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md,
                                                   BNetworkConnection *connection) :
    BBasePrivate(q), MetaData(md), Connection(connection)
{
    //
}

BNetworkOperationPrivate::~BNetworkOperationPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BNetworkOperationPrivate::init()
{
    autoDelete = false;
    finishedTimeoutMsecs = 0;
    isStarted = false;
    isError = false;
    bytesInReady = 0;
    bytesInTotal = -1;
    bytesOutReady = 0;
    bytesOutTotal = -1;
    isFinished = false;
}

void BNetworkOperationPrivate::setStarted()
{
    isStarted = true;
    QMetaObject::invokeMethod(q_func(), "started");
    if (finishedTimeoutMsecs > 0)
        QTimer::singleShot(finishedTimeoutMsecs, this, SLOT(testFinished()));
}

void BNetworkOperationPrivate::setError()
{
    isError = true;
    QMetaObject::invokeMethod(q_func(), "error");
    if (autoDelete)
        q_func()->deleteLater();
}

void BNetworkOperationPrivate::setDownloadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    bytesInReady = bytesReady;
    bytesInTotal = bytesTotal;
    QMetaObject::invokeMethod( q_func(), "downloadProgress", Q_ARG(qint64, bytesReady), Q_ARG(qint64, bytesTotal) );
    QMetaObject::invokeMethod( q_func(), "downloadProgress", Q_ARG(int, bytesReady), Q_ARG(int, bytesTotal) );
}

void BNetworkOperationPrivate::setUploadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    bytesOutReady = bytesReady;
    bytesOutTotal = bytesTotal;
    QMetaObject::invokeMethod( q_func(), "uploadProgress", Q_ARG(qint64, bytesReady), Q_ARG(qint64, bytesTotal) );
    QMetaObject::invokeMethod( q_func(), "uploadProgress", Q_ARG(int, bytesReady), Q_ARG(int, bytesTotal) );
}

void BNetworkOperationPrivate::setFinished(const QByteArray &dt)
{
    if ( !dt.isEmpty() )
        data = dt;
    isFinished = true;
    QMetaObject::invokeMethod(q_func(), "finished");
    if (autoDelete)
        q_func()->deleteLater();
}

/*============================== Public slots ==============================*/

void BNetworkOperationPrivate::testStarted()
{
    if (isStarted)
        return;
    Q_EMIT startedTimeout();
}

void BNetworkOperationPrivate::testFinished()
{
    if (isFinished)
        return;
    Q_EMIT finishedTimeout();
}

/*============================================================================
================================ BNetworkOperation ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkOperation::~BNetworkOperation()
{
    //
}

/*============================== Protected constructors ====================*/

BNetworkOperation::BNetworkOperation(BNetworkOperationPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Private constructors ======================*/

BNetworkOperation::BNetworkOperation(const BNetworkOperationMetaData &metaData, BNetworkConnection *parent) :
    QObject(parent), BBase( *new BNetworkOperationPrivate(this, metaData, parent) )
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BNetworkOperation::setAutoDelete(bool b)
{
    d_func()->autoDelete = b;
    if (b && (d_func()->isFinished || d_func()->isError))
        deleteLater();
}

void BNetworkOperation::setStartTimeout(int msecs, const QObject *receiver, const char *method)
{
    if (msecs <= 0 || d_func()->isStarted || d_func()->isError)
        return;
    if (!receiver || !method)
    {
        receiver = this;
        method = SLOT(cancell());
    }
    connect(d_func(), SIGNAL(startedTimeout()), receiver, method);
    QTimer::singleShot(msecs, d_func(), SLOT(testStarted()));
}

void BNetworkOperation::setFinishTimeout(int msecs, const QObject *receiver, const char *method)
{
    if (msecs <= 0 || d_func()->isFinished || d_func()->isError)
        return;
    if (!receiver || !method)
    {
        receiver = this;
        method = SLOT(cancell());
    }
    connect(d_func(), SIGNAL(finishedTimeout()), receiver, method);
    if (d_func()->isStarted)
        QTimer::singleShot(msecs, d_func(), SLOT(testFinished()));
    else
        d_func()->finishedTimeoutMsecs = msecs;
}

void BNetworkOperation::reply(const QByteArray &data)
{
    if (isRequest())
        return;
    connection()->sendReply(this, data);
}

void BNetworkOperation::reply(const QVariant &variant)
{
    if (isRequest())
        return;
    connection()->sendReply(this, variant);
}

void BNetworkOperation::onStarted(const QObject *receiver, const char *method, bool c)
{
    if (!receiver || !method)
        return;
    if (c)
        connect(this, SIGNAL(started()), receiver, method);
    else
        disconnect(this, SIGNAL(started()), receiver, method);
}

void BNetworkOperation::onFinished(const QObject *receiver, const char *method, bool c)
{
    if (!receiver || !method)
        return;
    if (c)
        connect(this, SIGNAL(finished()), receiver, method);
    else
        disconnect(this, SIGNAL(finished()), receiver, method);
}

void BNetworkOperation::onError(const QObject *receiver, const char *method, bool c)
{
    if (!receiver || !method)
        return;
    if (c)
        connect(this, SIGNAL(error()), receiver, method);
    else
        disconnect(this, SIGNAL(error()), receiver, method);
}

bool BNetworkOperation::autoDelete() const
{
    return d_func()->autoDelete;
}

BNetworkConnection *BNetworkOperation::connection() const
{
    return d_func()->Connection;
}

const QByteArray &BNetworkOperation::data() const
{
    return d_func()->data;
}

QVariant BNetworkOperation::variantData() const
{
    return BeQt::dataToVariant(data());
}

BNetworkOperationMetaData BNetworkOperation::metaData() const
{
    return d_func()->MetaData;
}

bool BNetworkOperation::isRequest() const
{
    return d_func()->MetaData.isRequest();
}

bool BNetworkOperation::isValid() const
{
    return d_func()->MetaData.isValid();
}

bool BNetworkOperation::isStarted() const
{
    return d_func()->isStarted;
}

bool BNetworkOperation::isError() const
{
    return d_func()->isError;
}

qint64 BNetworkOperation::downloadBytesReady() const
{
    return d_func()->bytesInReady;
}

qint64 BNetworkOperation::downloadBytesTotal() const
{
    return d_func()->bytesInTotal;
}

qint64 BNetworkOperation::uploadBytesReady() const
{
    return d_func()->bytesOutReady;
}

qint64 BNetworkOperation::uploadBytesTotal() const
{
    return d_func()->bytesOutTotal;
}

int BNetworkOperation::downloadProgress(int nth) const
{
    if (nth < 1)
        nth = 100;
    const B_D(BNetworkOperation);
    return (d->bytesInTotal != 0) ? ( (qreal) d->bytesInReady / (qreal) d->bytesInTotal ) * nth : 100;
}

int BNetworkOperation::uploadProgress(int nth) const
{
    if (nth < 1)
        nth = 100;
    const B_D(BNetworkOperation);
    return (d->bytesOutTotal != 0) ? ( (qreal) d->bytesOutReady / (qreal) d->bytesOutTotal ) * nth : 100;
}

bool BNetworkOperation::isFinished() const
{
    return d_func()->isFinished;
}

bool BNetworkOperation::waitForStarted(int msecs)
{
    if (d_func()->isStarted)
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(finished()), msecs);
    return d_func()->isStarted;
}

bool BNetworkOperation::waitForFinished(int msecs)
{
    if (d_func()->isFinished)
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(finished()), msecs);
    return d_func()->isFinished;
}

/*============================== Public slots ==============================*/

void BNetworkOperation::cancel()
{
    if (isFinished() || isError())
        return;
    Q_EMIT canceled();
}
