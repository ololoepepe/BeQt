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

#include "bglobal.h"
#include "bpersoninfo.h"
#include "bdirtools.h"

#include <QString>
#include <QtPlugin>
#include <QList>

/*============================================================================
================================ BPluginInterface ============================
============================================================================*/

class B_CORE_EXPORT BPluginInterface
{
public:
    struct PluginInfo
    {
        QString organization;
        QString copyrightYears;
        QString website;
        QString descriptionFileName;
        QString changeLogFileName;
        QString licenseFileName;
        QString authorsFileName;
        QString translatorsFileName;
        QString thanksToFileName;
    };
    struct PluginInfoStatic
    {
        QString organization;
        QString copyrightYears;
        QString website;
        QString description;
        QString changeLog;
        BPersonInfoList authors;
        BPersonInfoList translators;
        BPersonInfoList thanksTo;
        QString license;
    };
public:
    virtual ~BPluginInterface() {}
public:
    virtual QString type() const = 0;
    virtual QString name() const = 0;
    virtual bool prefereStaticInfo() const = 0;
    virtual PluginInfoStatic staticInfo() const = 0;
    virtual PluginInfo info() const = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

Q_DECLARE_INTERFACE(BPluginInterface, "BeQt.BPluginInterface")

#endif // BPLUGININTERFACE_H
