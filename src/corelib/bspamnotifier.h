#ifndef BSPAMNOTIFIER_H
#define BSPAMNOTIFIER_H

class BSpamNotifierPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>

/*============================================================================
================================ BSpamNotifier ===============================
============================================================================*/

class BSpamNotifier : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BSpamNotifier)
public:
    explicit BSpamNotifier(QObject *parent = 0);
    explicit BSpamNotifier(QObject *parent, int checkInterval, int eventLimit);
    ~BSpamNotifier();
protected:
    explicit BSpamNotifier(BSpamNotifierPrivate &d, QObject *parent = 0);
public:
    void setCheckInterval(int msecs);
    void setEventLimit(int count);
    int checkInterval() const;
    int eventLimit() const;
    int eventCount() const;
    int timeElapsed() const;
    bool isActive() const;
public slots:
    void spam(int eventWeight = 0);
signals:
    void spammed(int msecsElapsed);
private:
    Q_DISABLE_COPY(BSpamNotifier)
};

#endif // BSPAMNOTIFIER_H
