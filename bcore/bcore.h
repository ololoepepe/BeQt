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

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

namespace BCore
{

enum Translator
{
    QtTranslator,
    BCoreTranslator,
    BGuiTranslator,
    BNetworkTranslator
};

//

#if defined(Q_OS_MAC)
const QStringList PluginSuffixes = QStringList() << "*.dylib";
#elif defined(Q_OS_UNIX)
const QStringList PluginSuffixes = QStringList() << "*.so";
#elif defined(Q_OS_WIN)
const QStringList PluginSuffixes = QStringList() << "*.dll";
#endif
const QString Sep = QDir::separator();
const QString Home = QDir::homePath();
const QString ResourcesPath = ":/beqt/res";
const QString IcoPath = ResourcesPath + "/ico";
const QString TranslationsPath = ResourcesPath + "/translations";
//
const int Second = 1000;
const int Minute = 60 * Second;
const int Hour = 60 * Minute;
//
const int Kilobyte = 1024;
const int Megabyte = 1024 * Kilobyte;
const int Gigabyte = 1024 * Megabyte;
//
const QString GeneralSettingsTabId = "general";
const QString GeneralSettingsTabIdLocale = "locale";
//
const QString BeQtVersion = "1.0.0";

//

BCORESHARED_EXPORT QSettings *newSettingsInstance();
BCORESHARED_EXPORT void saveSettings();
BCORESHARED_EXPORT void loadSettings();
BCORESHARED_EXPORT void applySettings(const QVariantMap &settings);
BCORESHARED_EXPORT void addStandardTranslator(Translator translator);
BCORESHARED_EXPORT void setStandardTranslators( const QList<Translator> &translators = QList<Translator>() );
BCORESHARED_EXPORT void addTranslator(const QString &path);
BCORESHARED_EXPORT void setTranslators( const QStringList &paths = QStringList() );
BCORESHARED_EXPORT void setSharedRoot(const QString &path);
BCORESHARED_EXPORT void setUserRoot(const QString &path);
BCORESHARED_EXPORT void setPath(const QString &key, const QString &path, bool file = false);
BCORESHARED_EXPORT void setData(const QString &key, const QVariant &data);
BCORESHARED_EXPORT void createUserPath(const QString &key, bool file = false);
BCORESHARED_EXPORT bool copyResource(const QString &key);
BCORESHARED_EXPORT bool setLocale(const QLocale &locale);
BCORESHARED_EXPORT const QLocale &locale();
BCORESHARED_EXPORT const QList<QLocale> availableLocales();
BCORESHARED_EXPORT QString standardTranslatorPath(Translator translator);
BCORESHARED_EXPORT QString shared(const QString &key = QString(), bool file = false);
BCORESHARED_EXPORT QString user(const QString &key = QString(), bool file = false);
BCORESHARED_EXPORT QVariant data( const QString &key, const QVariant &def = QVariant() );
BCORESHARED_EXPORT QString dataS( const QString &key, const QString &def = QString() );
BCORESHARED_EXPORT void loadPlugin(const QString &fileName);
BCORESHARED_EXPORT void loadPlugins(const QString &dir);
BCORESHARED_EXPORT QList<QObject *> plugins();
BCORESHARED_EXPORT void addPluginHandlingObject(QObject *object);
BCORESHARED_EXPORT void removePluginHandlingObject(QObject *object);
BCORESHARED_EXPORT void setPluginValidityChecker( bool (*function)(QObject *) );
BCORESHARED_EXPORT bool removeDir(const QString &path);
BCORESHARED_EXPORT void copyDir(const QString &path, const QString &newPath, bool recursive = true);

}

#endif // BCORE_H
