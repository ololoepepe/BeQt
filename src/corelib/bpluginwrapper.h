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

/*============================================================================
================================ BPluginWrapper ==============================
============================================================================*/

class B_CORE_EXPORT BPluginWrapper : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginWrapper)
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
public:
    explicit BPluginWrapper(QObject *parent = 0);
    explicit BPluginWrapper(const QString &fileName, QObject *parent = 0);
    ~BPluginWrapper();
protected:
    explicit BPluginWrapper(BPluginWrapperPrivate &d, QObject *parent = 0);
public:
    static void setAcceptableTypes(const QStringList &list);
    static void setInterfaceTestFunction(InterfaceTestFunction function);
    static QStringList acceptableFileTypes();
    static InterfaceTestFunction interfacetestFunction();
    static BPluginWrapper *parentWrapper(const BPluginInterface *i);
public:
    void setLoaded(bool b);
    void setActivated(bool b);
    void setFileName(const QString &fn);
    bool isLoaded() const;
    bool isActivated() const;
    QString fileName() const;
    QString type() const;
    QString name() const;
    bool prefereStaticInfo() const;
    BPluginInterface::PluginInfo info() const;
    BPluginInterface::PluginInfoStatic staticInfo() const;
    QObject *instance() const;
    BPluginInterface *interface() const;
    QSettings *settings() const;
public Q_SLOTS:
    bool load();
    void unload();
    bool activate();
    void deactivate();
Q_SIGNALS:
    void activated();
    void aboutToBeDeactivated();
private:
    Q_DISABLE_COPY(BPluginWrapper)
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGINWRAPPER_H
