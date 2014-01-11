#ifndef BEDITGROUP_P_H
#define BEDITGROUP_P_H

class QLineEdit;

#include "beditgroup.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>

/*============================================================================
================================ BEditGroupPrivate ===========================
============================================================================*/

class B_WIDGETS_EXPORT BEditGroupPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BEditGroup)
public:
    explicit BEditGroupPrivate(BEditGroup *q);
    ~BEditGroupPrivate();
public:
    void init();
public Q_SLOTS:
    void ledtDestroyed(QObject *object);
    void textChanged();
public:
    QMap<QObject *, QLineEdit *> ledtMap;
    bool match;
private:
    Q_DISABLE_COPY(BEditGroupPrivate)
};

#endif // BEDITGROUP_P_H
