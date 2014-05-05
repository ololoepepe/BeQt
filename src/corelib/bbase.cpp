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

#include "bbase.h"
#include "bbase_p.h"

/*============================================================================
================================ BBasePrivate ================================
============================================================================*/

/*============================== Public constructors =======================*/

BBasePrivate::BBasePrivate(BBase *q) :
    _m_q(q)
{
    //
}

BBasePrivate::~BBasePrivate()
{
    //
}

/*============================== Public methods ============================*/

void BBasePrivate::init()
{
    //
}

/*============================================================================
================================ BBase =======================================
============================================================================*/

/*!
    \class BBase
    \brief The BBase class is the base class of all BeQt classes implementing
    D-pointer approach.
*/

/*============================== Public constructors =======================*/

/*!
    Constructs an object and the associated data object.
*/

BBase::BBase() :
    _m_d(new BBasePrivate(this))
{
    _m_d->init();
}

/*!
    Destroys the object, deleting the associated data object.
*/

BBase::~BBase()
{
    delete _m_d;
}

/*============================== Protected constructors ====================*/

/*!
    \overload BBase()

    Constructs an object and associates the given data object \a d with it.
 */

BBase::BBase(BBasePrivate &d) :
    _m_d(&d)
{
    _m_d->init();
}
