#include "blocalserver.h"

BLocalServer::BLocalServer(QObject *parent) :
    QLocalServer(parent)
{
}

//

void BLocalServer::incomingConnection(quintptr socketDescriptor)
{
    emit newConnection(socketDescriptor);
}
