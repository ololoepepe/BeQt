#ifndef BGUIPLUGININTERFACE_H
#define BGUIPLUGININTERFACE_H

class BAbstractSettingsTab;

class QPixmap;
class QVariantMap;

#include <BeQtCore/BeQt>

#include <QtPlugin>

class B_WIDGETS_EXPORT BGuiPluginInterface
{
public:
    virtual ~BGuiPluginInterface() {}
    //
    virtual QPixmap pixmap() const = 0;
    virtual BAbstractSettingsTab *settingsTab() const;
    virtual void handleSettings(const QVariantMap &s);
};

Q_DECLARE_INTERFACE(BGuiPluginInterface, "BeQt.BGuiPluginInterface")

#endif // BGUIPLUGININTERFACE_H
