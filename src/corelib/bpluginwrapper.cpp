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
    loader = new QPluginLoader;
    loader->load();
    instance = 0;
    interface = 0;
    testFunction = 0;
    valid = false;
    activated = false;
}

BPluginWrapperPrivate::~BPluginWrapperPrivate()
{
    //if ( !name.isEmpty() )
        //globalQMap.remove(name);
    deactivate();
    loader->deleteLater();
}

//

void BPluginWrapperPrivate::activate()
{
    if (activated)
        return;
    valid = false;
    if ( !loader->load() )
        return;
    QObject *obj = loader->instance();
    BPluginInterface *iface = qobject_cast<BPluginInterface *>(obj);
    if (!obj || !iface)
    {
        loader->unload();
        return;
    }
    QString tp = iface->type();
    QString nm = iface->name();
    if ( ( !acctptableTypes.isEmpty() && !acctptableTypes.contains(tp) ) || ( testFunction && !testFunction(obj) ) ||
         globalQMap.contains(nm) )
    {
        loader->unload();
        return;
    }
    instance = obj;
    interface = iface;
    type = tp;
    name = nm;
    info = interface->info();
    activated = true;
    valid = true;
    interface->activate();
    B_Q(BPluginWrapper);
    globalQMap.insert(name, q);
    QMetaObject::invokeMethod(q, "activated");
    if ( BCoreApplicationPrivate::testCoreInit("BPluginWrapper") )
    {
        BCoreApplicationPrivate *const dapp = BCoreApplication::instance()->d_func();
        if ( dapp->plugins.contains(q) )
        {
            dapp->deactivatedPlugins.removeAll(name);
            dapp->emitPluginActivated(q);
        }
    }
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
            dapp->emitPluginAboutToBeDeactivated(q);
    }
    interface->deactivate();
    instance->deleteLater();
    instance = 0;
    interface = 0;
    activated = false;
    globalQMap.remove(name);
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

void BPluginWrapper::setActivated(bool b)
{
    if (b)
        activate();
    else
        deactivate();
}

void BPluginWrapper::setFileName(const QString &fileName)
{
    if ( isActivated() )
        return;
    B_D(BPluginWrapper);
    BPluginWrapperPrivate::globalQMap.remove(d->name);
    d->loader->setFileName(fileName);
    d->instance = 0;
    d->interface = 0;
    d->type = "";
    d->name = "";
    d->info = BPluginInterface::PluginInfo();
    d->valid = false;
}

void BPluginWrapper::setAcceptableTypes(const QStringList &list)
{
    d_func()->acctptableTypes = list;
}

void BPluginWrapper::setInterfaceTestFunction(InterfaceTestFunction function)
{
    d_func()->testFunction = function;
}

bool BPluginWrapper::isActivated() const
{
    return d_func()->activated;
}

bool BPluginWrapper::isValid() const
{
    return d_func()->valid;
}

QString BPluginWrapper::fileName() const
{
    return d_func()->loader->fileName();
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

void BPluginWrapper::activate()
{
    d_func()->activate();
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
