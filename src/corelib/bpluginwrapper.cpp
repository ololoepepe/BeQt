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

#include "bpluginwrapper.h"
#include "bplugininterface.h"
#include "bcoreapplication.h"
#include "bcoreapplication_p.h"
#include "bapplicationbase.h"
#include "bapplicationbase_p.h"
#include "bpluginwrapper_p.h"
#include "bversion.h"

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QList>
#include <QPluginLoader>
#include <QMetaObject>
#include <QMap>
#include <QObject>
#include <QSettings>

#include <QDebug>

/*============================================================================
================================ BPluginWrapperPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BPluginWrapperPrivate::BPluginWrapperPrivate(BPluginWrapper *q) :
    BBasePrivate(q)
{
    //
}

BPluginWrapperPrivate::~BPluginWrapperPrivate()
{
    unload();
}

/*============================== Public methods ============================*/

void BPluginWrapperPrivate::init()
{
    instance = 0;
    interface = 0;
    testFunction = 0;
    loaded = false;
    activated = false;
    prefereStaticInfo = false;
}

bool BPluginWrapperPrivate::load()
{
    if (loaded)
        return true;
    createLoader();
    if (!loader->load()) {
        qDebug() << loader->errorString();
        deleteLoader();
        return false;
    }
    QObject *obj = loader->instance();
    BPluginInterface *iface = qobject_cast<BPluginInterface *>(obj);
    if (!obj || !iface) {
        deleteLoader();
        return false;
    }
    QString tp = iface->type();
    QString nm = iface->name();
    BVersion v = iface->version();
    if ((!acctptableTypes.isEmpty() && !acctptableTypes.contains(tp)) || (testFunction && !testFunction(obj))
            || globalMap.contains(nm)) {
        deleteLoader();
        return false;
    }
    instance = obj;
    interface = iface;
    type = tp;
    name = nm;
    version = v;
    prefereStaticInfo = interface->prefereStaticInfo();
    staticInfo = interface->staticInfo();
    info = interface->info();
    loaded = true;
    globalMap.insert( nm, q_func() );
    //TODO
    //settings = BApplicationBase::ds_func()->createSettingsInstance( interface->name() );
    return true;
}

void BPluginWrapperPrivate::unload()
{
    if (!loaded)
        return;
    if (activated)
        deactivate();
    deleteLoader();
    instance = 0;
    interface = 0;
    loaded = false;
    globalMap.remove(name);
    delete settings;
}

bool BPluginWrapperPrivate::activate()
{
    if (activated)
        return true;
    if (!loaded && !load())
        return false;
    interface->activate();
    activated = true;
    B_Q(BPluginWrapper);
    QMetaObject::invokeMethod(q, "activated");
    return true;
}

void BPluginWrapperPrivate::deactivate()
{
    if (!activated)
        return;
    B_Q(BPluginWrapper);
    QMetaObject::invokeMethod(q, "aboutToBeDeactivated");
    interface->deactivate();
    activated = false;
}

void BPluginWrapperPrivate::createLoader()
{
    if (!loader.isNull())
        return;
    loader = new QPluginLoader;
    loader->setFileName(fileName);
}

void BPluginWrapperPrivate::deleteLoader()
{
    if (loader.isNull())
        return;
    if (loader->isLoaded())
        loader->unload();
    loader->deleteLater();
}

/*============================== Static public variables ===================*/

QMap<QString, BPluginWrapper *> BPluginWrapperPrivate::globalMap;
QStringList BPluginWrapperPrivate::acctptableTypes;
BPluginWrapper::InterfaceTestFunction BPluginWrapperPrivate::testFunction;

/*============================================================================
================================ BPluginWrapper ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BPluginWrapper::BPluginWrapper(QObject *parent) :
    QObject(parent), BBase( *new BPluginWrapperPrivate(this) )
{
    d_func()->init();
}

BPluginWrapper::BPluginWrapper(const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BPluginWrapperPrivate(this))
{
    d_func()->init();
    setFileName(fileName);
}

BPluginWrapper::~BPluginWrapper()
{
    //
}

/*============================== Protected constructors ====================*/

BPluginWrapper::BPluginWrapper(BPluginWrapperPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

void BPluginWrapper::setAcceptableTypes(const QStringList &list)
{
    BPluginWrapperPrivate::acctptableTypes = list;
}

void BPluginWrapper::setInterfaceTestFunction(InterfaceTestFunction function)
{
    BPluginWrapperPrivate::testFunction = function;
}

QStringList BPluginWrapper::acceptableTypes()
{
    return BPluginWrapperPrivate::acctptableTypes;
}

BPluginWrapper::InterfaceTestFunction BPluginWrapper::interfaceTestFunction()
{
    return BPluginWrapperPrivate::testFunction;
}

BPluginWrapper *BPluginWrapper::parentWrapper(const BPluginInterface *i)
{
    if (!i)
        return 0;
    return BPluginWrapperPrivate::globalMap.value(i->name());
}

/*============================== Public methods ============================*/

void BPluginWrapper::setFileName(const QString &fn)
{
    if ( isActivated() || isLoaded() )
        return;
    B_D(BPluginWrapper);
    if (fn == d->fileName)
        return;
    d->fileName = fn;
    d->type.clear();
    d->name.clear();
    d->version.clear();
    d->info = BPluginInterface::PluginInfo();
}

void BPluginWrapper::install()
{
    BApplicationBase::installPlugin(this);
}

void BPluginWrapper::remove()
{
    BApplicationBase::removePlugin(this);
}

bool BPluginWrapper::isLoaded() const
{
    return d_func()->loaded;
}

bool BPluginWrapper::isActivated() const
{
    return d_func()->activated;
}

QString BPluginWrapper::fileName() const
{
    return d_func()->fileName;
}

QString BPluginWrapper::type() const
{
    return d_func()->type;
}

QString BPluginWrapper::name() const
{
    return d_func()->name;
}

BVersion BPluginWrapper::version() const
{
    return d_func()->version;
}

bool BPluginWrapper::prefereStaticInfo() const
{
    return d_func()->prefereStaticInfo;
}

BPluginInterface::PluginInfo BPluginWrapper::info() const
{
    return d_func()->info;
}

BPluginInterface::StaticPluginInfo BPluginWrapper::staticInfo() const
{
    return d_func()->staticInfo;
}

QObject *BPluginWrapper::instance() const
{
    return d_func()->instance;
}

BPluginInterface *BPluginWrapper::interface() const
{
    return d_func()->interface;
}

QSettings *BPluginWrapper::settings() const
{
    return d_func()->settings.data();
}

/*============================== Public slots ==============================*/

bool BPluginWrapper::load()
{
    return d_func()->load();
}

void BPluginWrapper::unload()
{
    d_func()->unload();
}

bool BPluginWrapper::activate()
{
    return d_func()->activate();
}

void BPluginWrapper::deactivate()
{
    d_func()->deactivate();
}

void BPluginWrapper::setLoaded(bool b)
{
    if (b)
        load();
    else
        unload();
}

void BPluginWrapper::setActivated(bool b)
{
    if (b)
        activate();
    else
        deactivate();
}
