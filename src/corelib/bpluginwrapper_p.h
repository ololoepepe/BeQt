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

class QPluginLoader;

#include "bpluginwrapper.h"

#include "bbase_p.h"
#include "bplugininterface.h"
#include "bversion.h"

#include <QList>
#include <QMap>
#include <QPointer>
#include <QSettings>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BPluginWrapperPrivate =======================
============================================================================*/

class B_CORE_EXPORT BPluginWrapperPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPluginWrapper)
public:
    static QStringList acctptableTypes;
    static QMap<QString, BPluginWrapper *> globalMap;
    static BPluginWrapper::InterfaceTestFunction testFunction;
public:
    bool activated;
    QString fileName;
    QString id;
    BPluginInterface::PluginInfo info;
    QObject *instance;
    BPluginInterface *interface;
    bool loaded;
    QPointer<QPluginLoader> loader;
    bool prefereStaticInfo;
    QPointer<QSettings> settings;
    BPluginInterface::StaticPluginInfo staticInfo;
    QString title;
    QString type;
    BVersion version;
public:
    explicit BPluginWrapperPrivate(BPluginWrapper *q);
    ~BPluginWrapperPrivate();
public:
    bool activate();
    void createLoader();
    void deactivate();
    void deleteLoader();
    void init();
    bool load();
    void unload();
private:
    Q_DISABLE_COPY(BPluginWrapperPrivate)
};

#endif // BPLUGINWRAPPER_P_H
