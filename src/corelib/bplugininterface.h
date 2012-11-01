#ifndef BPLUGININTERFACE_H
#define BPLUGININTERFACE_H

#include <BeQt>

#include <QtPlugin>

class B_CORE_EXPORT BPluginInterface
{
public:
    virtual ~BPluginInterface() {}
};

Q_DECLARE_INTERFACE(BPluginInterface, "BeQt.BPluginInterface")

#endif // BPLUGININTERFACE_H
