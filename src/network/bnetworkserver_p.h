#ifndef BNETWORKSERVER_P_H
#define BNETWORKSERVER_P_H

class BNetworkServer;
class BNetworkConnection;

#include <BeQtCore/BeQt>

class BNetworkServerPrivate
{
    B_DECLARE_PUBLIC(BNetworkServer)
public:
    BNetworkServer *_m_q;
    //
    explicit BNetworkServerPrivate(BNetworkServer *q);
    //
    BNetworkConnection *createConnection(int socketDescriptor);
};

#endif // BNETWORKSERVER_P_H
