#ifndef BGUIPLUGININTERFACE_H
#define BGUIPLUGININTERFACE_H

class BAbstractSettingsTab;

#include <QtPlugin>
#include <QVariantMap>
#include <QString>

class BGuiPluginInterface
{
public:
    virtual ~BGuiPluginInterface() {}
    //
    virtual BAbstractSettingsTab *settingsTab() = 0;
    virtual QString settingsTabId() const = 0;
    virtual void applySettings(const QVariantMap &settings) = 0;
};

Q_DECLARE_INTERFACE(BGuiPluginInterface, "BeQt.BGuiPluginInterface")

#endif // BGUIPLUGININTERFACE_H
