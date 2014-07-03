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

/*============================================================================
================================ BLocationProvider ===========================
============================================================================*/

class B_CORE_EXPORT BLocationProvider : public BAbstractLocationProvider, public BBase
{
    B_DECLARE_PRIVATE(BLocationProvider)
public:
    explicit BLocationProvider(const QString &locationName = QString());
    BLocationProvider(const BLocationProvider &other);
    ~BLocationProvider();
protected:
    BLocationProvider(BLocationProviderPrivate &d);
public:
    void setLocationName(const QString &name);
    void setLocationPath(BApplicationBase::ResourcesType type, const QString &path);
    QString locationName() const;
    QString locationPath(BApplicationBase::ResourcesType type) const;
    bool isValid() const;
public:
    BLocationProvider &operator=(const BLocationProvider &other);
    bool operator==(const BLocationProvider &other) const;
    bool operator!=(const BLocationProvider &other) const;
};

#endif // BABSTRACTLOCATIONPROVIDER_H
