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
#include <QPointer>

/*============================================================================
================================ Plugin Wrapper Private
============================================================================*/

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
    bool load();
    void unload();
    bool activate();
    void deactivate();
    void createLoader();
    void deleteLoader();
    //
    QString fileName;
    QPointer<QPluginLoader> loader;
    QStringList acctptableTypes;
    BPluginWrapper::InterfaceTestFunction testFunction;
    QObject *instance;
    BPluginInterface *interface;
    bool loaded;
    bool activated;
    QString type;
    QString name;
    BPluginInterface::PluginInfo info;
protected:
    BPluginWrapperPrivate(BPluginWrapper &q, BBasePrivateObject &o);
private:
    Q_DISABLE_COPY(BPluginWrapperPrivate)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGINWRAPPER_P_H
