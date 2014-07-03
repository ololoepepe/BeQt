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

#ifndef BLOCATIONPROVIDER_H
#define BLOCATIONPROVIDER_H

class BLocationProviderPrivate;

#include "bglobal.h"
#include "bbase.h"
#include "bapplicationbase.h"
#include "babstractlocationprovider.h"

#include <QString>
#include <QStringList>
#include <QMap>

/*============================================================================
================================ BLocationProvider ===========================
============================================================================*/

class B_CORE_EXPORT BLocationProvider : public BAbstractLocationProvider, public BBase
{
    B_DECLARE_PRIVATE(BLocationProvider)
public:
    typedef QMap<BApplicationBase::ResourcesType, QStringList> PathsMap;
    typedef QMap<QString, PathsMap> LocationMap;
public:
    explicit BLocationProvider(const LocationMap &locations = LocationMap());
    BLocationProvider(const BLocationProvider &other);
    ~BLocationProvider();
protected:
    BLocationProvider(BLocationProviderPrivate &d);
public:
    void setLocations(const LocationMap &locations);
    void addLocation(const QString &name, const PathsMap &paths);
    void removeLocation(const QString &name);
    void setLocationPaths(const QString &locationName, const PathsMap &paths);
    void addLocationPaths(const QString &locationName, BApplicationBase::ResourcesType type, const QStringList &paths);
    void removeLocationPaths(const QString &locationName, BApplicationBase::ResourcesType type);
    void setLocationPaths(const QString &locationName, BApplicationBase::ResourcesType type, const QStringList &paths);
    QStringList locationNames() const;
    QStringList locationPaths(const QString &locationName, BApplicationBase::ResourcesType type) const;
    bool isEmpty() const;
public:
    BLocationProvider &operator=(const BLocationProvider &other);
    bool operator==(const BLocationProvider &other) const;
    bool operator!=(const BLocationProvider &other) const;
};

#endif // BABSTRACTLOCATIONPROVIDER_H
