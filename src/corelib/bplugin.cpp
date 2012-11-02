#include "bplugin.h"

#include <BPluginInterface>
#include <BCoreApplication>

#include <private/bcoreapplication_p.h>

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QList>
#include <QPluginLoader>
#include <QMetaObject>

class BPluginPrivate
{
    B_DECLARE_PUBLIC(BPlugin)
public:
    BPlugin *const _m_q;
    const QString FileName;
    //
    QPluginLoader *loader;
    QObject *instance;
    BPluginInterface *interface;
    bool isValid;
    BPlugin::PluginState state;
    QString type;
    QString name;
    BPluginInterface::PluginInfo info;
    //
    BPluginPrivate(BPlugin *q, const QString &fileName);
    ~BPluginPrivate();
    //
    void activate();
    void deactivate();
private:
    friend class BCoreApplicationPrivate;
};

//

BPluginPrivate::BPluginPrivate(BPlugin *q, const QString &fileName) :
    _m_q(q), FileName(fileName)
{
    loader = new QPluginLoader(FileName);
    loader->load();
    instance = loader->instance();
    interface = qobject_cast<BPluginInterface *>(instance);
    if (interface)
    {
        type = interface->type();
        name = interface->name();
        info = interface->info();
    }
    else
    {
        instance = 0;
    }
    isValid = interface;
    state = BPlugin::NotInitialized;
}
BPluginPrivate::~BPluginPrivate()
{
    loader->deleteLater();
}

//

void BPluginPrivate::activate()
{
    if (!isValid)
        return;
    if (!instance)
    {
        loader->load();
        instance = loader->instance();
        interface = qobject_cast<BPluginInterface *>(instance);
        if (interface)
        {
            type = interface->type();
            name = interface->name();
            info = interface->info();
            state = BPlugin::Activated;
            BCoreApplication *app = BCoreApplication::instance();
            if (app)
                app->d_func()->emitPluginActivated( q_func() );
            QMetaObject::invokeMethod(q_func(), "activated");
        }
        else
        {
            instance = 0;
        }
    }
}
void BPluginPrivate::deactivate()
{
    if (!isValid || BPlugin::Activated != state)
        return;
    BCoreApplication *app = BCoreApplication::instance();
    if (app)
        app->d_func()->emitPluginAboutToBeDeactivated( q_func() );
    loader->unload();
    instance = 0;
    interface = 0;
    state = BPlugin::Deactivated;
    QMetaObject::invokeMethod(q_func(), "deactivated");
}

//

BPlugin::BPlugin(const QString &fileName, QObject *parent) :
    QObject(parent), _m_d( new BPluginPrivate(this, fileName) )
{
    //
}

BPlugin::~BPlugin()
{
    delete _m_d;
}

//

void BPlugin::setActivated(bool b)
{
    if ( (b && state() == Activated) || (!b && state() == Deactivated) )
        return;
    if (b)
        activate();
    else
        deactivate();
}

QString BPlugin::fileName() const
{
    return d_func()->FileName;
}

BPlugin::PluginState BPlugin::state() const
{
    return d_func()->state;
}

bool BPlugin::isValid() const
{
    return d_func()->isValid;
}

QString BPlugin::type() const
{
    return d_func()->type;
}

QString BPlugin::name() const
{
    return d_func()->name;
}

BPluginInterface::PluginInfo BPlugin::info() const
{
    return d_func()->info;
}

QObject *BPlugin::instance() const
{
    return d_func()->instance;
}

BPluginInterface *BPlugin::interface() const
{
    return d_func()->interface;
}

//

void BPlugin::activate()
{
    if (state() == Activated)
        return;
    d_func()->activate();
}

void BPlugin::deactivate()
{
    if (state() == Deactivated)
        d_func()->deactivate();
}
