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

#include "babstractlocationprovider.h"

/*============================================================================
================================ BAbstractLocationProvider ===================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractLocationProvider::BAbstractLocationProvider()
{
    //
}

BAbstractLocationProvider::~BAbstractLocationProvider()
{
    //
}

/*============================== Public methods ============================*/

bool BAbstractLocationProvider::canCreateLocationPath(const QString &, BApplicationBase::ResourceType) const
{
    return false;
}

bool BAbstractLocationProvider::createLocationPath(const QString &, BApplicationBase::ResourceType)
{
    return false;
}