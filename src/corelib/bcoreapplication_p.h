#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPluginWrapper;
class BTranslator;
class BPluginWrapperPrivate;
class BTranslatorPrivate;
class BPersonInfoProvider;
class BCoreApplicationPrivate;

class QString;
class QLocale;
class QSettings;
class QEvent;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QStringList>

/*============================================================================
================================ BCoreApplicationEventFilter =================
============================================================================*/

class B_CORE_EXPORT BCoreApplicationEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit BCoreApplicationEventFilter(BCoreApplicationPrivate *p);
    ~BCoreApplicationEventFilter();
public:
    bool eventFilter(QObject *o, QEvent *e);
public:
    BCoreApplicationPrivate *const _m_p;
};

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
    static const QString SettingsGroupBeqt;
      static const QString SettingsGroupCore;
        static const QString SettingsKeyDeactivatedPlugins;
        static const QString SettingsKeyLocale;
public:
    explicit BCoreApplicationPrivate(BCoreApplication *q);
    virtual ~BCoreApplicationPrivate();
public:
    static BCoreApplicationPrivate *instance();
    static QString toLowerNoSpaces(const QString &string);
    static QString subdir(BCoreApplication::Location loc);
    static bool testCoreInit(const char *where = 0);
    static bool testCoreUnique();
    static QSettings *createSettingsInstance(const QString &fileName);
    static BCoreApplication::LocaleSupportInfo createLocaleSupportInfo();
    static QString personInfoString(BPersonInfoProvider *prov);
public:
    void init();
    QString confFileName(const QString &path, const QString &name) const;
    QString prefix(BCoreApplication::ResourcesType type) const;
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitLanguageChange();
    void installTranslator(BTranslator *translator, bool languageChange);
    void removeTranslator(BTranslator *translator, bool languageChange);
    void loadSettings();
    void saveSettings();
public:
    bool initialized;
    QString appName;
    QString orgName;
    QString appPath;
    QString userPrefix;
    QString sharedPrefix;
#if defined(Q_OS_MAC)
    QString bundlePrefix;
#endif
    bool portable;
    QLocale locale;
    QStringList deactivatedPlugins;
    QMap<QString, BTranslator *> translators;
    BCoreApplicationEventFilter *appEventFilter;
    bool blockLanguageChange;
    QList<BPluginWrapper *> plugins;
    BPersonInfoProvider *beqtAuthors;
    BPersonInfoProvider *beqtTranslations;
    BPersonInfoProvider *beqtThanksTo;
private:
    Q_DISABLE_COPY(BCoreApplicationPrivate)
    friend class BPluginWrapperPrivate;
    friend class BTranslatorPrivate;
    friend class BCoreApplicationEventFilter;
};

#endif // BCOREAPPLICATION_P_H
