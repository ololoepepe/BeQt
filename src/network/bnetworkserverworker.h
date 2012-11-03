#ifndef BNETWORKSERVERWORKER_H
#define BNETWORKSERVERWORKER_H

class BNetworkConnection;
class BNetworkServerPrivate;

#include <BeQtCore/BeQt>

#include <QObject>
#include <QList>
#include <QMutex>

class BNetworkServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *serverPrivate, QObject *parent = 0);
    //
    int connectionCount() const;
public slots:
    void addConnection(int socketDescriptor);
private:
    BNetworkServerPrivate *_m_ServerPrivate;
    //
    mutable QMutex _m_connectionsMutex;
    QList<BNetworkConnection *> _m_connections;
private slots:
    void _m_disconnected();
signals:
    void ranOutOfConnections();
};

#endif // BNETWORKSERVERWORKER_H
