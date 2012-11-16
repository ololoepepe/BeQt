#include "bpluginwrapper.h"
#include "bplugininterface.h"
#include "bcoreapplication.h"
#include "bcoreapplication_p.h"
#include "bpluginwrapper_p.h"

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

QMap<QString, BPluginWrapper *> BPluginWrapperPrivate::globalQMap;

//

QSettings *BPluginWrapperPrivate::createPluginSettingsInstance(const QString &pluginName, bool createFile)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return 0;
    BCoreApplicationPrivate *const dapp = BCoreApplication::instance()->d_func();
    return new QSettings( dapp->confFileName(dapp->userPrefix, pluginName, createFile), QSettings::IniFormat);
}

//

BPluginWrapperPrivate::BPluginWrapperPrivate(BPluginWrapper *q) :
    BBasePrivate(q)
{
    instance = 0;
    interface = 0;
    testFunction = 0;
    loaded = false;
    activated = false;
}

BPluginWrapperPrivate::~BPluginWrapperPrivate()
{
    unload();
}

//

bool BPluginWrapperPrivate::load()
{
    if (loaded)
        return true;
    createLoader();
    if ( !loader->load() )
    {
        deleteLoader();
        return false;
    }
    QObject *obj = loader->instance();
    BPluginInterface *iface = qobject_cast<BPluginInterface *>(obj);
    if (!obj || !iface)
    {
        deleteLoader();
        return false;
    }
    QString tp = iface->type();
    QString nm = iface->name();
    if ( ( !acctptableTypes.isEmpty() && !acctptableTypes.contains(tp) ) || ( testFunction && !testFunction(obj) ) ||
         globalQMap.contains(nm) )
    {
        deleteLoader();
        return false;
    }
    instance = obj;
    interface = iface;
    type = tp;
    name = nm;
    info = interface->info();
    loaded = true;
    globalQMap.insert( nm, q_func() );
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
    globalQMap.remove(name);
}

bool BPluginWrapperPrivate::activate()
{
    if (activated)
        return true;
    if ( !loaded && !load() )
        return false;
    interface->activate();
    activated = true;
    B_Q(BPluginWrapper);
    QMetaObject::invokeMethod(q, "activated");
    if ( BCoreApplicationPrivate::testCoreInit("BPluginWrapper") )
    {
        BCoreApplicationPrivate *const dapp = BCoreApplication::instance()->d_func();
        if ( dapp->plugins.contains(q) )
            dapp->pluginActivated(q);
    }
    return true;
}

void BPluginWrapperPrivate::deactivate()
{
    if (!activated)
        return;
    B_Q(BPluginWrapper);
    QMetaObject::invokeMethod(q, "aboutToBeDeactivated");
    if ( BCoreApplicationPrivate::testCoreInit("BPluginWrapper") )
    {
        BCoreApplicationPrivate *const dapp = BCoreApplication::instance()->d_func();
        if ( dapp->plugins.contains(q) )
            dapp->pluginAboutToBeDeactivated(q);
    }
    interface->deactivate();
    activated = false;
}

void BPluginWrapperPrivate::createLoader()
{
    if ( !loader.isNull() )
        return;
    loader = new QPluginLoader;
    loader->setFileName(fileName);
}

void BPluginWrapperPrivate::deleteLoader()
{
    if ( loader.isNull() )
        return;
    if ( loader->isLoaded() )
        loader->unload();
    loader->deleteLater();
}

//

QSettings *BPluginWrapper::createPluginSettingsInstance(const QString &pluginName, bool createFile)
{
    return BPluginWrapperPrivate::createPluginSettingsInstance(pluginName, createFile);
}

//

BPluginWrapper::BPluginWrapper(QObject *parent) :
    QObject(parent), BBase( *new BPluginWrapperPrivate(this) )
{
    //
}

BPluginWrapper::BPluginWrapper(const QString &fileName, QObject *parent) :
    QObject(parent), BBase( *new BPluginWrapperPrivate(this) )
{
    setFileName(fileName);
}

BPluginWrapper::~BPluginWrapper()
{
    //
}

//

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

void BPluginWrapper::setFileName(const QString &fn)
{
    if ( isActivated() || isLoaded() )
        return;
    B_D(BPluginWrapper);
    if (fn == d->fileName)
        return;
    d->fileName = fn;
    d->type = "";
    d->name = "";
    d->info = BPluginInterface::PluginInfo();
}

void BPluginWrapper::setAcceptableTypes(const QStringList &list)
{
    d_func()->acctptableTypes = list;
}

void BPluginWrapper::setInterfaceTestFunction(InterfaceTestFunction function)
{
    d_func()->testFunction = function;
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

QStringList BPluginWrapper::acceptableFileTypes() const
{
    return d_func()->acctptableTypes;
}

BPluginWrapper::InterfaceTestFunction BPluginWrapper::interfacetestFunction() const
{
    return d_func()->testFunction;
}

QString BPluginWrapper::type() const
{
    return d_func()->type;
}

QString BPluginWrapper::name() const
{
    return d_func()->name;
}

BPluginInterface::PluginInfo BPluginWrapper::info() const
{
    return d_func()->info;
}

QObject *BPluginWrapper::instance() const
{
    return d_func()->instance;
}

BPluginInterface *BPluginWrapper::interface() const
{
    return d_func()->interface;
}

//

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

//

BPluginWrapper::BPluginWrapper(BPluginWrapperPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    //
}
