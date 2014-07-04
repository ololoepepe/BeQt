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

#ifndef BPLUGINWRAPPER_P_H
#define BPLUGINWRAPPER_P_H

class BPluginWrapper;

class QPluginLoader;

#include "bpluginwrapper.h"
#include "bglobal.h"
#include "bplugininterface.h"
#include "bbase_p.h"
#include "bversion.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QPointer>
#include <QSettings>

/*============================================================================
================================ BPluginWrapperPrivate =======================
============================================================================*/

class B_CORE_EXPORT BPluginWrapperPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPluginWrapper)
public:
    explicit BPluginWrapperPrivate(BPluginWrapper *q);
    ~BPluginWrapperPrivate();
public:
    void init();
    bool load();
    void unload();
    bool activate();
    void deactivate();
    void createLoader();
    void deleteLoader();
public:
    static QMap<QString, BPluginWrapper *> globalMap;
    static QStringList acctptableTypes;
    static BPluginWrapper::InterfaceTestFunction testFunction;
public:
    QString fileName;
    QPointer<QPluginLoader> loader;
    QObject *instance;
    BPluginInterface *interface;
    QPointer<QSettings> settings;
    bool loaded;
    bool activated;
    QString type;
    QString name;
    BVersion version;
    bool prefereStaticInfo;
    BPluginInterface::PluginInfo info;
    BPluginInterface::StaticPluginInfo staticInfo;
private:
    Q_DISABLE_COPY(BPluginWrapperPrivate)
};

#endif // BPLUGINWRAPPER_P_H
