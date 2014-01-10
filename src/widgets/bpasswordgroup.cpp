#include "bpasswordgroup.h"
#include "bpasswordgroup_p.h"
#include "bpasswordwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BPassword>

#include <QObject>
#include <QMap>
#include <QList>
#include <QMetaObject>

/*============================================================================
================================ BPasswordGroupPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordGroupPrivate::BPasswordGroupPrivate(BPasswordGroup *q) :
    BBasePrivate(q)
{
    //
}

BPasswordGroupPrivate::~BPasswordGroupPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BPasswordGroupPrivate::init()
{
    match = false;
}

/*============================== Public slots ==============================*/

void BPasswordGroupPrivate::pwdwgtDestroyed(QObject *object)
{
    wgtMap.remove(object);
    passwordChanged();
}

void BPasswordGroupPrivate::passwordChanged()
{
    QList<BPasswordWidget *> list = wgtMap.values();
    bool nmatch = !list.isEmpty();
    if (nmatch)
    {
        BPassword pwd = list.first()->password();
        foreach (int i, bRangeD(1, list.size() - 1))
        {
            if (list.at(i)->password() != pwd)
            {
                nmatch = false;
                break;
            }
        }
    }
    bool b = nmatch != match;
    match = nmatch;
    if (b)
        QMetaObject::invokeMethod(q_func(), "passwordsMatchChanged", Q_ARG(bool, match));
}

/*============================================================================
================================ BPasswordGroup ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordGroup::BPasswordGroup(QObject *parent) :
    QObject(parent), BBase(*new BPasswordGroupPrivate(this))
{
    d_func()->init();
}

BPasswordGroup::~BPasswordGroup()
{
    //
}

/*============================== Protected constructors ====================*/

BPasswordGroup::BPasswordGroup(BPasswordGroupPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BPasswordGroup::addPasswordWidget(BPasswordWidget *pwdwgt)
{
    if (!pwdwgt || d_func()->wgtMap.contains(pwdwgt))
        return;
    d_func()->wgtMap.insert(pwdwgt, pwdwgt);
    connect(pwdwgt, SIGNAL(passwordChanged()), d_func(), SLOT(passwordChanged()));
    connect(pwdwgt, SIGNAL(destroyed(QObject *)), d_func(), SLOT(pwdwgtDestroyed(QObject *)));
    d_func()->passwordChanged();
}

bool BPasswordGroup::passwordsMatch() const
{
    return d_func()->match;
}

BPassword BPasswordGroup::password() const
{
    return d_func()->match ? d_func()->wgtMap.values().first()->password() : BPassword();
}
