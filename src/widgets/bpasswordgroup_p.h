#ifndef BPASSWORDGROUP_P_H
#define BPASSWORDGROUP_P_H

class BPaswordWidget;

#include "bpasswordgroup.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>

/*============================================================================
================================ BPasswordGroupPrivate =======================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordGroupPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPasswordGroup)
public:
    explicit BPasswordGroupPrivate(BPasswordGroup *q);
    ~BPasswordGroupPrivate();
public:
    void init();
public Q_SLOTS:
    void pwdwgtDestroyed(QObject *object);
    void passwordChanged();
public:
    QMap<QObject *, BPasswordWidget *> wgtMap;
    bool match;
private:
    Q_DISABLE_COPY(BPasswordGroupPrivate)
};

#endif // BPASSWORDGROUP_P_H
