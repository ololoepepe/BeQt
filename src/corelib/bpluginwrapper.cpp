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
    QString tt = iface->title();
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
    title = tt;
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

/*!
\class BPluginWrapper
\inmodule BeQtCore
\brief The BPluginWrapper class is used to handle plugins: load and unload, activate and deactivate, etc.

The BPluginWrapper provides convenience methods for loading/unloading plugins and activating/deactivating them. It also
holds information about the plugins which were once loaded even if they were unloaded later.

The BPluginWrapper also keeps track of every loaded/unloaded plugin to ensure the uniqueness of the plugins. That
means, there will never be two plugins with the same identifier loaded at the same time.

Example:

\snippet src/corelib/bpluginwrapper.cpp 0

QPluginLoader is used internally to load plugins.

\sa {Plugins system}, BPluginInterface
*/

/*!
\property BPluginWrapper::activated

This property gets the BPluginWrapper's activation state.

By default, this property is false.
*/

/*!
\property BPluginWrapper::fileName

This property gets and sets the BPluginWrapper's source file name.

Previously loaded plugin (if present and loaded) is automatically unloaded.

By default, this property contains an empty QString.
*/

/*!
\property BPluginWrapper::id

This property gets the identifier of the plugin owned by this BPluginWrapper object.

By default, this property contains an empty QString.

\sa BPluginInterface::id()
*/

/*!
\property BPluginWrapper::loaded

This property gets the BPluginWrapper's loading state.

By default, this property is false.
*/

/*!
\property BPluginWrapper::prefereStaticInfo

This property gets the information about preferred info format (dynamic or static) of the plugin owned by this
BPluginWrapper object.

By default, this property is false.

\sa BPluginInterface::prefereStaticInfo()
*/

/*!
\property BPluginWrapper::title

This property gets the title of the plugin owned by this BPluginWrapper object.

By default, this property contains an empty QString.

\sa BPluginInterface::title()
*/

/*!
\property BPluginWrapper::type

This property gets the type of the plugin owned by this BPluginWrapper object.

By default, this property contains an empty QString.

\sa BPluginInterface::type()
*/

/*!
\property BPluginWrapper::version

This property gets the version of the plugin owned by this BPluginWrapper object.

By default, this property contains an invalid BVersion.

\sa BPluginInterface::version()
*/

/*!
\fn BPluginWrapper::aboutToBeDeactivated()

This signal is emitted immediately before the plugin owned by this BPluginWrapper is deactivated.

\sa BPluginInterface::deactivate()
*/

/*!
\fn BPluginWrapper::activated()

This signal is emitted immediately after the plugin owned by this BPluginWrapper is activated.

\sa BPluginInterface::activate()
*/

/*!
\typedef BPluginWrapper::InterfaceTestFunction

The BPluginWrapper::InterfaceTestFunction typedef provides a pointer to a function taking a pointer to const QObject
and returning bool.

\sa interfaceTestFunction()
*/

/*============================== Public constructors =======================*/

/*!
Constructs a plugin wrapper and sets it's parent to \a parent.
*/

BPluginWrapper::BPluginWrapper(QObject *parent) :
    QObject(parent), BBase(*new BPluginWrapperPrivate(this))
{
    d_func()->init();
}

/*!
Constructs a plugin wrapper and sets it's source file name to \a fileName, and parent to \a parent.
*/

BPluginWrapper::BPluginWrapper(const QString &fileName, QObject *parent) :
    QObject(parent), BBase(*new BPluginWrapperPrivate(this))
{
    d_func()->init();
    setFileName(fileName);
}

/*!
Destroys the object, deleting the associated data object.

The plugin is deactivated and unloaded before deletion.
*/

BPluginWrapper::~BPluginWrapper()
{
    //
}

/*============================== Protected constructors ====================*/

/*!
Constructs an object and associates the given data object \a d with it. Parent is set to \a parent.
*/

