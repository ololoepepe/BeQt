/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bpasswordgroup.h"
#include "bpasswordgroup_p.h"
#include "bpasswordwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtCore/BPassword>

#include <QObject>
#include <QMap>
#include <QList>
#include <QMetaObject>

#include <QDebug>

/*============================================================================
================================ BPasswordGroupPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BPasswordGroupPrivate::BPasswordGroupPrivate(BPasswordGroup *q) :
    BBaseObjectPrivate(q)
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
    QObject(parent), BBaseObject(*new BPasswordGroupPrivate(this))
{
    d_func()->init();
}

BPasswordGroup::~BPasswordGroup()
{
    //
}

/*============================== Protected constructors ====================*/

BPasswordGroup::BPasswordGroup(BPasswordGroupPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
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
