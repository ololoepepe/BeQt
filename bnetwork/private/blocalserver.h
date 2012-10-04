#ifndef BLOCALSERVER_H
#define BLOCALSERVER_H

#include <QLocalServer>

class BLocalServer : public QLocalServer
{
    Q_OBJECT
public:
    explicit BLocalServer(QObject *parent = 0);
protected:
    void incomingConnection(quintptr socketDescriptor);
signals:
    void newConnection(int socketDescriptor);
};

#endif // BLOCALSERVER_H
