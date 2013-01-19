#ifndef BSPAMNOTIFIER_P_H
#define BSPAMNOTIFIER_P_H

class QTimer;

#include "bspamnotifier.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BSpamNotifierPrivate
============================================================================*/

class BSpamNotifierPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BSpamNotifier)
public:
    explicit BSpamNotifierPrivate(BSpamNotifier *q);
    ~BSpamNotifierPrivate();
public:
    void init();
    void testSpam(int dcount = 1);
    int timeElapsed() const;
public slots:
    void timeout();
public:
    QTimer *timer;
    int interval;
    int limit;
    int count;
    int elapsed;
private:
    Q_DISABLE_COPY(BSpamNotifierPrivate)
};

#endif // BSPAMNOTIFIER_P_H
