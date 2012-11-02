#include "bpluginwrapper.h"
#include "bpluginwrapper_p.h"

#include <BPluginInterface>
#include <BCoreApplication>

#include <private/bcoreapplication_p.h>

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QList>
#include <QPluginLoader>
#include <QMetaObject>
#include <QMap>

QMap<QString, BPluginWrapper *> BPluginWrapperPrivate::globalQMap;

//

BPluginWrapperPrivate::BPluginWrapperPrivate(BPluginWrapper *q) :
    _m_q(q)
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
    if ( !name.isEmpty() )
        globalQMap.remove(name);
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
    BPluginWrapper *q = q_func();
    globalQMap.insert(name, q);
    QMetaObject::invokeMethod(q, "activated");
    if ( BCoreApplicationPrivate::testCoreInit("BPluginWrapper") )
    {
        BCoreApplicationPrivate *const dapp = BCoreApplication::instance()->d_func();
        if ( dapp->plugins.contains(q) )
            dapp->emitPluginActivated(q);
    }
}
void BPluginWrapperPrivate::deactivate()
{
    if (!activated)
        return;
    BPluginWrapper *q = q_func();
    QMetaObject::invokeMethod(q, "aboutToBeDeactivated");
    if ( BCoreApplicationPrivate::testCoreInit("BPluginWrapper") )
    {
        BCoreApplicationPrivate *const dapp = BCoreApplication::instance()->d_func();
        if ( dapp->plugins.contains(q) )
            dapp->emitPluginAboutToBeDeactivated(q);
    }
    interface->deactivate();
    loader->unload();
    instance = 0;
    interface = 0;
}

//

BPluginWrapper::BPluginWrapper(const QString &fileName, QObject *parent) :
    QObject(parent), _m_d( new BPluginWrapperPrivate(this) )
{
    setFileName(fileName);
}

BPluginWrapper::~BPluginWrapper()
{
    delete _m_d;
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
    BPluginWrapperPrivate *const d = d_func();
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
