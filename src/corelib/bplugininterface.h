#ifndef BPLUGININTERFACE_H
#define BPLUGININTERFACE_H

#include "bglobal.h"
#include "bpersoninfo.h"
#include "bdirtools.h"

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
        QString descriptionFileName;
        QString changeLogFileName;
        QString licenseFileName;
        QString authorsFileName;
        QString translatorsFileName;
        QString thanksToFileName;
    };
    struct PluginInfoStatic
    {
        QString organization;
        QString copyrightYears;
        QString website;
        QString description;
        QString changeLog;
        BPersonInfoList authors;
        BPersonInfoList translators;
        BPersonInfoList thanksTo;
        QString license;
    };
public:
    virtual ~BPluginInterface() {}
public:
    virtual QString type() const = 0;
    virtual QString name() const = 0;
    virtual bool prefereStaticInfo() const = 0;
    virtual PluginInfoStatic staticInfo() const = 0;
    virtual PluginInfo info() const = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
};

Q_DECLARE_INTERFACE(BPluginInterface, "BeQt.BPluginInterface")

#endif // BPLUGININTERFACE_H
