#ifndef BNETWORKSERVERWORKER_H
#define BNETWORKSERVERWORKER_H

class BNetworkServerWorkerPrivate;
class BNetworkServerPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

class BNetworkServerWorker : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BNetworkServerWorker)
    Q_OBJECT
public:
    explicit BNetworkServerWorker(BNetworkServerPrivate *serverPrivate);
    ~BNetworkServerWorker();
    //
    int connectionCount() const;
public slots:
    void addConnection(int socketDescriptor);
signals:
    void ranOutOfConnections();
protected:
    BNetworkServerWorker(BNetworkServerWorkerPrivate &d);
private:
    Q_DISABLE_COPY(BNetworkServerWorker)
};

#endif // BNETWORKSERVERWORKER_H
