#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;
class BTranslator;
class BTranslatorPrivate;
class BPluginWrapperPrivate;
class BDirToolsPrivate;

class QLocale;
class QSettings;

#include "bglobal.h"
#include "bpluginwrapper.h"
#include "bbase.h"

#include <QObject>
#include <QLocale>
#include <QList>
#include <QString>
#include <QStringList>

#define bApp BCoreApplication::instance()

class B_CORE_EXPORT BCoreApplication : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BCoreApplication)
    B_DECLARE_PRIVATE_S(BCoreApplication)
    Q_OBJECT
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
#if defined(B_OS_MAC)
        BundleResources,
#endif
        BuiltinResources
    };
    enum BeQtInfo
    {
        Description,
        ChangeLog,
        License
    };
    //
    struct LocaleSupportInfo
    {
        QLocale locale;
        int supports;
        int total;
        //
        LocaleSupportInfo()
        {
            locale = QLocale(QLocale::English);
            supports = 0;
            total = 0;
        }
    };
    //
    static BCoreApplication *instance();
    static QString location(Location loc, ResourcesType type);
    static QString location(const QString &subdir, ResourcesType type);
    static QStringList locations(Location loc);
    static QStringList locations(const QString &subdir);
    static QSettings *createAppSettingsInstance(bool createFile = true);
    static void registerPluginWrapper(BPluginWrapper *plugin);
    static void unregisterPluginWrapper(BPluginWrapper *plugin);
    static void loadPlugins(const QStringList &acceptableTypes = QStringList(),
                            BPluginWrapper::InterfaceTestFunction function = 0, bool reload = false);
    static QList<BPluginWrapper *> pluginWrappers( const QString &type = QString() );
    static void installTranslator(BTranslator *translator, bool noLanguageChange = false);
    static void removeTranslator(BTranslator *translator, bool noLanguageChange = false);
    static void setLocale(const QLocale &l, bool noRetranslate = false);
    static QLocale locale();
    static QList<LocaleSupportInfo> availableLocales(bool alwaysIncludeEnglish = false);
    static void retranslateUi();
    static void loadSettings();
    static void saveSettings();
    static QString beqtInfo(BeQtInfo type);
    //
    explicit BCoreApplication();
    ~BCoreApplication();
signals:
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void languageChanged();
    void settingsLoaded(QSettings *s);
    void settingsSaved(QSettings *s);
protected:
    static BCoreApplication *_m_self;
    //
    BCoreApplication(BCoreApplicationPrivate &d);
private:
    Q_DISABLE_COPY(BCoreApplication)
    //
    friend class BTranslatorPrivate;
    friend class BPluginWrapperPrivate;
    friend class BDirToolsPrivate;
};

#endif // BCOREAPPLICATION_H
