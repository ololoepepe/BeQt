#ifndef BNETWORKSERVER_H
#define BNETWORKSERVER_H

class BNetworkServerPrivate;
class BNetworkConnection;

class QString;

#include "bgenericserver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ BNetworkServer ==============================
============================================================================*/

class B_NETWORK_EXPORT BNetworkServer : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BNetworkServer)
public:
    explicit BNetworkServer(BGenericServer::ServerType type, QObject *parent = 0);
    ~BNetworkServer();
protected:
    explicit BNetworkServer(BNetworkServerPrivate &d, QObject *parent = 0);
public:
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
    virtual BNetworkConnection *createConnection(int socketDescriptor) const = 0;
private:
    Q_DISABLE_COPY(BNetworkServer)
};

#endif // BNETWORKSERVER_H
