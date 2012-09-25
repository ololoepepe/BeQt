#ifndef BCORE_H
#define BCORE_H

class QSettings;
class QTranslator;
class QObject;
class QPluginLoader;

#include <QString>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QLocale>
#include <QDir>
#include <QVariantMap>
#include <QMutex>
#include <QPointer>

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

class BCORESHARED_EXPORT BCore : public QObject
{
    Q_OBJECT
public:
    //beqt
    static const QString BeQtVersion;
    static const QString ResourcesPath;
    static const QString IcoPath;
    static const QLocale DefaultLocale;
    //other:time
    static const int Second;
    static const int Minute;
    static const int Hour;
    //other:data
    static const int Kilobyte;
    static const int Megabyte;
    static const int Gigabyte;
    //
    //main
    static BCore *instance();
    static void init(bool noUserDir = false);
    //settings
    static QSettings *newSettingsInstance();
    static void saveSettings();
    static void loadSettings();
    static void setMultipleInstancesEnabled(bool enabled);
    static bool setLocale(const QLocale &l);
    static bool multipleInstancesEnabled();
    static const QLocale &currentLocale();
    //translations
    static void addTranslatorPath(const QString &path);
    static void addTranslatorPrefix(const QString &prefix);
    static QList<QLocale> availableLocales();
    static QString translationsDir(bool user = true);
    //paths and data
    static void setPath(const QString &key, const QString &path, bool file = false);
    static QString shared(const QString &key = QString(), bool file = false);
    static QString user(const QString &key = QString(), bool file = false);
    static QString beqtIcon(const QString &iconName);
    //plugins
    static void addPluginHandlingObject(QObject *object);
    static void removePluginHandlingObject(QObject *object);
    static void setPluginValidityChecker( bool (*function)(const QObject *) );
    static QList<QObject *> plugins();
    static QString pluginsDir(bool user = true);
    //filesystem
    static void createUserPath(const QString &key, bool file = false);
    static bool copyResource(const QString &key);
    static bool removeDir(const QString &path);
    static void copyDir(const QString &path, const QString &newPath, bool recursive = true);
    static QString localeBasedFileName( const QString &fileName, const QString &defaultFileName,
                                        const QString &possibleSuffix = QString() );
    static QString localeBasedDirName(const QString &dir);
private:
    static const bool _m_MultipleInstancesDef;
    static const QStringList _m_PluginSuffixes;
    //
    static const QString _m_GroupCore;
      static const QString _m_KeyLocale;
      static const QString _m_KeyMultipleInstances;
    //
    static BCore *_m_inst;
    static QMutex _m_instMutex;
    static bool _m_initialized;
    static QStringList _m_translatorPaths;
    static QStringList _m_translatorPrefixes;
    static QList<QTranslator *> _m_translators;
    static bool _m_multipleInstances;
    static QLocale _m_locale;
    static QString _m_sharedTranslationsDir;
    static QString _m_userTranslationsDir;
    static QString _m_sharedRoot;
    static QString _m_userRoot;
    static QMap<QString, QString> _m_dirMap;
    static QMap<QString, QString> _m_fileMap;
    static QString _m_sharedPluginsDir;
    static QString _m_userPluginsDir;
    static QList< QPointer<QObject> > _m_pluginHandlingObjects;
    static QMap<QString, QPluginLoader *> _m_pluginMap;
    static bool (*_m_pluginValidityChecker)(const QObject *);
    //
    static void _m_loadPlugin(const QString &fileName);
    static void _m_loadPlugins(const QString &dir);
    //
    BCore();
    BCore(const BCore &other);
    ~BCore();
    //
    BCore &operator=(const BCore &other);
    //
    void emitLocaleChanged();
signals:
    void localeChanged();
};

Q_DECLARE_METATYPE(QList<QObject *>)

#endif // BCORE_H
