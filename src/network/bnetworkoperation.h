#ifndef BNETWORKOPERATION_H
#define BNETWORKOPERATION_H

class BNetworkConnection;
class BNetworkOperationPrivate;
class BNetworkConnectionPrivate;

class QVariant;

#include "bsocketwrapper.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QObject>
#include <QByteArray>

/*============================================================================
================================ BNetworkOperation ===========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperation : public QObject, public BBase
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
    void setAutoDelete(bool b);
    void setStartTimeout(int msecs = 30 * BeQt::Second, const QObject *receiver = 0, const char *method = 0);
    void setFinishTimeout(int msecs = 30 * BeQt::Second, const QObject *receiver = 0, const char *method = 0);
    void reply(const QByteArray &data = QByteArray());
    void reply(const QVariant &variant);
    void onStarted(const QObject *receiver, const char *method, bool c = true);
    void onFinished(const QObject *receiver, const char *method, bool c = true);
    void onError(const QObject *receiver, const char *method, bool c = true);
    bool autoDelete() const;
    BNetworkConnection *connection() const;
    const QByteArray &data() const;
    QVariant variantData() const;
    BNetworkOperationMetaData metaData() const;
    bool isRequest() const;
    bool isValid() const;
    bool isStarted() const;
    bool isError() const;
    qint64 downloadBytesReady() const;
    qint64 downloadBytesTotal() const;
    qint64 uploadBytesReady() const;
    qint64 uploadBytesTotal() const;
    int downloadProgress(int nth = 100) const;
    int uploadProgress(int nth = 100) const;
    bool isFinished() const;
    bool waitForStarted(int msecs = 30 * BeQt::Second);
    bool waitForFinished(int msecs = 30 * BeQt::Second);
public Q_SLOTS:
    void cancel();
Q_SIGNALS:
    void started();
    void error();
    void downloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void downloadProgress(int bytesReady, int bytesTotal);
    void uploadProgress(int bytesReady, int bytesTotal);
    void finished();
    void canceled();
private:
    Q_DISABLE_COPY(BNetworkOperation)
    friend class BNetworkConnectionPrivate;
};

#endif // BNETWORKOPERATION_H