BPluginWrapper::BPluginWrapper(BPluginWrapperPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

/*!
Returns the list of the acceptable plugin types.

When a plugin is loaded, it's type is checked. If the acceptable types list is not empty, and the type of a plugin is
not in that list, it will not be loaded (actually, it will be immediately unloaded).

\sa setAcceptableTypes()
*/

QStringList BPluginWrapper::acceptableTypes()
{
    return BPluginWrapperPrivate::acctptableTypes;
}

/*!
Returns the pointer to a function used to test interfaces of loaded plugins.

When a plugin is loaded, if the interface test function is set, and the interface does not pass the test (i.e. false is
returned by the test function), the plugin will not be loaded (actually, it will be immediately unloaded).

\sa InterfaceTestFunction, setInterfaceTestFunction()
*/

BPluginWrapper::InterfaceTestFunction BPluginWrapper::interfaceTestFunction()
{
    return BPluginWrapperPrivate::testFunction;
}

/*!
Returns a pointer to the BPluginWrapper object that owns the plugin with the interface \a i.

If \a i is a null pointer, a null poinjter is returned.

\sa pluginWrapper()
*/

BPluginWrapper *BPluginWrapper::parentWrapper(const BPluginInterface *i)
{
    if (!i)
        return 0;
    return BPluginWrapperPrivate::globalMap.value(i->id());
}

/*!
Returns a pointer to the BPluginWrapper object that owns the plugin with the identifier \a pluginId.

If \a pluginId is an empty QString, or if there is no plugin with that identifier, a null poinjter is returned.

\sa parentWrapper()
*/

BPluginWrapper *BPluginWrapper::pluginWrapper(const QString &pluginId)
{
    if (pluginId.isEmpty())
        return 0;
    return BPluginWrapperPrivate::globalMap.value(pluginId);
}

/*!
Removes a settings file for the plugin whith identifier \a pluginName.

Returns true on success. If any error occurs, returns false.

This function is typically used to remove the settings file used by a plugin which is removed in a newer version of
application (or replaced by a plugin with another identifier).

\sa settings()
*/

bool BPluginWrapper::removeSettings(const QString &pluginName)
{
    if (pluginName.isEmpty())
        return false;
    QString fn = BDirTools::createConfFileName(pluginName);
    if (fn.isEmpty())
        return false;
    return QFile::remove(fn);
}

/*!
Sets the list of acctptable plugin types to \a list.

\sa acceptableTypes()
*/

void BPluginWrapper::setAcceptableTypes(const QStringList &list)
{
    BPluginWrapperPrivate::acctptableTypes = list;
}

/*!
Sets the plugin interface test function to \a function.

\sa InterfaceTestFunction, interfaceTestFunction()
*/

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

/*!
Returns dynamic information about the undrelying plugin.

\sa staticInfo(), BPluginInterface::info()
*/

BPluginInterface::PluginInfo BPluginWrapper::info() const
{
    return d_func()->info;
}

/*!
Registers this plugin wrapper in the application plugins system.

\sa BApplicationBase::installPlugin(), {Plugins system}
*/

void BPluginWrapper::install()
{
    BApplicationBase::installPlugin(this);
}

/*!
Returns a pointer to the instance of the underlying plugin object.

\sa interface()
*/

QObject *BPluginWrapper::instance() const
{
    return d_func()->instance;
}

/*!
Returns a pointer to the instance of the underlying plugin object casted to a pointer to BPluginInterface.

\sa instance()
*/

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

/*!
Removes this plugin wrapper from the application plugins system.

\sa BApplicationBase::removePlugin(), {Plugins system}
*/

void BPluginWrapper::remove()
{
    BApplicationBase::removePlugin(this);
}

/*!
Returns a pointer to the QSettings object for the underlying plugin.

For details, see {Settings system}

\sa removeSettings()
*/

QSettings *BPluginWrapper::settings() const
{
    return d_func()->settings.data();
}

/*!
Returns static information about the undrelying plugin.

\sa info(), BPluginInterface::staticInfo()
*/

BPluginInterface::StaticPluginInfo BPluginWrapper::staticInfo() const
{
    return d_func()->staticInfo;
}

void BPluginWrapper::setFileName(const QString &fn)
{
    if (isActivated() || isLoaded())
        return;
    B_D(BPluginWrapper);
    if (fn == d->fileName)
        return;
    d->fileName = fn;
    d->type.clear();
    d->id.clear();
    d->title.clear();
    d->version.clear();
    d->info = BPluginInterface::PluginInfo();
}

QString BPluginWrapper::title() const
{
    return d_func()->interface ? d_func()->interface->title() : d_func()->title;
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

/*!
Activates the underlying plugin. The plugin is loaded if it is not already loaded.

Returns true on success. If any error occurs, returns false.

If the plugin is already activated, nothing is done and true is returned.

\sa deactivate(), BPluginInterface::activate()
*/

bool BPluginWrapper::activate()
{
    return d_func()->activate();
}

/*!
Deactivates the underlying plugin.

\sa activate(), BPluginInterface::deactivate()
*/

void BPluginWrapper::deactivate()
{
    d_func()->deactivate();
}

/*!
Loads the underlying plugin. The plugin is \e not activated.

Returns true on success. If any error occurs, returns false.

If the plugin is already loaded, nothing is done and true is returned.
*/

bool BPluginWrapper::load()
{
    return d_func()->load();
}

/*!
Activates the underlying plugin if \a b is true; otherwise deactivates the plugin.

\sa activate(), deactivate()
*/

void BPluginWrapper::setActivated(bool b)
{
    if (b)
        activate();
    else
        deactivate();
}

/*!
Loads the underlying plugin if \a b is true; otherwise unloads the plugin.

\sa load(), unload()
*/

void BPluginWrapper::setLoaded(bool b)
{
    if (b)
        load();
    else
        unload();
}

/*!
Unloads the underlying plugin. The plugin is deactivated before unloading.

\sa activate()
*/

void BPluginWrapper::unload()
{
    d_func()->unload();
}
