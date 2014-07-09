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

#ifndef BPLUGININTERFACE_H
#define BPLUGININTERFACE_H

class BVersion;

#include "bglobal.h"
#include "bpersoninfolist.h"

#include <QString>
#include <QtPlugin>

/*============================================================================
================================ BPluginInterface ============================
============================================================================*/

class B_CORE_EXPORT BPluginInterface
{
public:
    struct PluginInfo
    {
        QString authorsFileName;
        QString changeLogFileName;
        QString copyrightYears;
        QString descriptionFileName;
        QString licenseFileName;
        QString organization;
        QString thanksToFileName;
        QString translatorsFileName;
        QString website;
    };
    struct StaticPluginInfo
    {
        BPersonInfoList authors;
        QString changeLog;
        QString copyrightYears;
        QString description;
        QString license;
        QString organization;
        BPersonInfoList thanksTo;
        BPersonInfoList translators;
        QString website;
    };
public:
    virtual ~BPluginInterface() {}
public:
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual PluginInfo info() const = 0;
    virtual QString name() const = 0;
    virtual bool prefereStaticInfo() const = 0;
    virtual StaticPluginInfo staticInfo() const = 0;
    virtual QString type() const = 0;
    virtual BVersion version() const = 0;
};

Q_DECLARE_INTERFACE(BPluginInterface, "BeQt.BPluginInterface")

#endif // BPLUGININTERFACE_H
