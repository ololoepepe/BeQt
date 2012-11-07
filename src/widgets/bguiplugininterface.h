#ifndef BGUIPLUGININTERFACE_H
#define BGUIPLUGININTERFACE_H

class BAbstractSettingsTab;

class QPixmap;

#include <BeQtCore/BeQt>

#include <QtPlugin>
#include <QVariantMap>

class B_WIDGETS_EXPORT BGuiPluginInterface
{
public:
    virtual ~BGuiPluginInterface() {}
    //
    virtual QPixmap pixmap() const = 0;
    virtual BAbstractSettingsTab *settingsTab() const = 0;
    virtual void handleSettings(const QVariantMap &s) = 0;
};

Q_DECLARE_INTERFACE(BGuiPluginInterface, "BeQt.BGuiPluginInterface")

#endif // BGUIPLUGININTERFACE_H
