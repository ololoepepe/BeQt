#ifndef BTCPSERVER_H
#define BTCPSERVER_H

#include <QTcpServer>

class BTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit BTcpServer(QObject *parent = 0);
protected:
    void incomingConnection(int handle);
signals:
    void newConnection(int socketDescriptor);
};

#endif // BTCPSERVER_H
