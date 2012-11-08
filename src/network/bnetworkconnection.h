#ifndef BNETWORKCONNECTON_H
#define BNETWORKCONNECTON_H

class BNetworkConnectionPrivate;
class BNetworkOperation;

class QString;
class QUuid;

#include "bgenericsocket.h"
#include "bsocketwrapper.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QByteArray>
#include <QAbstractSocket>

class B_NETWORK_EXPORT BNetworkConnection : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BNetworkConnection)
    Q_OBJECT
public:
    explicit BNetworkConnection(BGenericSocket *socket, QObject *parent = 0);
    explicit BNetworkConnection(BGenericSocket::SocketType type, QObject *parent = 0);
    //
    void setCriticalBufferSize(qint64 size);
    void setCloseOnCriticalBufferSize(bool close);
    void setDetailedLogMode(bool enabled);
    void setAutoDeleteSentReplies(bool enabled);
    void connectToHost(const QString &hostName, quint16 port = 0);
    bool connectToHostBlocking(const QString &hostName, quint16 port = 0, int msecs = 30000);
    void disconnectFromHost();
    bool disconnectFromHostBlocking(int msecs = 30000);
    void close();
    bool isValid() const;
    bool isConnected() const;
    const QUuid uniqueId() const;
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    qint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSize() const;
    bool detailedLogMode() const;
    bool autoDeleteSentReplies() const;
    QString peerAddress() const;
    BNetworkOperation *sendRequest( const QString &operation, const QByteArray &data = QByteArray() );
    bool sendReply(BNetworkOperation *operation, const QByteArray &data);
signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void requestSent(BNetworkOperation *operation);
    void replyReceived(BNetworkOperation *operation);
    void incomingRequest(BNetworkOperation *operation);
    void requestReceived(BNetworkOperation *operation);
    void replySent(BNetworkOperation *operation);
    void criticalBufferSizeReached();
protected:
    BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent = 0);
    //
    virtual void log(const QString &text);
private:
    Q_DISABLE_COPY(BNetworkConnection)
};

#endif // BNETWORKCONNECTON_H
