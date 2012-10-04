#ifndef BNETWORKSERVERTHREAD_H
#define BNETWORKSERVERTHREAD_H

class BNetworkServerWorker;

#include <QThread>
#include <QPointer>

class BNetworkServerThread : public QThread
{
    Q_OBJECT
public:
    explicit BNetworkServerThread(BNetworkServerWorker *worker, QObject *parent = 0);
    //
    void addConnection(int socketDescriptor);
    int connectionCount() const;
    bool isValid() const;
private:
    QPointer<BNetworkServerWorker> _m_worker;
};

#endif // BNETWORKSERVERTHREAD_H
