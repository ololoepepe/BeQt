#ifndef BSOCKETWRAPPER_H
#define BSOCKETWRAPPER_H

class BSocketWrapperPrivate;

class QByteArray;
class QVariant;

#include "bgenericsocket.h"
#include "bnetworkoperationmetadata.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QDataStream>

/*============================================================================
================================ BSocketWrapper ==============================
============================================================================*/

class B_NETWORK_EXPORT BSocketWrapper : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSocketWrapper)
public:
    explicit BSocketWrapper(QObject *parent = 0);
    explicit BSocketWrapper(BGenericSocket *socket, QObject *parent = 0);
    explicit BSocketWrapper(BGenericSocket::SocketType type, QObject *parent = 0);
    ~BSocketWrapper();
protected:
    explicit BSocketWrapper(BSocketWrapperPrivate &d, QObject *parent = 0);
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    static QByteArray variantToData(const QVariant &variant, QDataStream::Version version = QDataStream::Qt_4_8);
    static QVariant dataToVariant(const QByteArray &data, QDataStream::Version version = QDataStream::Qt_4_8);
#else
    static QByteArray variantToData(const QVariant &variant, QDataStream::Version version = QDataStream::Qt_5_0);
    static QVariant dataToVariant(const QByteArray &data, QDataStream::Version version = QDataStream::Qt_5_0);
#endif
public:
    void setSocket(BGenericSocket *socket);
    void setDataStreamVersion(QDataStream::Version version);
    void setCompressionLevel(int level);
    void setCriticalBufferSize(qint64 size);
    void setCloseOnCriticalBufferSize(bool close);
    bool unsetSocket();
    BGenericSocket *socket() const;
    QDataStream::Version dataStreamVersion() const;
    int compressionLevel() const;
    qint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSize() const;
    bool isBuisy() const;
    bool sendData( const QByteArray &data, const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData() );
    bool sendData( const QVariant &variant, const BNetworkOperationMetaData &metaData = BNetworkOperationMetaData() );
signals:
    void downloadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void uploadProgress(const BNetworkOperationMetaData &metaData, qint64 bytesReady, qint64 bytesTotal);
    void dataReceived(const QByteArray &data, const BNetworkOperationMetaData &metaData);
    void dataSent(const BNetworkOperationMetaData &metaData);
    void criticalBufferSizeReached();
private:
    Q_DISABLE_COPY(BSocketWrapper)
};

#endif // BSOCKETWRAPPER_H
