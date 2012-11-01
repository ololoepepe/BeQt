#include "bplugin.h"

#include <BPluginInterface>
#include <BTranslator>
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
    BPlugin *_m_q;
    QPluginLoader *loader;
    QObject *instance;
    BPluginInterface *interface;
    BTranslator *translator;
    //
    BPluginPrivate(BPlugin *q) :
        _m_q(q)
    {
        loader = new QPluginLoader;
        instance = 0;
        interface = 0;
        translator = 0;
    }
    ~BPluginPrivate()
    {
        loader->deleteLater();
        if (translator)
            translator->deleteLater();
    }
    //
    void activate()
    {
        BCoreApplication *app = BCoreApplication::instance();
        if (app)
            app->d_func()->emitPluginActivated( q_func() );
        //
        QMetaObject::invokeMethod(q_func(), "activated");

    }
    void deactivate()
    {
        BCoreApplication *app = BCoreApplication::instance();
        if (app)
            app->d_func()->emitPluginAboutToBeDeactivated( q_func() );
        //
        QMetaObject::invokeMethod(q_func(), "deactivated");
    }
private:
    friend class BCoreApplicationPrivate;
};

//

BPlugin::BPlugin(const QString &fileName, QObject *parent) :
    QObject(parent), _m_d( new BPluginPrivate(this) )
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
    //
}

void BPlugin::setLocale(const QLocale &l)
{
    if (d_func()->translator)
        d_func()->translator->setLocale(l);
}

void BPlugin::reloadTranslator()
{
    if (d_func()->translator)
        d_func()->translator->reload();
}

BPlugin::PluginState BPlugin::state() const
{
    //
}

bool BPlugin::isValid() const
{
    //
}

QString BPlugin::type() const
{
    //
}

QString BPlugin::name() const
{
    //
}

BPluginInterface::PluginInfo BPlugin::info() const
{
    //
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
    //
}

void BPlugin::deactivate()
{
    //
}
