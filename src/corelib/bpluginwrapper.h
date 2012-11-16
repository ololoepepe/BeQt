#ifndef BPLUGINWRAPPER_H
#define BPLUGINWRAPPER_H

class BPluginWrapperPrivate;

class QStringList;
class QSettings;

#include "bglobal.h"
#include "bplugininterface.h"
#include "bbase.h"

#include <QObject>
#include <QList>
#include <QString>

class B_CORE_EXPORT BPluginWrapper : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginWrapper)
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
    //
    static QSettings *createPluginSettingsInstance(const QString &pluginName, bool createFile = true);
    //
    explicit BPluginWrapper(QObject *parent = 0);
    explicit BPluginWrapper(const QString &fileName, QObject *parent = 0);
    ~BPluginWrapper();
    //
    void setLoaded(bool b);
    void setActivated(bool b);
    void setFileName(const QString &fn);
    void setAcceptableTypes(const QStringList &list);
    void setInterfaceTestFunction(InterfaceTestFunction function);
    bool isLoaded() const;
    bool isActivated() const;
    QString fileName() const;
    QStringList acceptableFileTypes() const;
    InterfaceTestFunction interfacetestFunction() const;
    QString type() const;
    QString name() const;
    BPluginInterface::PluginInfo info() const;
    QObject *instance() const;
    BPluginInterface *interface() const;
public slots:
    bool load();
    void unload();
    bool activate();
    void deactivate();
signals:
    void activated();
    void aboutToBeDeactivated();
protected:
    BPluginWrapper(BPluginWrapperPrivate &d, QObject *parent = 0);
private:
    Q_DISABLE_COPY(BPluginWrapper)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGINWRAPPER_H
