#ifndef BPLUGIN_H
#define BPLUGIN_H

class BPluginPrivate;
class BCoreApplication;

class QString;
class QLocale;

#include <BeQt>
#include <BPluginInterface>

#include <QObject>
#include <QList>

class B_CORE_EXPORT BPlugin : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BPlugin)
public:
    enum PluginState
    {
        NotInitialized = 0,
        Activated,
        Deactivated
    };
    //
    explicit BPlugin(const QString &fileName, QObject *parent = 0);
    ~BPlugin();
    //
    void setActivated(bool b);
    void setLocale(const QLocale &l);
    void reloadTranslator();
    PluginState state() const;
    bool isValid() const;
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
    void deactivated();
protected:
    BPluginPrivate *_m_d;
private:
    Q_DISABLE_COPY(BPlugin)
    //
    friend class BCoreApplication;
    friend class BCoreApplicationPrivate;
};

#endif // BPLUGIN_H
