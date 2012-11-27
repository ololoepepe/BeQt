#ifndef BPLUGININTERFACE_H
#define BPLUGININTERFACE_H

#include "bglobal.h"
#include "bpersoninfoprovider.h"

#include <QString>
#include <QtPlugin>
#include <QList>

/*============================================================================
================================ BPluginInterface ============================
============================================================================*/

class B_CORE_EXPORT BPluginInterface
{
public:
    struct PluginInfo
    {
        QString organization;
        QString copyrightYears;
        QString website;
        QString description;
        QString changelog;
        BPersonInfoProvider::PersonInfoList authors;
        BPersonInfoProvider::PersonInfoList translators;
        BPersonInfoProvider::PersonInfoList thanksTo;
        QString license;
    };
public:
    virtual ~BPluginInterface() {}
public:
    virtual QString type() const = 0;
    virtual QString name() const = 0;
    virtual PluginInfo info() const = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

Q_DECLARE_INTERFACE(BPluginInterface, "BeQt.BPluginInterface")

#endif // BPLUGININTERFACE_H
