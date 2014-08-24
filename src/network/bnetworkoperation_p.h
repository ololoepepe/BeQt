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

#ifndef BNETWORKOPERATION_P_H
#define BNETWORKOPERATION_P_H

class BNetworkConnection;

#include "bnetworkoperation.h"

#include "bnetworkoperationmetadata.h"

#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>

/*============================================================================
================================ BNetworkOperationPrivate ====================
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperationPrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BNetworkOperation)
public:
    BNetworkConnection * const Connection;
    const BNetworkOperationMetaData MetaData;
public:
    bool autoDelete;
    qint64 bytesInReady;
    qint64 bytesInTotal;
    qint64 bytesOutReady;
    qint64 bytesOutTotal;
    QByteArray data;
    int finishedTimeoutMsecs;
    bool isCancelled;
    bool isError;
    bool isFinished;
    bool isStarted;
public:
    explicit BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md,
                                      BNetworkConnection *connection);
    ~BNetworkOperationPrivate();
public:
    void init();
    void setDownloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void setError();
    void setFinished(const QByteArray &dt = QByteArray());
    void setStarted();
    void setUploadProgress(qint64 bytesReady, qint64 bytesTotal);
public Q_SLOTS:
    void testFinished();
    void testStarted();
Q_SIGNALS:
    void finishedTimeout();
    void startedTimeout();
private:
    Q_DISABLE_COPY(BNetworkOperationPrivate)
};

#endif // BNETWORKOPERATION_P_H
