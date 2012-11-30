#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;
class BTranslator;
class BTranslatorPrivate;
class BPluginWrapperPrivate;
class BDirToolsPrivate;
class BPluginWrapper;

class QLocale;
class QSettings;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>
#include <QLocale>
#include <QList>
#include <QString>
#include <QStringList>

#define bApp BCoreApplication::instance()

/*============================================================================
================================ BCoreApplication ============================
============================================================================*/

class B_CORE_EXPORT BCoreApplication : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCoreApplication)
    B_DECLARE_PRIVATE_S(BCoreApplication)
public:
    enum Location
    {
        DataPath,
        DocumentationPath,
        PluginsPath,
        SettingsPath,
        TranslationsPath,
        BeqtPath
    };
    enum ResourcesType
    {
        UserResources,
        SharedResources,
#if defined(Q_OS_MAC)
        BundleResources,
#endif
        BuiltinResources
    };
    enum BeQtInfo
    {
        Description,
        ChangeLog,
        License,
        Authors,
        Translators,
        ThanksTo
    };
public:
    struct LocaleSupportInfo
    {
        QLocale locale;
        int supports;
        int total;
    };
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
public:
    explicit BCoreApplication();
    ~BCoreApplication();
protected:
    explicit BCoreApplication(BCoreApplicationPrivate &d);
public:
    static BCoreApplication *instance();
    static QString location(Location loc, ResourcesType type);
    static QString location(const QString &subdir, ResourcesType type);
    static QStringList locations(Location loc);
    static QStringList locations(const QString &subdir);
    static QSettings *createAppSettingsInstance();
    static void registerPluginWrapper(BPluginWrapper *plugin);
    static void unregisterPluginWrapper(BPluginWrapper *plugin);
    static void loadPlugins(const QStringList &acceptableTypes = QStringList(), InterfaceTestFunction function = 0);
    static QList<BPluginWrapper *> pluginWrappers( const QString &type = QString() );
    static void installTranslator(BTranslator *translator, bool blockLanguageChange = true);
    static void removeTranslator(BTranslator *translator, bool blockLanguageChange = true);
    static void setLocale(const QLocale &l, bool noRetranslate = false);
    static QLocale locale();
    static QList<LocaleSupportInfo> availableLocales(bool alwaysIncludeEnglish = false);
    static void retranslateUi(bool blockLanguageChange = true);
    static void loadSettings();
    static void saveSettings();
    static QString beqtInfo( BeQtInfo type, const QLocale &loc = locale() );
signals:
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void languageChanged();
    void settingsLoaded(QSettings *s);
    void settingsSaved(QSettings *s);
protected:
    static BCoreApplication *_m_self;
private:
    Q_DISABLE_COPY(BCoreApplication)
    friend class BTranslatorPrivate;
    friend class BPluginWrapperPrivate;
    friend class BDirToolsPrivate;
};

#endif // BCOREAPPLICATION_H
