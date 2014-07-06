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

#ifndef BABSTRACTLOCATIONPROVIDER_H
#define BABSTRACTLOCATIONPROVIDER_H

class QString;
class QStringList;

#include "bapplicationbase.h"

/*============================================================================
================================ BAbstractLocationProvider ===================
============================================================================*/

class B_CORE_EXPORT BAbstractLocationProvider
{
public:
    explicit BAbstractLocationProvider();
    virtual ~BAbstractLocationProvider();
public:
    virtual bool canCreateLocationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
    virtual bool createLocationPath(const QString &locationName, BApplicationBase::ResourceType type);
    virtual QStringList locationNames() const = 0;
    virtual QString locationPath(const QString &locationName, BApplicationBase::ResourceType type) const = 0;
private:
    Q_DISABLE_COPY(BAbstractLocationProvider)
};

#endif // BABSTRACTLOCATIONPROVIDER_H
