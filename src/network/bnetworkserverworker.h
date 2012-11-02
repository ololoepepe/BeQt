#ifndef BNETWORKSERVERWORKER_H
#define BNETWORKSERVERWORKER_H

class BNetworkConnection;

#include <BeQt>

#include <QObject>
#include <QList>
#include <QMutex>

class B_NETWORK_EXPORT BNetworkServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerWorker(QObject *parent = 0);
    //
    int connectionCount() const;
public slots:
    void addConnection(int socketDescriptor);
protected:
    virtual BNetworkConnection *createConnection(int socketDescriptor);
private:
    mutable QMutex _m_connectionsMutex;
    QList<BNetworkConnection *> _m_connections;
private slots:
    void _m_disconnected();
signals:
    void ranOutOfConnections();
};

#endif // BNETWORKSERVERWORKER_H
