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
#include <QEventLoop>
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
}

void BNetworkOperationPrivate::setError()
{
    isError = true;
    QMetaObject::invokeMethod(q_func(), "error");
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
    QDataStream in(data());
    in.setVersion(BeQt::DataStreamVersion);
    QVariant v;
    in >> v;
    return v;
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

bool BNetworkOperation::waitForFinished(int msecs)
{
    if (isFinished() || isError())
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(finished()), this, SIGNAL(error()), msecs);
    return isFinished() || isError();
}

/*============================== Public slots ==============================*/

void BNetworkOperation::cancel()
{
    if (isFinished() || isError())
        return;
    emit canceled();
}
