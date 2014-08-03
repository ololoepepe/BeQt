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

#ifndef BNETWORKOPERATION_H
#define BNETWORKOPERATION_H

class BNetworkOperationPrivate;

class BNetworkConnection;
class BNetworkConnectionPrivate;
class BNetworkOperationMetaData;

class QVariant;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBaseObject>

#include <QObject>
#include <QByteArray>

/*============================================================================
================================ BNetworkOperation ===========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperation : public QObject, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkOperation)
public:
    ~BNetworkOperation();
protected:
    explicit BNetworkOperation(BNetworkOperationPrivate &d, QObject *parent = 0);
private:
    explicit BNetworkOperation(const BNetworkOperationMetaData &metaData, BNetworkConnection *parent);
public:
    bool autoDelete() const;
    BNetworkConnection *connection() const;
    const QByteArray &data() const;
    qint64 downloadBytesReady() const;
    qint64 downloadBytesTotal() const;
    int downloadProgress(int nth = 100) const;
    bool isError() const;
    bool isFinished() const;
    bool isRequest() const;
    bool isStarted() const;
    bool isValid() const;
    BNetworkOperationMetaData metaData() const;
    void onError(const QObject *receiver, const char *method, bool c = true);
    void onFinished(const QObject *receiver, const char *method, bool c = true);
    void onStarted(const QObject *receiver, const char *method, bool c = true);
    bool reply(const QByteArray &data = QByteArray());
    bool reply(const QVariant &variant);
    bool reply(int compressionLevel, const QByteArray &data = QByteArray());
    bool reply(int compressionLevel, const QVariant &variant);
    void setAutoDelete(bool b);
    void setFinishTimeout(int msecs = 30 * BeQt::Second, const QObject *receiver = 0, const char *method = 0);
    void setStartTimeout(int msecs = 30 * BeQt::Second, const QObject *receiver = 0, const char *method = 0);
    qint64 uploadBytesReady() const;
    qint64 uploadBytesTotal() const;
    int uploadProgress(int nth = 100) const;
    QVariant variantData() const;
    bool waitForDownload(qint64 bytes, int msecs = 30 * BeQt::Second);
    bool waitForDownloadProgress(int n, int nth = 100, int msecs = 30 * BeQt::Second);
    bool waitForFinished(int msecs = 30 * BeQt::Second);
    bool waitForStarted(int msecs = 30 * BeQt::Second);
    bool waitForUpload(qint64 bytes, int msecs = 30 * BeQt::Second);
    bool waitForUploadProgress(int n, int nth = 100, int msecs = 30 * BeQt::Second);
public Q_SLOTS:
    void cancel();
Q_SIGNALS:
    void canceled();
    void downloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void downloadProgress(int bytesReady, int bytesTotal);
    void error();
    void finished();
    void started();
    void uploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(int bytesReady, int bytesTotal);
private:
    Q_DISABLE_COPY(BNetworkOperation)
    friend class BNetworkConnectionPrivate;
};

#endif // BNETWORKOPERATION_H
