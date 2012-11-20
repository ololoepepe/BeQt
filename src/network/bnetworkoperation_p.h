#ifndef BNETWORKOPERATION_P_H
#define BNETWORKOPERATION_P_H

class BNetworkConnectionPrivate;

#include "bnetworkoperation.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QByteArray>

/*============================================================================
================================ Network Operation Private
============================================================================*/

class B_NETWORK_EXPORT BNetworkOperationPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BNetworkOperation)
public:
    explicit BNetworkOperationPrivate(BNetworkOperation *q, const BNetworkOperationMetaData &md);
    ~BNetworkOperationPrivate();
    //
    void init();
    void setStarted();
    void setError();
    void setDownloadProgress(qint64 bytesReady, qint64 bytesTotal);
    void setUploadProgress(qint64 bytesReady, qint64 bytesTotal);
    void setFinished( const QByteArray &dt = QByteArray() );
    //
    const BNetworkOperationMetaData MetaData;
    //
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
    //
    friend class BNetworkConnectionPrivate;
};

#endif // BNETWORKOPERATION_P_H
