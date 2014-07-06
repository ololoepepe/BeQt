/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bnetworkoperation.h"
#include "bnetworkoperation_p.h"

#include "bnetworkconnection.h"
#include "bsocketwrapper.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>
#include <QElapsedTimer>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariant>

/*============================================================================
================================ BNetworkOperationPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BNetworkOperationPrivate::BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md,
                                                   BNetworkConnection *connection) :
    BBaseObjectPrivate(q), Connection(connection), MetaData(md)
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

void BNetworkOperationPrivate::setDownloadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    bytesInReady = bytesReady;
    bytesInTotal = bytesTotal;
    QMetaObject::invokeMethod(q_func(), "downloadProgress", Q_ARG(qint64, bytesReady), Q_ARG(qint64, bytesTotal));
    QMetaObject::invokeMethod(q_func(), "downloadProgress", Q_ARG(int, bytesReady), Q_ARG(int, bytesTotal));
}

void BNetworkOperationPrivate::setError()
{
    isError = true;
    QMetaObject::invokeMethod(q_func(), "error");
    if (autoDelete)
        q_func()->deleteLater();
}

void BNetworkOperationPrivate::setFinished(const QByteArray &dt)
{
    if (!dt.isEmpty())
        data = dt;
    isFinished = true;
    QMetaObject::invokeMethod(q_func(), "finished");
    if (autoDelete)
        q_func()->deleteLater();
}

void BNetworkOperationPrivate::setStarted()
{
    isStarted = true;
    QMetaObject::invokeMethod(q_func(), "started");
    if (finishedTimeoutMsecs > 0)
        QTimer::singleShot(finishedTimeoutMsecs, this, SLOT(testFinished()));
}

void BNetworkOperationPrivate::setUploadProgress(qint64 bytesReady, qint64 bytesTotal)
{
    bytesOutReady = bytesReady;
    bytesOutTotal = bytesTotal;
    QMetaObject::invokeMethod(q_func(), "uploadProgress", Q_ARG(qint64, bytesReady), Q_ARG(qint64, bytesTotal));
    QMetaObject::invokeMethod(q_func(), "uploadProgress", Q_ARG(int, bytesReady), Q_ARG(int, bytesTotal));
}

/*============================== Public slots ==============================*/

void BNetworkOperationPrivate::testFinished()
{
    if (isFinished)
        return;
    Q_EMIT finishedTimeout();
}

