#ifndef MYPLUGIN_H
#define MYPLUGIN_H

#include <BPluginInterface>
#include <BGuiPluginInterface>

#include <QObject>

class MyPlugin : public QObject, public BPluginInterface, public BGuiPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(BPluginInterface)
    Q_INTERFACES(BGuiPluginInterface)
public:
    MyPlugin();
    ~MyPlugin();
    //
    virtual QString type() const;
    virtual QString name() const;
    virtual PluginInfo info() const;
    virtual void activate();
    virtual void deactivate();
    virtual QPixmap pixmap() const;
    virtual BAbstractSettingsTab *settingsTab() const;
    virtual void handleSettings(const QVariantMap &s);
private:
    QString firstName;
};

#endif // MYPLUGIN_H
