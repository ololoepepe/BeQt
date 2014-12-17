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

#include "blocationprovider.h"

#include "bapplicationbase.h"
#include "bbase_p.h"
#include "bdirtools.h"

#include <QDebug>
#include <QDir>
#include <QMap>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BLocationProviderPrivate ====================
============================================================================*/

class BLocationProviderPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocationProvider)
public:
    bool autoCreate;
    BLocationProvider::LocationMap locations;
public:
    explicit BLocationProviderPrivate(BLocationProvider *q);
    ~BLocationProviderPrivate();
public:
    void init();
private:
    Q_DISABLE_COPY(BLocationProviderPrivate)
};

/*============================================================================
================================ BLocationProviderPrivate ====================
============================================================================*/

/*============================== Public constructors =======================*/

BLocationProviderPrivate::BLocationProviderPrivate(BLocationProvider *q) :
    BBasePrivate(q)
{
    //
}

BLocationProviderPrivate::~BLocationProviderPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BLocationProviderPrivate::init()
{
    autoCreate = true;
}

/*============================================================================
================================ BLocationProvider ===========================
============================================================================*/

/*!
\class BLocationProvider
\inmodule BeQtCore
\brief The BLocationProvider class provides a simple location provider, which is used to provide information about
application resources.

This class is an implementation of BAbstractLocationProvider. It adds methods which are used to set mepping between
resources identifiers and paths to that resources.

Example:

\snippet src/corelib/blocationprovider.cpp 0

\sa BAbstractLocationProvider
*/

/*!
\typedef BLocationProvider::PathMap

The BLocationProvider::PathMap typedef provides QString map with BApplicationBase::ResourceType keys for
BLocationProvider.
*/

/*!
\typedef BLocationProvider::LocationMap

The BLocationProvider::LocationMap typedef provides BLocationProvider::PathMap map with QString keys for
BLocationProvider.
*/

/*============================== Public constructors =======================*/

/*!
Constructs a location provider and sets it's location mapping to \a locations.
*/

BLocationProvider::BLocationProvider(const LocationMap &locations) :
    BAbstractLocationProvider(), BBase(*new BLocationProviderPrivate(this))
{
    d_func()->init();
    d_func()->locations = locations;
}

/*!
Constructs a copy of \a other.
*/

BLocationProvider::BLocationProvider(const BLocationProvider &other) :
    BAbstractLocationProvider(), BBase(*new BLocationProviderPrivate(this))
{
    *this = other;
}

/*!
Destroys the location provider.
*/

BLocationProvider::~BLocationProvider()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs a location provider and associates the given data object \a d with it.
*/

BLocationProvider::BLocationProvider(BLocationProviderPrivate &d) :
    BBase(d)
{
    //
}

/*============================== Public methods ============================*/

/*!
Adds \a paths to the location provider map for the resource identifier \a name.

If the mapping already exists, it is replaced by the new one.

\sa addLocationPath()
*/

void BLocationProvider::addLocation(const QString &name, const PathMap &paths)
{
    if (name.isEmpty())
        return;
    if (d_func()->locations.contains(name))
        return;
    d_func()->locations.insert(name, paths);
}

/*!
\overload
Adds paths deducted form the \a subdirName to the location provider map for the resource identifier \a name.

Paths are deducting by appending \a subdirName to all basic resources paths. See \l {Resources system} for details.

If \a subdirName is empty, \a name is used as a subdirectory name.

If the mapping already exists, it is replaced by the new one.

\sa addLocationPath()
*/

void BLocationProvider::addLocation(const QString &name, const QString &subdirName)
{
    typedef QList<BApplicationBase::ResourceType> ResTypeList;
    init_once(ResTypeList, resTypes, ResTypeList()) {
        resTypes << BApplicationBase::BuiltinResource;
        resTypes << BApplicationBase::SharedResource;
        resTypes << BApplicationBase::UserResource;
    }
    if (name.isEmpty())
        return;
    if (d_func()->locations.contains(name))
        return;
    QString s = !subdirName.isEmpty() ? subdirName : name;
    PathMap m;
    foreach (BApplicationBase::ResourceType r, resTypes) {
        QString l = BApplicationBase::location(BApplicationBase::DataPath, r);
        if (l.isEmpty())
            continue;
        m.insert(r, l + "/" + s);
    }
    d_func()->locations.insert(name, m);
}

/*!
\overload
Adds \a path for the resource type \a type to the location provider map for the resource identifier \a locationName.

If the mapping already exists, it is replaced by the new one.

\sa addLocation()
*/

