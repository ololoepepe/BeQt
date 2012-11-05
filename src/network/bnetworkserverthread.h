#ifndef BNETWORKSERVERTHREAD_H
#define BNETWORKSERVERTHREAD_H

class BNetworkServerThreadPrivate;
class BNetworkServerPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase.h>

#include <QThread>

class BNetworkServerThread : public QThread, public BBase
{
    B_DECLARE_PRIVATE(BNetworkServerThread)
    Q_OBJECT
public:
    explicit BNetworkServerThread(BNetworkServerPrivate *serverPrivate);
    //
    void addConnection(int socketDescriptor);
    int connectionCount() const;
protected:
    BNetworkServerThread(BNetworkServerThreadPrivate &d);
private:
    Q_DISABLE_COPY(BNetworkServerThread)
};

#endif // BNETWORKSERVERTHREAD_H
