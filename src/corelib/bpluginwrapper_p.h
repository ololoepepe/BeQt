#ifndef BPLUGINWRAPPER_P_H
#define BPLUGINWRAPPER_P_H

class BPluginWrapper;

class QPluginLoader;

#include "bpluginwrapper.h"
#include "bglobal.h"
#include "bplugininterface.h"
#include "bbase_p.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QPointer>
#include <QSettings>

/*============================================================================
================================ BPluginWrapperPrivate =======================
============================================================================*/

class B_CORE_EXPORT BPluginWrapperPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPluginWrapper)
public:
    explicit BPluginWrapperPrivate(BPluginWrapper *q);
    ~BPluginWrapperPrivate();
public:
    void init();
    bool load();
    void unload();
    bool activate();
    void deactivate();
    void createLoader();
    void deleteLoader();
public slots:
    void initSettings();
public:
    static QMap<QString, BPluginWrapper *> globalQMap;
    static QStringList acctptableTypes;
    static BPluginWrapper::InterfaceTestFunction testFunction;
public:
    QString fileName;
    QPointer<QPluginLoader> loader;
    QObject *instance;
    BPluginInterface *interface;
    QPointer<QSettings> settings;
    bool loaded;
    bool activated;
    QString type;
    QString name;
    bool prefereStaticInfo;
    BPluginInterface::PluginInfo info;
    BPluginInterface::PluginInfoStatic staticInfo;
private:
    Q_DISABLE_COPY(BPluginWrapperPrivate)
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGINWRAPPER_P_H