void BNetworkOperationPrivate::testStarted()
{
    if (isStarted)
        return;
    Q_EMIT startedTimeout();
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
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Private constructors ======================*/

BNetworkOperation::BNetworkOperation(const BNetworkOperationMetaData &metaData, BNetworkConnection *parent) :
    QObject(parent), BBaseObject(*new BNetworkOperationPrivate(this, metaData, parent))
{
    d_func()->init();
}

/*============================== Public methods ============================*/

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

qint64 BNetworkOperation::downloadBytesReady() const
{
    return d_func()->bytesInReady;
}

qint64 BNetworkOperation::downloadBytesTotal() const
{
    return d_func()->bytesInTotal;
}

int BNetworkOperation::downloadProgress(int nth) const
{
    if (nth < 1)
        nth = 100;
    const B_D(BNetworkOperation);
    return (d->bytesInTotal > 0) ? ((qreal(d->bytesInReady) / qreal(d->bytesInTotal)) * nth) : 100;
}

bool BNetworkOperation::isError() const
{
    return d_func()->isError;
}

bool BNetworkOperation::isFinished() const
{
    return d_func()->isFinished;
}

bool BNetworkOperation::isRequest() const
{
    return d_func()->MetaData.isRequest();
}

bool BNetworkOperation::isStarted() const
{
    return d_func()->isStarted;
}

bool BNetworkOperation::isValid() const
{
    return d_func()->MetaData.isValid();
}

BNetworkOperationMetaData BNetworkOperation::metaData() const
{
    return d_func()->MetaData;
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

void BNetworkOperation::onFinished(const QObject *receiver, const char *method, bool c)
{
    if (!receiver || !method)
        return;
    if (c)
        connect(this, SIGNAL(finished()), receiver, method);
    else
        disconnect(this, SIGNAL(finished()), receiver, method);
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

void BNetworkOperation::setAutoDelete(bool b)
{
    d_func()->autoDelete = b;
    if (b && (d_func()->isFinished || d_func()->isError))
        deleteLater();
}

void BNetworkOperation::setFinishTimeout(int msecs, const QObject *receiver, const char *method)
{
    if (msecs <= 0 || d_func()->isFinished || d_func()->isError)
        return;
    if (!receiver || !method) {
        receiver = this;
        method = SLOT(cancell());
    }
    connect(d_func(), SIGNAL(finishedTimeout()), receiver, method);
    if (d_func()->isStarted)
        QTimer::singleShot(msecs, d_func(), SLOT(testFinished()));
    else
        d_func()->finishedTimeoutMsecs = msecs;
}

void BNetworkOperation::setStartTimeout(int msecs, const QObject *receiver, const char *method)
{
    if (msecs <= 0 || d_func()->isStarted || d_func()->isError)
        return;
    if (!receiver || !method) {
        receiver = this;
        method = SLOT(cancell());
    }
    connect(d_func(), SIGNAL(startedTimeout()), receiver, method);
    QTimer::singleShot(msecs, d_func(), SLOT(testStarted()));
}

qint64 BNetworkOperation::uploadBytesReady() const
{
    return d_func()->bytesOutReady;
}

qint64 BNetworkOperation::uploadBytesTotal() const
{
    return d_func()->bytesOutTotal;
}

int BNetworkOperation::uploadProgress(int nth) const
{
    if (nth < 1)
        nth = 100;
    const B_D(BNetworkOperation);
    return (d->bytesOutTotal > 0) ? ((qreal(d->bytesOutReady) / qreal(d->bytesOutTotal)) * nth) : 100;
}

QVariant BNetworkOperation::variantData() const
{
    return BeQt::deserialize(data());
}

bool BNetworkOperation::waitForDownload(qint64 bytes, int msecs)
{
    if (bytes <= 0)
        return false;
    if (downloadBytesReady() >= bytes)
        return true;
    if (msecs >= 0) {
        while (msecs > 0) {
            QElapsedTimer timer;
            timer.start();
            BeQt::waitNonBlocking(this, SIGNAL(downloadProgress(qint64, qint64)), msecs);
            msecs -= (int) timer.elapsed();
            if (downloadBytesReady() >= bytes)
                return true;
        }
        return downloadBytesReady() >= bytes;
    } else {
        BeQt::waitNonBlocking(this, SIGNAL(downloadProgress(qint64, qint64)), msecs);
        return downloadBytesReady() >= bytes;
    }
}

bool BNetworkOperation::waitForDownloadProgress(int n, int nth, int msecs)
{
    if (n < 1)
        return false;
    if (nth < 1)
        nth = 100;
    if (n > nth)
        return false;
    if (downloadProgress(nth) >= n)
        return true;
    if (msecs >= 0) {
        while (msecs > 0) {
            QElapsedTimer timer;
            timer.start();
            BeQt::waitNonBlocking(this, SIGNAL(downloadProgress(int, int)), msecs);
            msecs -= (int) timer.elapsed();
            if (downloadProgress(nth) >= n)
                return true;
        }
        return downloadProgress(nth) >= n;
    } else {
        BeQt::waitNonBlocking(this, SIGNAL(downloadProgress(int, int)), msecs);
        return downloadProgress(nth) >= n;
    }
}

bool BNetworkOperation::waitForFinished(int msecs)
{
    if (d_func()->isFinished)
        return true;
    if (d_func()->isError)
        return false;
    BeQt::waitNonBlocking(this, SIGNAL(finished()), this, SIGNAL(error()), msecs);
    return d_func()->isFinished;
}

bool BNetworkOperation::waitForStarted(int msecs)
{
    if (d_func()->isStarted)
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(finished()), msecs);
    return d_func()->isStarted;
}

bool BNetworkOperation::waitForUpload(qint64 bytes, int msecs)
{
    if (bytes <= 0)
        return false;
    if (uploadBytesReady() >= bytes)
        return true;
    if (msecs >= 0) {
        while (msecs > 0) {
            QElapsedTimer timer;
            timer.start();
            BeQt::waitNonBlocking(this, SIGNAL(uploadProgress(qint64, qint64)), msecs);
            msecs -= (int) timer.elapsed();
            if (uploadBytesReady() >= bytes)
                return true;
        }
        return uploadBytesReady() >= bytes;
    } else {
        BeQt::waitNonBlocking(this, SIGNAL(uploadProgress(qint64, qint64)), msecs);
        return uploadBytesReady() >= bytes;
    }
}

bool BNetworkOperation::waitForUploadProgress(int n, int nth, int msecs)
{
    if (n < 1)
        return false;
    if (nth < 1)
        nth = 100;
    if (n > nth)
        return false;
    if (uploadProgress(nth) >= n)
        return true;
    if (msecs >= 0) {
        while (msecs > 0) {
            QElapsedTimer timer;
            timer.start();
            BeQt::waitNonBlocking(this, SIGNAL(uploadProgress(int, int)), msecs);
            msecs -= (int) timer.elapsed();
            if (uploadProgress(nth) >= n)
                return true;
        }
        return uploadProgress(nth) >= n;
    } else {
        BeQt::waitNonBlocking(this, SIGNAL(uploadProgress(int, int)), msecs);
        return uploadProgress(nth) >= n;
    }
}

/*============================== Public slots ==============================*/

void BNetworkOperation::cancel()
{
    if (isFinished() || isError())
        return;
    Q_EMIT canceled();
}
