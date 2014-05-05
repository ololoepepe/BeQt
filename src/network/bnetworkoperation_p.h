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

class BNetworkConnectionPrivate;
class BNetworkConnection;

#include "bnetworkoperation.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QByteArray>

/*============================================================================
================================ BNetworkOperationPrivate ====================
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperationPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BNetworkOperation)
public:
    explicit BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md,
                                      BNetworkConnection *connection);
    ~BNetworkOperationPrivate();
public:
    void init();
    void setStarted();
    void setError();
    void setDownloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void setUploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void setFinished( const QByteArray &dt = QByteArray() );
public Q_SLOTS:
    void testStarted();
    void testFinished();
Q_SIGNALS:
    void startedTimeout();
    void finishedTimeout();
public:
    const BNetworkOperationMetaData MetaData;
    BNetworkConnection *const Connection;
public:
    bool autoDelete;
    int finishedTimeoutMsecs;
    bool isStarted;
    bool isError;
    qint64 bytesInReady;
    qint64 bytesInTotal;
    qint64 bytesOutReady;
    qint64 bytesOutTotal;
    bool isFinished;
    QByteArray data;
private:
    Q_DISABLE_COPY(BNetworkOperationPrivate)
    friend class BNetworkConnectionPrivate;
};

#endif // BNETWORKOPERATION_P_H
