#ifndef BPLUGIN_H
#define BPLUGIN_H

class BPluginPrivate;

#include <BeQt>

#include <QObject>

class B_CORE_EXPORT BPlugin : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(BPlugin)
public:
    explicit BPlugin(QObject *parent = 0);
    ~BPlugin();
private:
    Q_DISABLE_COPY(BPlugin)
};

#endif // BPLUGIN_H
