#ifndef BPLUGINWRAPPER_H
#define BPLUGINWRAPPER_H

class BPluginWrapperPrivate;

class QStringList;

#include "bnamespace.h"
#include "bplugininterface.h"

#include <QObject>
#include <QList>
#include <QString>

class B_CORE_EXPORT BPluginWrapper : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPluginWrapper)
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
    //
    explicit BPluginWrapper(const QString &fileName = QString(), QObject *parent = 0);
    ~BPluginWrapper();
    //
    void setActivated(bool b);
    void setFileName(const QString &fileName);
    void setAcceptableTypes(const QStringList &list);
    void setInterfaceTestFunction(InterfaceTestFunction function);
    bool isActivated() const;
    bool isValid() const;
    QString fileName() const;
    QStringList acceptableFileTypes() const;
    InterfaceTestFunction interfacetestFunction() const;
    QString type() const;
    QString name() const;
    BPluginInterface::PluginInfo info() const;
    QObject *instance() const;
    BPluginInterface *interface() const;
public slots:
    void activate();
    void deactivate();
signals:
    void activated();
    void aboutToBeDeactivated();
protected:
    BPluginWrapperPrivate *const _m_d;
private:
    Q_DISABLE_COPY(BPluginWrapper)
    //
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGINWRAPPER_H
