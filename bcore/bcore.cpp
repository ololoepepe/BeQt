#include "bcore.h"
#include "bplugininterface.h"

#include <QList>
#include <QTranslator>
#include <QMap>
#include <QCoreApplication>
#include <QString>
#include <QVariantMap>
#include <QLocale>
#include <QDir>
#include <QSettings>
#include <QObject>
#include <QPluginLoader>
#include <QPointer>
#include <QFileInfo>
#include <QScopedPointer>
#include <QMutex>
#include <QMutexLocker>

#include <QDebug>

#if defined(Q_OS_MAC)
const QStringList PluginSuffixes = QStringList() << "*.dylib";
#elif defined(Q_OS_UNIX)
const QStringList PluginSuffixes = QStringList() << "*.so";
#elif defined(Q_OS_WIN)
const QStringList PluginSuffixes = QStringList() << "*.dll";
#endif
//
const QString GroupCore = "beqt_core";
  const QString KeyLocale = "locale";

//

BCore *inst = 0;
QMutex mutex;
QStringList translatorPaths;
QList<QTranslator *> translators;
QLocale locale = BCore::DefaultLocale;
QString sharedRoot;
QString userRoot;
QMap<QString, QString> dirMap;
QMap<QString, QString> fileMap;
QVariantMap dataMap;
QList< QPointer<QObject> > pluginHandlingObjects;
QMap<QString, QPluginLoader *> pluginMap;
bool (*pluginValidityChecker)(const QObject *) = 0;

//

//beqt
const QString BCore::BeQtVersion = "1.0.0";
const QString BCore::ResourcesPath = ":/beqt/res";
const QString BCore::IcoPath = BCore::ResourcesPath + "/ico";
const QString BCore::TranslationsPath = BCore::ResourcesPath + "/translations";
const QLocale BCore::DefaultLocale = QLocale(QLocale::English, QLocale::UnitedStates);
//other:time
const int BCore::Second = 1000;
const int BCore::Minute = 60 * BCore::Second;
const int BCore::Hour = 60 * BCore::Minute;
//other:data
const int BCore::Kilobyte = 1024;
const int BCore::Megabyte = 1024 * BCore::Kilobyte;
const int BCore::Gigabyte = 1024 * BCore::Megabyte;

//

BCore *BCore::instance()
{
    if (inst)
        return inst;
    QMutexLocker locker(&mutex);
    if (inst)
        return inst;
    inst = new BCore;
    return inst;
}

//settings

QSettings *BCore::newSettingsInstance()
{
    QString fn = QCoreApplication::applicationDirPath() + "/settings.ini";
    return QFile(fn).exists() ? new QSettings(fn, QSettings::IniFormat) : new QSettings;
}

void BCore::saveSettings()
{
    QScopedPointer<QSettings> s( newSettingsInstance() );
    if (!s)
        return;
    s->remove(GroupCore);
    s->beginGroup(GroupCore);
      s->setValue(KeyLocale, locale);
    s->endGroup();
    //
    QList<QPluginLoader *> loaders = pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
        qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->saveSettings();
}

void BCore::loadSettings()
{
    QScopedPointer<QSettings> s( newSettingsInstance() );
    if (!s)
        return;
    s->beginGroup(GroupCore);
      QLocale l = s->value( KeyLocale, QLocale::system() ).toLocale();
      if ( QLocale::system() == l && !availableLocales().contains(l) )
          l = QLocale(QLocale::system().language(), QLocale::AnyCountry);
      setLocale(l);
    s->endGroup();
    //
    QList<QPluginLoader *> loaders = pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
        qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->loadSettings();
}

//translation

void BCore::addStandardTranslator(Translator translator)
{
    addTranslator( standardTranslatorPath(translator) );
}

void BCore::setStandardTranslators(const QList<Translator> &translators)
{
    for (int i = 0; i < translators.size(); ++i)
        addTranslator( standardTranslatorPath( translators.at(i) ) );
}

void BCore::addTranslator(const QString &path)
{
    if ( path.isEmpty() || translatorPaths.contains(path) )
        return;
    translatorPaths << path;
}

void BCore::setTranslators(const QStringList &paths)
{
    translatorPaths = paths;
}

