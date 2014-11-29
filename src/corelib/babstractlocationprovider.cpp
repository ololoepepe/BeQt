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

/*!
\class BAbstractLocationProvider
\brief The BAbstractLocationProvider class is the abstract base class of location providers, which are used to provide
information about application resources.

This class implements an abstract location provider. Subclasses of this class handle application resources locations,
test their existance and create corresponding directories.

BAbstractLocationProvider instances are registered in BApplicationBase (specifically in BCoreApplication or
BApplication) by calling BApplicationBase::installLocationProvider or BApplicationBase::installLocationProviders. The
application then uses installed location providers to resolve paths to resources requested by
BApplicationBase::location or BApplicationBase::locations call.

Example:

\snippet src/corelib/babstractapplicationprovider.cpp 0

\sa BLocationProvider
*/

/*============================== Public constructors =======================*/

/*!
Constructs an abstract location provider.
*/

BAbstractLocationProvider::BAbstractLocationProvider()
{
    //
}

/*!
Destroys the location provider.
*/

BAbstractLocationProvider::~BAbstractLocationProvider()
{
    //
}

/*============================== Public methods ============================*/

/*!
\fn QStringList BAbstractLocationProvider::locationNames() const

Returns a list of location names (identifiers) supported by this location provider.

Reimplement this function when creating a subclass of BAbstractLocationProvider.

\sa locationPath()
*/

/*!
\fn QString BAbstractLocationProvider::locationPath(const QString &locationName,
                                                    BApplicationBase::ResourceType type) const

Returns the path associated with \a locationName for the resource type \a type.

This function should return empty QString if the specified \a locationName is not supported by this location provider,
or if there is no resource for specified \a type.

Reimplement this function when creating a subclass of BAbstractLocationProvider.

\sa locationNames()
*/

/*!
This virtual function is called by BApplicationBase to determine if the location provider can create path specified by
\a locationName for resource type \a type.

Default implementation returns false.

\sa createLocationPath()
*/

bool BAbstractLocationProvider::canCreateLocationPath(const QString &, BApplicationBase::ResourceType) const
{
    return false;
}

/*!
This virtual function is called by BApplicationBase to create the path specified by \a locationName for resource type
\a type.

Should return true if the location exists or has been successfully created. Otherwise should return false.

Default implementation does nothing and returns false.

\sa canCreateLocationPath()
*/

bool BAbstractLocationProvider::createLocationPath(const QString &, BApplicationBase::ResourceType)
{
    return false;
}
