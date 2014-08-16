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
#include "bpluginwrapper_p.h"

#include "bplugininterface.h"
#include "bapplicationbase.h"

#include "bdirtools.h"
#include "bversion.h"

#include <QDebug>
#include <QList>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QObject>
#include <QPluginLoader>
#include <QSettings>
#include <QString>

/*============================================================================
================================ BPluginWrapperPrivate =======================
============================================================================*/

/*============================== Static public variables ===================*/

QStringList BPluginWrapperPrivate::acctptableTypes;
QMap<QString, BPluginWrapper *> BPluginWrapperPrivate::globalMap;
BPluginWrapper::InterfaceTestFunction BPluginWrapperPrivate::testFunction;

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

void BPluginWrapperPrivate::createLoader()
{
    if (!loader.isNull())
        return;
    loader = new QPluginLoader;
    loader->setFileName(fileName);
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

void BPluginWrapperPrivate::deleteLoader()
{
    if (loader.isNull())
        return;
    if (loader->isLoaded())
        loader->unload();
    loader->deleteLater();
}

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
    QString iid = iface->id();
    BVersion v = iface->version();
    if ((!acctptableTypes.isEmpty() && !acctptableTypes.contains(tp)) || (testFunction && !testFunction(obj))
            || globalMap.contains(iid)) {
        deleteLoader();
        return false;
    }
    instance = obj;
    interface = iface;
    type = tp;
    id = iid;
    version = v;
    prefereStaticInfo = interface->prefereStaticInfo();
    staticInfo = interface->staticInfo();
    info = interface->info();
    loaded = true;
    globalMap.insert(iid, q_func());
    QString fn = BDirTools::createConfFileName(iid);
    if (!fn.isEmpty()) {
        settings = new QSettings(fn, QSettings::IniFormat);
        settings->setIniCodec("UTF-8");
    }
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
    globalMap.remove(id);
    delete settings;
}

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
    return BPluginWrapperPrivate::globalMap.value(i->id());
}

bool BPluginWrapper::removeSettings(const QString &pluginName)
{
    if (pluginName.isEmpty())
        return false;
    QString fn = BDirTools::createConfFileName(pluginName);
    if (fn.isEmpty())
        return false;
    return QFile::remove(fn);
}

void BPluginWrapper::setAcceptableTypes(const QStringList &list)
{
    BPluginWrapperPrivate::acctptableTypes = list;
}

void BPluginWrapper::setInterfaceTestFunction(InterfaceTestFunction function)
{
    BPluginWrapperPrivate::testFunction = function;
}

/*============================== Public methods ============================*/

QString BPluginWrapper::fileName() const
{
    return d_func()->fileName;
}

QString BPluginWrapper::id() const
{
    return d_func()->id;
}

BPluginInterface::PluginInfo BPluginWrapper::info() const
{
    return d_func()->info;
}

void BPluginWrapper::install()
{
    BApplicationBase::installPlugin(this);
}

QObject *BPluginWrapper::instance() const
{
    return d_func()->instance;
}

BPluginInterface *BPluginWrapper::interface() const
{
    return d_func()->interface;
}

bool BPluginWrapper::isActivated() const
{
    return d_func()->activated;
}

bool BPluginWrapper::isLoaded() const
{
    return d_func()->loaded;
}

bool BPluginWrapper::prefereStaticInfo() const
{
    return d_func()->prefereStaticInfo;
}

void BPluginWrapper::remove()
{
    BApplicationBase::removePlugin(this);
}

QSettings *BPluginWrapper::settings() const
{
    return d_func()->settings.data();
}

BPluginInterface::StaticPluginInfo BPluginWrapper::staticInfo() const
{
    return d_func()->staticInfo;
}

void BPluginWrapper::setFileName(const QString &fn)
{
    if ( isActivated() || isLoaded() )
        return;
    B_D(BPluginWrapper);
    if (fn == d->fileName)
        return;
    d->fileName = fn;
    d->type.clear();
    d->id.clear();
    d->version.clear();
    d->info = BPluginInterface::PluginInfo();
}

QString BPluginWrapper::title() const
{
    return d_func()->interface ? d_func()->interface->title() : QString();
}

QString BPluginWrapper::type() const
{
    return d_func()->type;
}

BVersion BPluginWrapper::version() const
{
    return d_func()->version;
}

/*============================== Public slots ==============================*/

bool BPluginWrapper::activate()
{
    return d_func()->activate();
}

void BPluginWrapper::deactivate()
{
    d_func()->deactivate();
}

bool BPluginWrapper::load()
{
    return d_func()->load();
}

void BPluginWrapper::setActivated(bool b)
{
    if (b)
        activate();
    else
        deactivate();
}

void BPluginWrapper::setLoaded(bool b)
{
    if (b)
        load();
    else
        unload();
}

void BPluginWrapper::unload()
{
    d_func()->unload();
}
