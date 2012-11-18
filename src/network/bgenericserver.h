#ifndef BGENERICSERVER_H
#define BGENERICSERVER_H

class BGenericServerPrivate;
class BGenericSocket;

class QTcpServer;
class QLocalServer;
class QString;

#include "bgenericsocket.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QAbstractSocket>

/*============================================================================
================================ Generic Server
============================================================================*/

class B_NETWORK_EXPORT BGenericServer : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BGenericServer)
    Q_OBJECT
public:
    enum ServerType
    {
        NoServer = 0x00,    //0 0 0 0 0 0 0 0
        TcpServer = 0x03,   //0 0 0 0 0 0 1 1
        LocalServer = 0x10  //0 0 0 1 0 0 0 0
    };
    //
    explicit BGenericServer(ServerType type, QObject *parent = 0);
    //
    QTcpServer *tcpServer() const;
    QLocalServer *localServer() const;
    void close();
    QString errorString() const;
    bool hasPendingConnections() const;
    bool isListening() const;
    bool isServerSet() const;
    bool listen(const QString &address, quint16 port = 0);
    int maxPendingConnections() const;
    BGenericSocket *nextPendingConnection();
    QString serverAddress() const;
    QAbstractSocket::SocketError serverError() const;
    ServerType serverType() const;
    void setMaxPendingConnections(int numConnections);
    bool waitForNewConnection(int msec = 0, bool *timedOut = 0);
signals:
    void newPendingConnection();
    void newConnection(int socketDescriptor);
    void connectionOverflow();
protected:
    BGenericServer(BGenericServerPrivate &d, QObject *parent = 0);
    //
    virtual BGenericSocket *createSocket(int socketDescriptor);
private:
    Q_DISABLE_COPY(BGenericServer)
};

#endif // BGENERICSERVER_H
