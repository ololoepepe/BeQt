#ifndef BTCPSERVER_H
#define BTCPSERVER_H

#include <QTcpServer>

class BTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit BTcpServer(QObject *parent = 0);
signals:
    void newConnection(int socketDescriptor);
protected:
    void incomingConnection(int handle);
};

#endif // BTCPSERVER_H
