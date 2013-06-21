#ifndef BSIGNALDELAYPROXY_P_H
#define BSIGNALDELAYPROXY_P_H

class QTimer;

#include "bsignaldelayproxy.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QObject>
#include <QVariant>

/*============================================================================
================================ BSignalDelayProxyPrivate ====================
============================================================================*/

class B_CORE_EXPORT BSignalDelayProxyPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSignalDelayProxy)
public:
    explicit BSignalDelayProxyPrivate(BSignalDelayProxy *q);
    ~BSignalDelayProxyPrivate();
public:
    void init();
    void trigger( const QVariant &data = QVariant() );
public Q_SLOTS:
    void timeout();
public:
    QTimer *tmrIntermediate;
    QTimer *tmrMaximum;
    int intermediateDelay;
    int maximumDelay;
    QVariant lastData;
private:
    Q_DISABLE_COPY(BSignalDelayProxyPrivate)
};

#endif //BSIGNALDELAYPROXY_P_H
