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

void BLocationProvider::addLocation(const QString &name, const PathMap &paths)
{
    if (name.isEmpty())
        return;
    if (d_func()->locations.contains(name))
        return;
    d_func()->locations.insert(name, paths);
}

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

bool BLocationProvider::autoCreatePaths() const
{
    return d_func()->autoCreate;
}

bool BLocationProvider::canCreateLocationPath(const QString &, BApplicationBase::ResourceType) const
{
    return d_func()->autoCreate;
}

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

bool BLocationProvider::isEmpty() const
{
    return d_func()->locations.isEmpty();
}

QStringList BLocationProvider::locationNames() const
{
    return d_func()->locations.keys();
}

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

void BLocationProvider::removeLocation(const QString &name)
{
    if (name.isEmpty())
        return;
    d_func()->locations.remove(name);
}

void BLocationProvider::removeLocationPath(const QString &locationName, BApplicationBase::ResourceType type)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    d_func()->locations[locationName].remove(type);
}

void BLocationProvider::setAutoCreatePaths(bool enabled)
{
    d_func()->autoCreate = enabled;
}

void BLocationProvider::setLocationPath(const QString &locationName, BApplicationBase::ResourceType type,
                                        const QString &path)
{
    if (locationName.isEmpty())
        return;
    if (!d_func()->locations.contains(locationName))
        return;
    d_func()->locations[locationName][type] = path;
}

void BLocationProvider::setLocationPaths(const QString &locationName, const PathMap &paths)
{
    if (locationName.isEmpty())
        return;
    d_func()->locations[locationName] = paths;
}

void BLocationProvider::setLocations(const LocationMap &locations)
{
    d_func()->locations = locations;
}

/*============================== Public operators ==========================*/

BLocationProvider &BLocationProvider::operator =(const BLocationProvider &other)
{
    d_func()->locations = other.d_func()->locations;
    d_func()->autoCreate = other.d_func()->autoCreate;
    return *this;
}

bool BLocationProvider::operator ==(const BLocationProvider &other) const
{
    return d_func()->autoCreate == other.d_func()->autoCreate && d_func()->locations == other.d_func()->locations;
}

bool BLocationProvider::operator !=(const BLocationProvider &other) const
{
    return !(*this == other);
}
