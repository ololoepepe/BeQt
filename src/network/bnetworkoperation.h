#ifndef BNETWORKOPERATION_H
#define BNETWORKOPERATION_H

class BNetworkOperationPrivate;
class BNetworkConnectionPrivate;

#include "bsocketwrapper.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QObject>
#include <QString>
#include <QUuid>
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
    explicit BNetworkOperation(const BNetworkOperationMetaData &metaData, QObject *parent = 0);
public:
    const QByteArray &data() const;
    BNetworkOperationMetaData metaData() const;
    bool isRequest() const;
    bool isValid() const;
    bool isStarted() const;
    bool isError() const;
    int downloadProgress() const;
    int uploadProgress() const;
    bool isFinished() const;
    bool waitForFinished(int msecs = 30 * BeQt::Second);
signals:
    void started();
    void error();
    void downloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void finished();
private:
    Q_DISABLE_COPY(BNetworkOperation)
    friend class BNetworkConnectionPrivate;
};

#endif // BNETWORKOPERATION_H
