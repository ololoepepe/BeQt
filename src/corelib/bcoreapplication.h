#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;
class BTranslator;
class BTranslatorPrivate;
class BPluginWrapperPrivate;

class QLocale;
class QSettings;

#include <BeQt>
#include <BPluginWrapper>

#include <QObject>
#include <QLocale>
#include <QList>
#include <QString>
#include <QStringList>

class B_CORE_EXPORT BCoreApplication : public QObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCoreApplication)
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
        SharedResources
#if defined(B_OS_MAC)
        , BundleResources
#endif
    };
    //
    struct AppOptions
    {
        bool noSettingsDir;
        QLocale defaultLocale;
        //
        AppOptions()
        {
            noSettingsDir = false;
            defaultLocale = QLocale::system();
        }
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
    static void createUserLocation(Location loc);
    static void createUserLocation(const QString &subdir);
    static void createUserLocations(const QStringList &subdirs);
    static QSettings *createAppSettingsInstance(bool createFile = true);
    static void registerPluginWrapper(BPluginWrapper *plugin);
    static void unregisterPluginWrapper(BPluginWrapper *plugin);
    static void loadPlugins(const QStringList &acceptableTypes = QStringList(),
                            BPluginWrapper::InterfaceTestFunction function = 0, bool reload = false);
    static QList<BPluginWrapper *> pluginWrappers( const QString &type = QString() );
    static void installTranslator(BTranslator *translator, bool noLanguageChange = false);
    static void removeTranslator(BTranslator *translator, bool noLanguageChange = false);
    static void setLocale(const QLocale &l);
    static QLocale locale();
    static QList<LocaleSupportInfo> availableLocales(bool alwaysIncludeEnglish = false);
    static void retranslateUi();
    static void loadSettings();
    static void saveSettings();
    //
    explicit BCoreApplication( const AppOptions &options = AppOptions() );
    ~BCoreApplication();
signals:
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void languageChanged();
    void settingsLoaded();
    void settingsSaved();
protected:
    BCoreApplicationPrivate *_m_d;
private:
    Q_DISABLE_COPY(BCoreApplication)
    //
    static BCoreApplication *_m_self;
    //
    friend class BTranslatorPrivate;
    friend class BPluginWrapperPrivate;
};

#endif // BCOREAPPLICATION_H
