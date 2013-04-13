#ifndef BSPAMNOTIFIER_P_H
#define BSPAMNOTIFIER_P_H

class QTimer;

#include "bspamnotifier.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
class QElapsedTimer;
#endif

/*============================================================================
================================ BSpamNotifierPrivate
============================================================================*/

class B_CORE_EXPORT BSpamNotifierPrivate : public BBasePrivate
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QElapsedTimer *etimer;
#endif
    int interval;
    int limit;
    int count;
    int elapsed;
    bool enabled;
private:
    Q_DISABLE_COPY(BSpamNotifierPrivate)
};

#endif // BSPAMNOTIFIER_P_H
