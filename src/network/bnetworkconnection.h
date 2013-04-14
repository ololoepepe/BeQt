#ifndef BNETWORKCONNECTON_H
#define BNETWORKCONNECTON_H

class BNetworkConnectionPrivate;
class BNetworkOperation;
class BNetworkServer;

class QString;
class QUuid;
class QByteArray;
class QVariant;

#include "bgenericsocket.h"
#include "bsocketwrapper.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/BLogger>

#include <QObject>
#include <QByteArray>
#include <QAbstractSocket>

/*============================================================================
================================ BNetworkConnection ==========================
============================================================================*/

class B_NETWORK_EXPORT BNetworkConnection : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkConnection)
public:
    typedef void (BNetworkConnection::*InternalHandler)(BNetworkOperation *);
    typedef void (*ExternalHandler)(BNetworkOperation *);
public:
    explicit BNetworkConnection(BGenericSocket *socket, QObject *parent = 0);
    explicit BNetworkConnection(BNetworkServer *server, BGenericSocket *socket);
    explicit BNetworkConnection(BGenericSocket::SocketType type, QObject *parent = 0);
    ~BNetworkConnection();
protected:
    explicit BNetworkConnection(BNetworkConnectionPrivate &d, QObject *parent = 0);
public:
    void setCompressionLevel(int level);
    void setCriticalBufferSize(qint64 size);
    void setCloseOnCriticalBufferSize(bool close);
    void setDetailedLogMode(bool enabled);
    void setAutoDeleteSentReplies(bool enabled);
    void setLogger(BLogger *l);
    void connectToHost(const QString &hostName, quint16 port = 0);
    bool connectToHostBlocking(const QString &hostName, quint16 port = 0, int msecs = 30 * BeQt::Second);
    bool disconnectFromHostBlocking(int msecs = 30 * BeQt::Second);
    bool waitForConnected(int msecs = 30 * BeQt::Second);
    bool waitForDisconnected(int msecs = 30 * BeQt::Second);
    void installReplyHandler(const QString &operation, InternalHandler handler);
    void installReplyHandler(const QString &operation, ExternalHandler handler);
    void installRequestHandler(const QString &operation, InternalHandler handler);
    void installRequestHandler(const QString &operation, ExternalHandler handler);
    bool isValid() const;
    bool isConnected() const;
    const QUuid uniqueId() const;
    BNetworkServer *server() const;
    QAbstractSocket::SocketError error() const;
    QString errorString() const;
    int compressionLevel() const;
    qint64 criticalBufferSize() const;
    bool closeOnCriticalBufferSize() const;
    bool detailedLogMode() const;
    bool autoDeleteSentReplies() const;
    BLogger *logger() const;
    QString peerAddress() const;
    BNetworkOperation *sendRequest(const QString &op, const QByteArray &data = QByteArray());
    BNetworkOperation *sendRequest(const QString &op, const QVariant &variant);
    bool sendReply(BNetworkOperation *op, const QByteArray &data);
    bool sendReply(BNetworkOperation *op, const QVariant &variant);
public slots:
    void disconnectFromHost();
    void close();
    void abort();
protected:
    virtual void handleReply(BNetworkOperation *op);
    virtual void handleRequest(BNetworkOperation *op);
    virtual void log(const QString &text, BLogger::Level lvl = BLogger::NoLevel);
    BGenericSocket *socket() const;
    BSocketWrapper *socketWrapper() const;
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
private:
    Q_DISABLE_COPY(BNetworkConnection)
};

#endif // BNETWORKCONNECTON_H