void BLocationProvider::addLocationPath(const QString &locationName, BApplicationBase::ResourceType type,
                                        const QString &path)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    PathMap &m = d_func()->locations[locationName];
    if (m.contains(type))
        return;
    m.insert(type, path);
}

/*!
This function simply calls addLocation() for each name in \a names.

\sa addLocation()
*/

void BLocationProvider::addLocations(const QStringList &names)
{
    foreach (const QString &name, names)
        addLocation(name);
}

/*!
Returns true if automatic path creation is enabled, otherwise returns false.

\sa setAutoCreatePaths()
*/

bool BLocationProvider::autoCreatePaths() const
{
    return d_func()->autoCreate;
}

/*!
\reimp
*/

bool BLocationProvider::canCreateLocationPath(const QString &, BApplicationBase::ResourceType) const
{
    return d_func()->autoCreate;
}

/*!
\reimp
*/

bool BLocationProvider::createLocationPath(const QString &locationName, BApplicationBase::ResourceType type)
{
    if (locationName.isEmpty())
        return false;
    if (!d_func()->autoCreate)
        return false;
    if (!d_func()->locations.contains(locationName))
        return false;
    const PathMap &m = d_func()->locations[locationName];
    if (!m.contains(type))
        return false;
    const QString &s = m.value(type);
    return !s.isEmpty() && BDirTools::mkpath(s);
}

/*!
Returns true if this location provider has no resource path mappings. Otherwise returns false.
*/

bool BLocationProvider::isEmpty() const
{
    return d_func()->locations.isEmpty();
}

/*!
\reimp
*/

QStringList BLocationProvider::locationNames() const
{
    return d_func()->locations.keys();
}

/*!
\reimp
*/

QString BLocationProvider::locationPath(const QString &locationName, BApplicationBase::ResourceType type) const
{
    if (locationName.isEmpty())
        return QString();
    if (!d_func()->locations.contains(locationName))
        return QString();
    const PathMap &m = d_func()->locations[locationName];
    if (!m.contains(type))
        return QString();
    return m.value(type);
}

/*!
Removes resource path mapping with identifier \a name form the internal map.
*/

void BLocationProvider::removeLocation(const QString &name)
{
    if (name.isEmpty())
        return;
    d_func()->locations.remove(name);
}

/*!
Removes resource path mapping with identifier \a locationName for resource type \a type form the internal map.
*/

void BLocationProvider::removeLocationPath(const QString &locationName, BApplicationBase::ResourceType type)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    d_func()->locations[locationName].remove(type);
}

/*!
Enables automatic path creation if \a enabled is true, otherwise disables it.

\sa autoCreatePaths()
*/

void BLocationProvider::setAutoCreatePaths(bool enabled)
{
    d_func()->autoCreate = enabled;
}

/*!
Sets \a path for the resource type \a type of the location provider map for the existing resource identifier
\a locationName.

If the mapping does not exist, nothing is done.

\sa addLocationPath(), setLocationPaths(), setLocations()
*/

void BLocationProvider::setLocationPath(const QString &locationName, BApplicationBase::ResourceType type,
                                        const QString &path)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    d_func()->locations[locationName][type] = path;
}

/*!
Sets mapping of the location provider map for the existing resource identifier \a locationName to \a paths.

If the mapping does not exist, nothing is done.

\sa addLocation(), setLocationPath(), setLocations()
*/

void BLocationProvider::setLocationPaths(const QString &locationName, const PathMap &paths)
{
    if (locationName.isEmpty())
        return;
    d_func()->locations[locationName] = paths;
}

/*!
Sets the location provider's mapping to \a locations.
*/

void BLocationProvider::setLocations(const LocationMap &locations)
{
    d_func()->locations = locations;
}

/*============================== Public operators ==========================*/

/*!
Assigns \a other to this location provider and returns a reference to this location provider.
*/

BLocationProvider &BLocationProvider::operator =(const BLocationProvider &other)
{
    d_func()->locations = other.d_func()->locations;
    d_func()->autoCreate = other.d_func()->autoCreate;
    return *this;
}

/*!
Returns true if this location provider is equal to location provider \a other; otherwise returns false.

The location providers are equal if they have exactly the same mappings.
*/

bool BLocationProvider::operator ==(const BLocationProvider &other) const
{
    return d_func()->autoCreate == other.d_func()->autoCreate && d_func()->locations == other.d_func()->locations;
}

/*!
Returns true if this location provider is not equal to location provider \a other; otherwise returns false.

The location providers are equal if they have exactly the same mappings; otherwise they are not equal.
*/

bool BLocationProvider::operator !=(const BLocationProvider &other) const
{
    return !(*this == other);
}
