#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;
class BPlugin;
class BTranslator;
class BTranslatorPrivate;
class BPluginPrivate;

class QLocale;
class QSettings;

#include <BeQt>

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
    enum LocaleSupport
    {
        LS_No = 0,
        LS_Partial,
        LS_Good,
        LS_Full
    };
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
    static BCoreApplication *instance();
    static QString location(Location loc, ResourcesType type);
    static QString location(const QString &subdir, ResourcesType type);
    static QStringList locations(Location loc);
    static QStringList locations(const QString &subdir);
    static void createUserLocation(Location loc);
    static void createUserLocation(const QString &subdir);
    static void createUserLocations(const QStringList &subdirs);
    static QSettings *createAppSettingsInstance(bool createFile = true);
    static void loadPlugins(const QStringList &acceptableTypes = QStringList(),
                            bool (*testFunction)(const QObject *) = 0, bool reload = false);
    static BPlugin *plugin(const QString &name);
    static QList<BPlugin *> plugins( const QString &type = QString() );
    static void installTranslator(BTranslator *translator);
    static void removeTranslator(BTranslator *translator);
    static void setLocale(const QLocale &l);
    static QLocale locale();
    static QList<QLocale> availableLocales();
    static LocaleSupport localeSupport();
    static void retranslateUi();
    static void saveSettings();
    //
    explicit BCoreApplication( const AppOptions &options = AppOptions() );
    ~BCoreApplication();
signals:
    void pluginActivated(BPlugin *plugin);
    void pluginAboutToBeDeactivated(BPlugin *plugin);
    void languageChanged();
protected:
    BCoreApplicationPrivate *_m_d;
private:
    Q_DISABLE_COPY(BCoreApplication)
    //
    static BCoreApplication *_m_self;
    //
    friend class BTranslatorPrivate;
    friend class BPluginPrivate;
};

#endif // BCOREAPPLICATION_H
