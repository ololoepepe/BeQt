#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPluginWrapper;
class BTranslator;
class BPluginWrapperPrivate;
class BTranslatorPrivate;
class BPersonInfoProvider;
class BCoreApplicationPrivate;
class BLogger;

class QString;
class QLocale;
class QEvent;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QVariantMap>
#include <QPointer>
#include <QSettings>

/*============================================================================
================================ BCoreApplicationPrivate =====================
============================================================================*/

class B_CORE_EXPORT BCoreApplicationPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCoreApplication)
    B_DECLARE_PUBLIC_S(BCoreApplication)
public:
    static const QStringList PluginSuffixes;
public:
    explicit BCoreApplicationPrivate(BCoreApplication *q);
    virtual ~BCoreApplicationPrivate();
public:
    static BCoreApplicationPrivate *instance();
    static QString toLowerNoSpaces(const QString &string);
    static QString subdir(BCoreApplication::Location loc);
    static bool testCoreInit(const char *where = 0);
    static bool testCoreUnique();
    static BCoreApplication::LocaleSupportInfo createLocaleSupportInfo();
    static QString personInfoString(BPersonInfoProvider *prov, const QLocale &loc, bool noDefault = false);
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    QString getAppName() const;
    QString getOrgName() const;
    QString getAppPath() const;
    QString getUserPrefix() const;
    QString getSharedPrefix() const;
#if defined(Q_OS_MAC)
    QString getBundlePrefix() const;
#endif
    bool getIsPortable() const;
    QLocale getLocale() const;
    QStringList getDeactivatedPlugins() const;
    void setDeactivatedPlugins(const QStringList &list);
    void addDeactivatedPlugin(const QString &pluginName);
    QSettings *createSettingsInstance(const QString &fileName) const;
    QString confFileName(const QString &path, const QString &name) const;
    QString prefix(BCoreApplication::ResourcesType type) const;
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitLanguageChange();
    void installTranslator(BTranslator *translator, bool blockLC);
    void removeTranslator(BTranslator *translator, bool blockLC);
    void loadSettings();
    void saveSettings();
public slots:
    void initSettings();
public:
    mutable QString appName;
    mutable QString orgName;
    mutable QString appPath;
    mutable QString userPrefix;
    mutable QString sharedPrefix;
#if defined(Q_OS_MAC)
    mutable QString bundlePrefix;
#endif
    QPointer<QSettings> settings;
    QMap<QString, BTranslator *> translators;
    bool blockLanguageChange;
    QList<BPluginWrapper *> plugins;
    BPersonInfoProvider *beqtAuthors;
    BPersonInfoProvider *beqtTranslations;
    BPersonInfoProvider *beqtThanksTo;
    BLogger *logger;
private:
    Q_DISABLE_COPY(BCoreApplicationPrivate)
    friend class BPluginWrapperPrivate;
    friend class BTranslatorPrivate;
    friend class BCoreApplicationEventFilter;
};

#endif // BCOREAPPLICATION_P_H
