#include "bnetworkoperation.h"
#include "bsocketwrapper.h"
#include "bnetworkoperation_p.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QString>
#include <QUuid>
#include <QByteArray>

/*============================================================================
================================ Network Operation Private
============================================================================*/

BNetworkOperationPrivate::BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md) :
    BBasePrivate(q), MetaData(md)
{
    isStarted = false;
    isError = false;
    bytesInReady = 0;
    bytesInTotal = -1;
    bytesOutReady = 0;
    bytesOutTotal = -1;
    isFinished = false;
}

BNetworkOperationPrivate::~BNetworkOperationPrivate()
{
    //
}

//

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

//

BNetworkOperationPrivate::BNetworkOperationPrivate(BNetworkOperation &q, BBasePrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Network Operation
============================================================================*/

BNetworkOperation::~BNetworkOperation()
{
    emit destroyed(d_func()->MetaData);
}

//

const QByteArray &BNetworkOperation::data() const
{
    return d_func()->data;
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

//

BNetworkOperation::BNetworkOperation(BNetworkOperationPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}

//

BNetworkOperation::BNetworkOperation(const BNetworkOperationMetaData &metaData, QObject *parent) :
    QObject(parent), BBase( *new BNetworkOperationPrivate(this, metaData) )
{
    //
}
