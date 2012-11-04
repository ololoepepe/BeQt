#ifndef BLOCALSERVER_H
#define BLOCALSERVER_H

#include <QLocalServer>

class BLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit BLocalServer(QObject *parent = 0);
signals:
    void newConnection(int socketDescriptor);
protected:
    void incomingConnection(quintptr socketDescriptor);
};

#endif // BLOCALSERVER_H