bool BCore::setLocale(const QLocale &l)
{
    if ( !availableLocales().contains(l) )
        return false;
    if (l == locale)
        return true;
    for (int i = 0; i < translators.size(); ++i)
    {
        QTranslator *t = translators.at(i);
        QCoreApplication::removeTranslator(t);
        t->deleteLater();
    }
    translators.clear();
    locale = l;
    if (DefaultLocale == locale)
        return true;
    QString ln = locale.name();
    for (int i = 0; i < translatorPaths.size(); ++i)
    {
        const QString &path = translatorPaths.at(i);
        QTranslator *t = new QTranslator;
        if ( !t->load(path + "_" + ln) && !t->load( path + "_" + ln.left(2) ) )
        {
            t->deleteLater();
        }
        else
        {
            QCoreApplication::installTranslator(t);
            translators << t;
        }
    }
    return true;
}

const QLocale &BCore::currentLocale()
{
    return locale;
}

QList<QLocale> BCore::availableLocales()
{
    QLocale::setDefault(DefaultLocale);
    QMap<QString, QStringList> m;
    for (int i = 0; i < translatorPaths.size(); ++i)
    {
        QFileInfo fi( translatorPaths.at(i) );
        QString name = fi.fileName();
        QStringList &sl = m[fi.path()];
        if ( !sl.contains(name) )
            sl << name;
    }
    QStringList files;
    QStringList keys = m.keys();
    for (int i = 0; i < keys.size(); ++i)
    {
        const QString &key = keys.at(i);
        QStringList &sl = m[key];
        for (int i = 0; i < sl.size(); ++i)
            sl[i].append("_*.qm");
        files << QDir(key).entryList(sl, QDir::Files);
    }
    QList<QLocale> locales;
    locales << DefaultLocale;
    for (int i = 0; i < files.size(); ++i)
    {
        QString &file = files[i];
        file.remove(file.length() - 3, 3);
        int j = 0;
        while (j < file.length() && file.at(j) != '_')
            ++j;
        file.remove(0, j + 1);
        if (file.length() == 2 || file.length() == 5 || file.length() == 6)
        {
            QLocale l(file);
            if ( !locales.contains(l) )
                locales << l;
        }
    }
    return locales;
}

QString BCore::standardTranslatorPath(Translator translator)
{
    switch (translator)
    {
    case BCoreTranslator:
        return TranslationsPath + "/bcore";
    case BGuiTranslator:
        return TranslationsPath + "/bgui";
    case BNetworkTranslator:
        return TranslationsPath + "/bnetwork";
    case QtTranslator:
        return TranslationsPath + "/qt";
    default:
        return "";
    }
}

//paths and data

void BCore::setSharedRoot(const QString &path)
{
    sharedRoot = path;
}

void BCore::setUserRoot(const QString &path)
{
    userRoot = path;
}

void BCore::setPath(const QString &key, const QString &path, bool file)
{
    if ( key.isEmpty() )
        return;
    if (file)
        fileMap[key] = path;
    else
        dirMap[key] = path;
}

void BCore::setData(const QString &key, const QVariant &data)
{
    dataMap[key] = data;
}

QString BCore::shared(const QString &key, bool file)
{
    if ( key.isEmpty() )
        return sharedRoot;
    if ( file && fileMap.contains(key) )
        return sharedRoot + "/" + fileMap.value(key);
    else if ( !file && dirMap.contains(key) )
        return sharedRoot + "/" + dirMap.value(key);
    else
        return "";
}

QString BCore::user(const QString &key, bool file)
{
    if ( key.isEmpty() )
        return userRoot;
    if ( file && fileMap.contains(key) )
        return userRoot + "/" + fileMap.value(key);
    else if ( !file && dirMap.contains(key) )
        return userRoot + "/" + dirMap.value(key);
    else
        return "";
}

QVariant BCore::data(const QString &key, const QVariant &def)
{
    return dataMap.value(key, def);
}

QString BCore::dataS(const QString &key, const QString &def)
{
    return data( key, QVariant(def) ).toString();
}

QString BCore::beqtIcon(const QString &iconName)
{
    return IcoPath + "/" + (QFileInfo(iconName).suffix().isEmpty() ? iconName + ".png" : iconName);
}

//plugins

