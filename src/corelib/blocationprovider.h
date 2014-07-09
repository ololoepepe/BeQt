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

class QStringList;

#include "babstractlocationprovider.h"
#include "bapplicationbase.h"
#include "bbase.h"

#include <QMap>
#include <QString>

/*============================================================================
================================ BLocationProvider ===========================
============================================================================*/

class B_CORE_EXPORT BLocationProvider : public BAbstractLocationProvider, public BBase
{
    B_DECLARE_PRIVATE(BLocationProvider)
public:
    typedef QMap<BApplicationBase::ResourceType, QString> PathMap;
    typedef QMap<QString, PathMap> LocationMap;
public:
    explicit BLocationProvider(const LocationMap &locations = LocationMap());
    BLocationProvider(const BLocationProvider &other);
    ~BLocationProvider();
protected:
    explicit BLocationProvider(BLocationProviderPrivate &d);
public:
    void addLocation(const QString &name, const PathMap &paths);
    void addLocation(const QString &name, const QString &subdirName = QString());
    void addLocationPath(const QString &locationName, BApplicationBase::ResourceType type, const QString &path);
    bool autoCreatePaths() const;
    bool canCreateLocationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
    bool createLocationPath(const QString &locationName, BApplicationBase::ResourceType type);
    bool isEmpty() const;
    QStringList locationNames() const;
    QString locationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
    void removeLocation(const QString &name);
    void removeLocationPath(const QString &locationName, BApplicationBase::ResourceType type);
    void setAutoCreatePaths(bool enabled);
    void setLocationPath(const QString &locationName, BApplicationBase::ResourceType type, const QString &path);
    void setLocationPaths(const QString &locationName, const PathMap &paths);
    void setLocations(const LocationMap &locations);
public:
    BLocationProvider &operator =(const BLocationProvider &other);
    bool operator ==(const BLocationProvider &other) const;
    bool operator !=(const BLocationProvider &other) const;
};

#endif // BABSTRACTLOCATIONPROVIDER_H
