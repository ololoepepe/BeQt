#ifndef BNETWORKSERVER_H
#define BNETWORKSERVER_H

class BNetworkServerThread;
class BNetworkServerWorker;
class BNetworkConnection;
class BNetworkServerPrivate;

#include <BeQt>

#include "bgenericserver.h"

#include <QObject>
#include <QList>
#include <QPointer>
#include <QString>

class B_NETWORK_EXPORT BNetworkServer : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkServer)
public:
    explicit BNetworkServer(BGenericServer::ServerType type, QObject *parent = 0);
    ~BNetworkServer();
    //
    void setMaxConnectionCount(int count);
    void setMaxThreadCount(int count);
    bool isValid() const;
    bool isListening() const;
    bool listen(const QString &address, quint16 port = 0);
    void close();
    BGenericServer::ServerType serverType() const;
    int maxConnectionCount() const;
    int currentConnectionCount() const;
    int maxThreadCount() const;
    int currentThreadCount() const;
protected:
    virtual BNetworkConnection *createConnection(int socketDescriptor);
private:
    BNetworkServerPrivate *_m_d;
    QPointer<BGenericServer> _m_server;
    QList<BNetworkServerThread *> _m_threads;
    int _m_maxConnectionCount;
    int _m_maxThreadCount;
private slots:
    void _m_newConnection(int socketDescriptor);
    void _m_finished();
};

#endif // BNETWORKSERVER_H
