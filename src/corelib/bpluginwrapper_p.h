#ifndef BPLUGINWRAPPER_P_H
#define BPLUGINWRAPPER_P_H

class BPluginWrapper;

class QPluginLoader;
class QSettings;

#include "bpluginwrapper.h"
#include "bglobal.h"
#include "bplugininterface.h"
#include "bbase_p.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>

class B_CORE_EXPORT BPluginWrapperPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPluginWrapper)
public:
    static QSettings *createPluginSettingsInstance(const QString &pluginName, bool createFile);
    //
    static QMap<QString, BPluginWrapper *> globalQMap;
    //
    explicit BPluginWrapperPrivate(BPluginWrapper *q);
    ~BPluginWrapperPrivate();
    //
    void activate();
    void deactivate(); 
    //
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