void BCore::loadPlugin(const QString &fileName)
{
    pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    if ( fileName.isEmpty() || !PluginSuffixes.contains( "*." + QFileInfo(fileName).suffix() ) )
        return;
    if ( pluginMap.contains(fileName) )
        return;
    QPluginLoader *pl = new QPluginLoader(fileName);
    if ( !pl->load() )
        return pl->deleteLater();
    QObject *plugin = pl->instance();
    BPluginInterface *interface = qobject_cast<BPluginInterface *>(plugin);
    if (!interface)
    {
        pl->unload();
        return pl->deleteLater();
    }
    QString id = interface->uniqueId();
    QList<QPluginLoader *> loaders = pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
    {
        if (qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->uniqueId() == id)
        {
            pl->unload();
            return pl->deleteLater();
        }
    }
    if ( pluginValidityChecker && !pluginValidityChecker(plugin) )
    {
        pl->unload();
        return pl->deleteLater();
    }
    addTranslator( interface->translatorPath() );
    pluginMap.insert(fileName, pl);
    for (int i = 0; i < pluginHandlingObjects.size(); ++i)
        interface->handleLoad( pluginHandlingObjects.at(i).data() );
}

void BCore::loadPlugins(const QString &dir)
{
    QDir d(dir);
    QStringList list = d.entryList(PluginSuffixes, QDir::Files);
    for (int i = 0; i < list.size(); ++i)
        loadPlugin( d.absoluteFilePath( list.at(i) ) );
}

QList<QObject *> BCore::plugins()
{
    QList<QPluginLoader *> loaders = pluginMap.values();
    QList<QObject *> list;
    for (int i = 0; i < loaders.size(); ++i)
        list << loaders.at(i)->instance();
    return list;
}

void BCore::addPluginHandlingObject(QObject *object)
{
    pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    QPointer<QObject> op(object);
    if ( !op || pluginHandlingObjects.contains(op) )
        return;
    pluginHandlingObjects << op;
    QList<QPluginLoader *> values = pluginMap.values();
    for (int i = 0; i < values.size(); ++i)
    {
        BPluginInterface *interface = qobject_cast<BPluginInterface *>( values.at(i)->instance() );
        if (interface)
            interface->handleLoad( op.data() );
    }
}

void BCore::removePluginHandlingObject(QObject *object)
{
    pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    QPointer<QObject> op(object);
    if ( !op || !pluginHandlingObjects.contains(op) )
        return;
    pluginHandlingObjects.removeAll(op);
    QList<QPluginLoader *> values = pluginMap.values();
    for (int i = 0; i < values.size(); ++i)
    {
        BPluginInterface *interface = qobject_cast<BPluginInterface *>( values.at(i)->instance() );
        if (interface)
            interface->handleUnload( op.data() );
    }
}

void BCore::setPluginValidityChecker( bool (*function)(const QObject *) )
{
    pluginValidityChecker = function;
}

//filesystem

void BCore::createUserPath(const QString &key, bool file)
{
    if (file)
    {
        if ( !fileMap.contains(key) )
            return;
        QFile f( user(key, true) );
        f.open(QFile::WriteOnly);
        f.close();
    }
    else
    {
        if ( !dirMap.contains(key) )
            return;
        QString path = user(key);
        QDir(path).mkpath(path);
    }
}

bool BCore::copyResource(const QString &key)
{
    QString sfn = shared(key, true);
    QString ufn = user(key, true);
    if ( sfn.isEmpty() || ufn.isEmpty() )
        return false;
    QFile sf(sfn);
    if ( !sf.exists() || QFile(ufn).exists() )
        return false;
    QString ufnd = QFileInfo(ufn).path();
    QDir d(ufnd);
    if ( !d.exists() && !d.mkpath(ufnd) )
        return false;
    return sf.copy(ufn);
}

bool BCore::removeDir(const QString &path)
{
    QDir d(path);
    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); ++i)
        if ( !removeDir( path + "/" + dirs.at(i) ) )
            return false;
    QStringList files = d.entryList(QDir::Files);
    for (int i = 0; i < files.size(); ++i)
        if ( !QFile( path + "/" + files.at(i) ).remove() )
            return false;
    return d.rmdir(path);
}

void BCore::copyDir(const QString &path, const QString &newPath, bool recursive)
{
    QDir d(path);
    QDir nd(newPath);
    if ( !d.exists() || !nd.mkpath(newPath) )
        return;
    QStringList files = d.entryList(QDir::Files);
    for (int i = 0; i < files.size(); ++i)
    {
        const QString &bfn = files.at(i);
        QFile::copy(path + "/" + bfn, newPath + "/" + bfn);
    }
    if (recursive)
    {
        QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i = 0; i < dirs.size(); ++i)
        {
            QString bdn = dirs.at(i);
            copyDir(path + "/" + bdn, newPath + "/" + bdn);
        }
    }
}

//

BCore::BCore()
{
    //
}
