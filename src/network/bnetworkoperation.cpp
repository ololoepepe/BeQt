#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bnetworkoperation_p.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>
#include <QTimer>
#include <QEventLoop>
#include <QVariant>

/*============================================================================
================================ BNetworkOperationPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkOperationPrivate::BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md) :
    BBasePrivate(q), MetaData(md)
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
}

void BNetworkOperationPrivate::setUploadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    bytesOutReady = bytesReady;
    bytesOutTotal = bytesTotal;
    QMetaObject::invokeMethod( q_func(), "uploadProgress", Q_ARG(qint64, bytesReady), Q_ARG(qint64, bytesTotal) );
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

BNetworkOperation::BNetworkOperation(const BNetworkOperationMetaData &metaData, QObject *parent) :
    QObject(parent), BBase( *new BNetworkOperationPrivate(this, metaData) )
{
    d_func()->init();
}

/*============================== Public methods ============================*/

const QByteArray &BNetworkOperation::data() const
{
    return d_func()->data;
}

QVariant BNetworkOperation::variantData() const
{
    return BSocketWrapper::dataToVariant( data() );
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

int BNetworkOperation::downloadProgress() const
{
    const B_D(BNetworkOperation);
    return d->bytesInTotal != 0 ? ( (qreal) d->bytesInReady / (qreal) d->bytesInTotal ) * 100 : 100;
}

int BNetworkOperation::uploadProgress() const
{
    const B_D(BNetworkOperation);
    return d->bytesOutTotal != 0 ? ( (qreal) d->bytesOutReady / (qreal) d->bytesOutTotal ) * 100 : 100;
}

bool BNetworkOperation::isFinished() const
{
    return d_func()->isFinished;
}

bool BNetworkOperation::waitForFinished(int msecs)
{
    if ( isFinished() || isError() )
        return true;
    if (!msecs)
        return isFinished() || isError();
    QEventLoop el;
    if (msecs > 0)
        QTimer::singleShot( msecs, &el, SLOT( quit() ) );
    connect( this, SIGNAL( finished() ), &el, SLOT( quit() ) );
    connect( this, SIGNAL( error() ), &el, SLOT( quit() ) );
    el.exec();
    return isFinished() || isError();
}
