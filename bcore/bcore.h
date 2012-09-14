#ifndef BCORE_H
#define BCORE_H

class QSettings;
class QTranslator;
class QObject;

#include <QString>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QLocale>
#include <QDir>
#include <QVariantMap>
#include <QMutex>

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

class BCORESHARED_EXPORT BCore
{
public:
    enum Translator
    {
        QtTranslator,
        BCoreTranslator,
        BGuiTranslator,
        BNetworkTranslator
    };
    //beqt
    static const QString BeQtVersion;
    static const QString ResourcesPath;
    static const QString IcoPath;
    static const QString TranslationsPath;
    //other:time
    static const int Second;
    static const int Minute;
    static const int Hour;
    //other:data
    static const int Kilobyte;
    static const int Megabyte;
    static const int Gigabyte;
    //
    static BCore *instance();
    //settings
    static QSettings *newSettingsInstance();
    static void saveSettings();
    static void loadSettings();
    //translation
    static void addStandardTranslator(Translator translator);
    static void setStandardTranslators( const QList<Translator> &translators = QList<Translator>() );
    static void addTranslator(const QString &path);
    static void setTranslators( const QStringList &paths = QStringList() );
    static bool setLocale(const QLocale &l);
    static const QLocale &currentLocale();
    static QList<QLocale> availableLocales();
    static QString standardTranslatorPath(Translator translator);
    //paths and data
    static void setSharedRoot(const QString &path);
    static void setUserRoot(const QString &path);
    static void setPath(const QString &key, const QString &path, bool file = false);
    static void setData(const QString &key, const QVariant &data);
    static QString shared(const QString &key = QString(), bool file = false);
    static QString user(const QString &key = QString(), bool file = false);
    static QVariant data( const QString &key, const QVariant &def = QVariant() );
    static QString dataS( const QString &key, const QString &def = QString() );
    //plugins
    static void loadPlugin(const QString &fileName);
    static void loadPlugins(const QString &dir);
    static QList<QObject *> plugins();
    static void addPluginHandlingObject(QObject *object);
    static void removePluginHandlingObject(QObject *object);
    static void setPluginValidityChecker( bool (*function)(QObject *) );
    //filesystem
    static void createUserPath(const QString &key, bool file = false);
    static bool copyResource(const QString &key);
    static bool removeDir(const QString &path);
    static void copyDir(const QString &path, const QString &newPath, bool recursive = true);
private:
    BCore();
    BCore(const BCore &other);
    ~BCore();
    //
    BCore &operator=(const BCore &other);
};

#endif // BCORE_H
