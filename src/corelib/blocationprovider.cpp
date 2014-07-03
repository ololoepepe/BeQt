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

#include "bbase_p.h"
#include "bapplicationbase.h"

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
    explicit BLocationProviderPrivate(BLocationProvider *q);
    ~BLocationProviderPrivate();
public:
    void init();
public:
    BLocationProvider::LocationMap locations;
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
    //
}

/*============================================================================
================================ BLocationProvider ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BLocationProvider::BLocationProvider(const LocationMap &locations) :
    BAbstractLocationProvider(), BBase(*new BLocationProviderPrivate(this))
{
    d_func()->init();
    d_func()->locations = locations;
}

BLocationProvider::BLocationProvider(const BLocationProvider &other) :
    BAbstractLocationProvider(), BBase(*new BLocationProviderPrivate(this))
{
    *this = other;
}

BLocationProvider::~BLocationProvider()
{
    //
}

/*============================== Protected constructors ====================*/

BLocationProvider::BLocationProvider(BLocationProviderPrivate &d) :
    BBase(d)
{
    //
}

/*============================== Public methods ============================*/

void BLocationProvider::setLocations(const LocationMap &locations)
{
    d_func()->locations = locations;
}

void BLocationProvider::addLocation(const QString &name, const PathsMap &paths)
{
    if (name.isEmpty())
        return;
    if (d_func()->locations.contains(name))
        return;
    d_func()->locations.insert(name, paths);
}

void BLocationProvider::removeLocation(const QString &name)
{
    if (name.isEmpty())
        return;
    d_func()->locations.remove(name);
}

void BLocationProvider::setLocationPaths(const QString &locationName, const PathsMap &paths)
{
    if (locationName.isEmpty())
        return;
    d_func()->locations[locationName] = paths;
}

void BLocationProvider::addLocationPaths(const QString &locationName, BApplicationBase::ResourcesType type,
                                         const QStringList &paths)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    PathsMap &m = d_func()->locations[locationName];
    if (m.contains(type))
        return;
    m.insert(type, paths);
}

void BLocationProvider::removeLocationPaths(const QString &locationName, BApplicationBase::ResourcesType type)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    d_func()->locations[locationName].remove(type);
}

void BLocationProvider::setLocationPaths(const QString &locationName, BApplicationBase::ResourcesType type,
                                         const QStringList &paths)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    d_func()->locations[locationName][type] = paths;
}

bool BLocationProvider::isEmpty() const
{
    return d_func()->locations.isEmpty();
}

/*============================== Public operators ==========================*/

BLocationProvider &BLocationProvider::operator=(const BLocationProvider &other)
{
    d_func()->locations = other.d_func()->locations;
    return *this;
}

bool BLocationProvider::operator==(const BLocationProvider &other) const
{
    return d_func()->locations == other.d_func()->locations;
}

bool BLocationProvider::operator!=(const BLocationProvider &other) const
{
    return !(*this == other);
}
