#ifndef BPLUGININTERFACE_H
#define BPLUGININTERFACE_H

class QObject;

#include <QtPlugin>

class BPluginInterface
{
public:
    virtual ~BPluginInterface() {}
    //
    virtual QString uniqueId() const = 0;
    virtual QString title() const = 0;
    virtual QString version() const = 0;
    virtual QString copyright() const = 0;
    virtual QString about() const = 0;
    virtual QString www() const = 0;
    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void handleLoad(QObject *object) = 0;
    virtual void handleUnload(QObject *object) = 0;
    virtual QString translatorPrefix() const = 0;
    virtual QString translatorPath() const = 0;
};

Q_DECLARE_INTERFACE(BPluginInterface, "BeQt.BPluginInterface")

#endif // BPLUGININTERFACE_H
