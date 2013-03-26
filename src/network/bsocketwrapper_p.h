#ifndef BSOCKETWRAPPER_P_H
#define BSOCKETWRAPPER_P_H

class BSocketWrapperPrivate;

#include "bgenericsocket.h"
#include "bsocketwrapper.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BeQtGlobal>

#include <QObject>
#include <QAbstractSocket>
#include <QPointer>
#include <QDataStream>

/*============================================================================
================================ BSocketWrapperPrivate =======================
============================================================================*/

class B_NETWORK_EXPORT BSocketWrapperPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSocketWrapper)
public:
    explicit BSocketWrapperPrivate(BSocketWrapper *q);
    ~BSocketWrapperPrivate();
public:
    void init();
    void resetIn();
    void resetOut();
public slots:
    void bytesWritten(qint64 bytes);
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readyRead();
public:
    QPointer<BGenericSocket> socket;
    qint64 bytesInTotal;
    BNetworkOperationMetaData metaIn;
    qint64 bytesOutTotal;
    qint64 bytesOutReady;
    BNetworkOperationMetaData metaOut;
    QDataStream::Version dsVersion;
    int comprLvl;
    qint64 criticalBufferSize;
    bool closeOnCriticalBufferSize;
private:
    Q_DISABLE_COPY(BSocketWrapperPrivate)
};

#endif // BSOCKETWRAPPER_P_H
