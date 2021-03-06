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

#include "beditgroup.h"
#include "beditgroup_p.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QString>

/*============================================================================
================================ BEditGroupPrivate ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BEditGroupPrivate::BEditGroupPrivate(BEditGroup *q) :
    BBaseObjectPrivate(q)
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
    matchAndAcceptable = false;
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
    bool nmatchAndAcceptable = nmatch;
    if (nmatch) {
        QString txt = list.first()->text();
        nmatchAndAcceptable = nmatchAndAcceptable && list.first()->hasAcceptableInput();
        foreach (int i, bRangeD(1, list.size() - 1)) {
            nmatchAndAcceptable = nmatchAndAcceptable && list.at(i)->hasAcceptableInput();
            if (list.at(i)->text() != txt) {
                nmatch = false;
                nmatchAndAcceptable = false;
                break;
            }
        }
    }
    bool b = nmatch != match;
    bool bb = nmatchAndAcceptable != matchAndAcceptable;
    match = nmatch;
    matchAndAcceptable = nmatchAndAcceptable;
    if (b)
        QMetaObject::invokeMethod(q_func(), "textsMatchChanged", Q_ARG(bool, match));
    if (bb)
        QMetaObject::invokeMethod(q_func(), "textsMatchAndAcceptableChanged", Q_ARG(bool, matchAndAcceptable));
}

/*============================================================================
================================ BEditGroup ==================================
============================================================================*/

/*============================== Public constructors =======================*/

BEditGroup::BEditGroup(QObject *parent) :
    QObject(parent), BBaseObject(*new BEditGroupPrivate(this))
{
    d_func()->init();
}

BEditGroup::~BEditGroup()
{
    //
}

/*============================== Protected constructors ====================*/

BEditGroup::BEditGroup(BEditGroupPrivate &d, QObject *parent) :
    QObject(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BEditGroup::addEdit(QLineEdit *ledt)
{
    if (!ledt || d_func()->ledtMap.contains(ledt))
        return;
    d_func()->ledtMap.insert(ledt, ledt);
    connect(ledt, SIGNAL(textChanged(QString)), d_func(), SLOT(textChanged()));
    connect(ledt, SIGNAL(destroyed(QObject *)), d_func(), SLOT(ledtDestroyed(QObject *)));
    d_func()->textChanged();
}

QString BEditGroup::text() const
{
    return d_func()->match ? d_func()->ledtMap.values().first()->text() : QString();
}

bool BEditGroup::textsMatch() const
{
    return d_func()->match;
}

bool BEditGroup::textsMatchAndAcceptable() const
{
    return d_func()->matchAndAcceptable;
}
