#ifndef BPLUGINWRAPPER_P_H
#define BPLUGINWRAPPER_P_H

#include "bpluginwrapper.h"

class BPluginWrapper;

class QPluginLoader;

#include <BeQt>
#include <BPluginInterface>

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>

class B_CORE_EXPORT BPluginWrapperPrivate : public QObject
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPluginWrapper)
public:
    static QMap<QString, BPluginWrapper *> globalQMap;
    //
    explicit BPluginWrapperPrivate(BPluginWrapper *q);
    ~BPluginWrapperPrivate();
    //
    void activate();
    void deactivate();
    //
    BPluginWrapper *const _m_q;
    QPluginLoader *loader;
    QStringList acctptableTypes;
    BPluginWrapper::InterfaceTestFunction testFunction;
    QObject *instance;
    BPluginInterface *interface;
    bool valid;
    bool activated;
    QString type;
    QString name;
    BPluginInterface::PluginInfo info;
private:
    Q_DISABLE_COPY(BPluginWrapperPrivate)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGINWRAPPER_P_H
