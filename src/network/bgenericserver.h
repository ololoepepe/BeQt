#ifndef BGENERICSERVER_H
#define BGENERICSERVER_H

#include "bgenericsocket.h"

#include <BeQt>

#include <QObject>
#include <QTcpServer>
#include <QPointer>
#include <QLocalServer>
#include <QString>
#include <QAbstractSocket>
#include <QQueue>

class B_NETWORK_EXPORT BGenericServer : public QObject
{
    Q_OBJECT
public:
    enum ServerType
    {
        NoServer = 0x00,    //000 0 0 0 0 0
        TcpServer = 0x03,   //000 0 0 0 1 1
        LocalServer = 0x10  //000 1 0 0 0 0
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
protected:
    virtual BGenericSocket *createSocket(int socketDescriptor);
private:
    QPointer<QTcpServer> _m_tserver;
    QPointer<QLocalServer> _m_lserver;
    QQueue<BGenericSocket *> _m_socketQueue;
    int _m_maxPending;
private slots:
    void _m_newConnection(int socketDescriptor);
signals:
    void newConnection(int socketDescriptor);
};

#endif // BGENERICSERVER_H
