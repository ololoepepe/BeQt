#include "beditgroup.h"
#include "beditgroup_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QMetaObject>
#include <QLineEdit>

/*============================================================================
================================ BEditGroupPrivate ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BEditGroupPrivate::BEditGroupPrivate(BEditGroup *q) :
    BBasePrivate(q)
{
    //
}

BEditGroupPrivate::~BEditGroupPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BEditGroupPrivate::init()
{
    match = false;
}

/*============================== Public slots ==============================*/

void BEditGroupPrivate::ledtDestroyed(QObject *object)
{
    ledtMap.remove(object);
    textChanged();
}

void BEditGroupPrivate::textChanged()
{
    QList<QLineEdit *> list = ledtMap.values();
    bool nmatch = !list.isEmpty();
    if (nmatch)
    {
        QString txt = list.first()->text();
        foreach (int i, bRangeD(1, list.size() - 1))
        {
            if (list.at(i)->text() != txt)
            {
                nmatch = false;
                break;
            }
        }
    }
    bool b = nmatch != match;
    match = nmatch;
    if (b)
        QMetaObject::invokeMethod(q_func(), "textsMatchChanged", Q_ARG(bool, match));
}

/*============================================================================
================================ BEditGroup ==================================
============================================================================*/

/*============================== Public constructors =======================*/

BEditGroup::BEditGroup(QObject *parent) :
    QObject(parent), BBase(*new BEditGroupPrivate(this))
{
    d_func()->init();
}

BEditGroup::~BEditGroup()
{
    //
}

/*============================== Protected constructors ====================*/

BEditGroup::BEditGroup(BEditGroupPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BEditGroup::addEdit(QLineEdit *ledt)
{
    if (!ledt || d_func()->ledtMap.contains(ledt))
        return;
    d_func()->ledtMap.insert(ledt, ledt);
    connect(ledt, SIGNAL(textChanged()), d_func(), SLOT(textChanged()));
    connect(ledt, SIGNAL(destroyed(QObject *)), d_func(), SLOT(ledtDestroyed(QObject *)));
    d_func()->textChanged();
}

bool BEditGroup::textsMatch() const
{
    return d_func()->match;
}

QString BEditGroup::text() const
{
    return d_func()->match ? d_func()->ledtMap.values().first()->text() : QString();
}
