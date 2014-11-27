/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
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

#include "bbaseobject.h"
#include "bbaseobject_p.h"

#include <QObject>

/*============================================================================
================================ BBaseObjectPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BBaseObjectPrivate::BBaseObjectPrivate(BBaseObject *q) :
    QObject(), _m_q(q)
{
    //
}

BBaseObjectPrivate::~BBaseObjectPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BBaseObjectPrivate::init()
{
    //
}

/*============================================================================
================================ BBaseObject =================================
============================================================================*/

/*!
    \class BBaseObject
    \brief The BBaseObject class is the base class of all BeQt classes
    implementing D-pointer approach.

    The corresponding BBaseObjectPrivate class is inherited from the QObject
    class, so you may make use of Qt signals and slots in it's successor. If
    you do not need Qt signals and slots, use BBase class instead.

    \note BBaseObject itself is \e not inherited from QObject. Only
    BBaseObjectPrivate is. You have to use multiple inheritance if you wish
    BBaseObject successor to inherit from QObject. You must declare QObject
    before any other ancestors in that case.

    You will need BeQt private headers to use BBaseObjectPrivate class:

    \snippet src/corelib/bbaseobject.cpp 0

    \sa BBase
*/

/*============================== Public constructors =======================*/

/*!
    Constructs an object and the associated data object.
*/

BBaseObject::BBaseObject() :
    _m_d(new BBaseObjectPrivate(this))
{
    _m_d->init();
}

/*!
    Destroys the object, deleting the associated data object.
*/

BBaseObject::~BBaseObject()
{
    delete _m_d;
}

/*============================== Protected constructors ====================*/

/*!
    Constructs an object and associates the given data object \a d with it.
 */

BBaseObject::BBaseObject(BBaseObjectPrivate &d) :
    _m_d(&d)
{
    _m_d->init();
}
