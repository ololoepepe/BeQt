#include "btcpserver.h"

#include <QTcpServer>

BTcpServer::BTcpServer(QObject *parent) :
    QTcpServer(parent)
{
}

//

void BTcpServer::incomingConnection(int handle)
{
    emit newConnection(handle);
}
