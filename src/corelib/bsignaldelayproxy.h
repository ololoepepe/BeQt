#ifndef BSIGNALDELAYPROXY_H
#define BSIGNALDELAYPROXY_H

class BSignalDelayProxyPrivate;

class QString;

#include "bglobal.h"
#include "bnamespace.h"
#include "bbase.h"

#include <QObject>

/*============================================================================
================================ BSignalDelayProxy ===========================
============================================================================*/

class B_CORE_EXPORT BSignalDelayProxy : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSignalDelayProxy)
public:
    explicit BSignalDelayProxy(QObject *parent);
    explicit BSignalDelayProxy(int intermediateDelay = 500, int maximumDelay = BeQt::Second, QObject *parent = 0);
    ~BSignalDelayProxy();
protected:
    explicit BSignalDelayProxy(BSignalDelayProxyPrivate &d, QObject *parent = 0);
public:
    void setIntermediateDelay(int msecs);
    void setMaximumDelay(int msecs);
    int intermediateDelay() const;
    int maximumDelay() const;
    void setConnection(QObject *sender, const char *signal, QObject *receiver, const char *method);
    void setIntConnection(QObject *sender, const char *signal, QObject *receiver, const char *method);
    void setStringConnection(QObject *sender, const char *signal, QObject *receiver, const char *method);
public slots:
    void trigger();
    void trigger(int number);
    void trigger(const QString &string);
signals:
    void triggered();
    void triggered(int number);
    void triggered(const QString &string);
private:
    Q_DISABLE_COPY(BSignalDelayProxy)
};

#endif // BSIGNALDELAYPROXY_H

