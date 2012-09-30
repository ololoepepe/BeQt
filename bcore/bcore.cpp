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

#include <QDebug>

//beqt
const QString BCore::BeQtVersion = "1.0.0b1";
const QString BCore::ResourcesPath = ":/beqt/res";
const QString BCore::IcoPath = BCore::ResourcesPath + "/ico";
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

const bool BCore::_m_MultipleInstancesDef = true;
#if defined(Q_OS_MAC)
const QStringList BCore::_m_PluginSuffixes = QStringList() << "*.dylib";
#elif defined(Q_OS_UNIX)
const QStringList BCore::_m_PluginSuffixes = QStringList() << "*.so";
#elif defined(Q_OS_WIN)
const QStringList BCore::_m_PluginSuffixes = QStringList() << "*.dll";
#endif
//
const QString BCore::_m_GroupCore = "beqt_core";
  const QString BCore::_m_KeyLocale = "locale";
  const QString BCore::_m_KeyMultipleInstances = "multiple_instances";

//


BCore *BCore::_m_inst = 0;
QMutex BCore::_m_instMutex;
bool BCore::_m_initialized = false;
QStringList BCore::_m_translatorPaths;
QStringList BCore::_m_translatorPrefixes;
QList<QTranslator *> BCore::_m_translators;
bool BCore::_m_multipleInstances = BCore::_m_MultipleInstancesDef;
QLocale BCore::_m_locale = BCore::BCore::DefaultLocale;
QString BCore::_m_sharedTranslationsDir;
QString BCore::_m_userTranslationsDir;
QString BCore::_m_sharedRoot;
QString BCore::_m_userRoot;
QMap<QString, QString> BCore::_m_dirMap;
QMap<QString, QString> BCore::_m_fileMap;
QString BCore::_m_sharedPluginsDir;
QString BCore::_m_userPluginsDir;
QString BCore::_m_sharedDocsDir;
QList< QPointer<QObject> > BCore::_m_pluginHandlingObjects;
QMap<QString, QPluginLoader *> BCore::_m_pluginMap;
bool (*BCore::_m_pluginValidityChecker)(const QObject *) = 0;

//

//main

BCore *BCore::instance()
{
    if (!_m_inst)
    {
        _m_instMutex.lock();
        if (!_m_inst)
            _m_inst = new BCore;
        _m_instMutex.unlock();
    }
    return _m_inst;
}

void BCore::init(bool noUserDir)
{
    //Checking conditions
    if (_m_initialized)
    {
        qDebug() << tr("BCore is already initialized", "debug");
        return;
    }
    QString appname = QCoreApplication::applicationName();
    if ( appname.isEmpty() )
    {
        qDebug() << tr("Error: application name must not be empty", "debug");
        return;
    }
    QString appdir = QCoreApplication::applicationDirPath();
    if ( appdir.isEmpty() )
    {
        qDebug() << tr("Failed to get application directory path", "debug");
        return;
    }
#if defined(Q_OS_WIN)
    if ( !QCoreApplication::instance() )
    {
        qDebug() << tr("Error: no QCoreApplication instance", "debug");
        return;
    }
#endif
    //TODO: add native paths for Mac OS
    //Setting directories
    QString appnameUnix = appname.toLower().replace(' ', '-');
#if defined(Q_OS_UNIX)
    _m_sharedRoot = "/usr/share/" + appnameUnix;
    _m_userRoot = QDir::homePath() + "/." + appnameUnix;
    _m_sharedPluginsDir = "/usr/lib/" + appnameUnix + "/plugins";
    _m_sharedDocsDir = "/usr/share/doc/" + appnameUnix;
#elif defined(Q_OS_WIN)
    _m_sharedRoot = appdir;
    _m_userRoot = QDir::homePath() + "/" + appname;
    _m_sharedPluginsDir = appdir + "/plugins";
    _m_sharedDocsDir = appdir + "/doc";
#endif
    _m_sharedTranslationsDir = _m_sharedRoot + "/translations";
    _m_userPluginsDir = _m_userRoot + "/plugins";
    _m_userTranslationsDir = _m_userRoot + "/translations";
    if (!noUserDir)
    {
        QDir(_m_userPluginsDir).mkpath(_m_userPluginsDir);
        QDir(_m_userTranslationsDir).mkpath(_m_userTranslationsDir);
    }
    //Adding translator prefixes and paths
    addTranslatorPrefix("qt");
    addTranslatorPrefix("beqt");
    addTranslatorPrefix(appnameUnix);
    addTranslatorPath(_m_sharedTranslationsDir);
    if (!noUserDir)
        addTranslatorPath(_m_userTranslationsDir);
    //Loading plugins
    _m_loadPlugins(_m_userPluginsDir);
    _m_loadPlugins(_m_sharedPluginsDir);
    //Setting initialized state
    _m_initialized = true;
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
    s->remove(_m_GroupCore);
    s->beginGroup(_m_GroupCore);
      s->setValue(_m_KeyLocale, _m_locale);
      s->setValue(_m_KeyMultipleInstances, _m_multipleInstances);
    s->endGroup();
    //
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
        qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->saveSettings();
}

void BCore::loadSettings()
{
    QScopedPointer<QSettings> s( newSettingsInstance() );
    if (!s)
        return;
    s->beginGroup(_m_GroupCore);
      QLocale l = s->value( _m_KeyLocale, QLocale::system() ).toLocale();
      if ( QLocale::system() == l && !availableLocales().contains(l) )
          l = QLocale(QLocale::system().language(), QLocale::AnyCountry);
      setLocale(l);
      setMultipleInstancesEnabled( s->value(_m_KeyMultipleInstances, _m_MultipleInstancesDef).toBool() );
    s->endGroup();
    //
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
        qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->loadSettings();
}

void BCore::setMultipleInstancesEnabled(bool enabled)
{
    _m_multipleInstances = enabled;
}

bool BCore::setLocale(const QLocale &l)
{
    if ( !availableLocales().contains(l) )
        return false;
    if (l == _m_locale)
        return true;
    for (int i = 0; i < _m_translators.size(); ++i)
    {
        QTranslator *t = _m_translators.at(i);
        QCoreApplication::removeTranslator(t);
        t->deleteLater();
    }
    _m_translators.clear();
    _m_locale = l;
    if (DefaultLocale == _m_locale)
    {
        instance()->emitLocaleChanged();
        return true;
    }
    QString ln = _m_locale.name();
    for (int i = 0; i < _m_translatorPaths.size(); ++i)
    {
        const QString &path = _m_translatorPaths.at(i);
        for (int j = 0; j < _m_translatorPrefixes.size(); ++j)
        {
            QString prefix = path + "/" + _m_translatorPrefixes.at(j);
            QTranslator *t = new QTranslator;
            if ( !t->load(prefix + "_" + ln) && !t->load( prefix + "_" + ln.left(2) ) )
            {
                t->deleteLater();
            }
            else
            {
                QCoreApplication::installTranslator(t);
                _m_translators << t;
            }
        }
    }
    instance()->emitLocaleChanged();
    return true;
}

bool BCore::multipleInstancesEnabled()
{
    return _m_multipleInstances;
}

const QLocale &BCore::currentLocale()
{
    return _m_locale;
}

//translations

void BCore::addTranslatorPath(const QString &path)
{
    if ( !QDir(path).exists() || _m_translatorPaths.contains(path) )
        return;
    _m_translatorPaths << path;
}

void BCore::addTranslatorPrefix(const QString &prefix)
{
    if ( prefix.isEmpty() || _m_translatorPrefixes.contains(prefix) )
        return;
    _m_translatorPrefixes << prefix;
}

QList<QLocale> BCore::availableLocales()
{
    QLocale::setDefault(DefaultLocale);
    QMap<QString, QStringList> m;
    for (int i = 0; i < _m_translatorPaths.size(); ++i)
    {
        const QString &path = _m_translatorPaths.at(i);
        for (int j = 0; j < _m_translatorPrefixes.size(); ++j)
        {
            QFileInfo fi( path + "/" + _m_translatorPrefixes.at(j) );
            QString name = fi.fileName();
            QStringList &sl = m[fi.path()];
            if ( !sl.contains(name) )
                sl << name;
        }
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

QString BCore::translationsDir(bool user)
{
    return user ? _m_userTranslationsDir : _m_sharedTranslationsDir;
}

//paths and data


void BCore::setPath(const QString &key, const QString &path, bool file)
{
    if ( key.isEmpty() )
        return;
    if (file)
        _m_fileMap[key] = path;
    else
        _m_dirMap[key] = path;
}

QString BCore::shared(const QString &key, bool file)
{
    if ( key.isEmpty() )
        return _m_sharedRoot;
    if ( file && _m_fileMap.contains(key) )
        return _m_sharedRoot + "/" + _m_fileMap.value(key);
    else if ( !file && _m_dirMap.contains(key) )
        return _m_sharedRoot + "/" + _m_dirMap.value(key);
    else
        return "";
}

QString BCore::user(const QString &key, bool file)
{
    if ( key.isEmpty() )
        return _m_userRoot;
    if ( file && _m_fileMap.contains(key) )
        return _m_userRoot + "/" + _m_fileMap.value(key);
    else if ( !file && _m_dirMap.contains(key) )
        return _m_userRoot + "/" + _m_dirMap.value(key);
    else
        return "";
}

QString BCore::beqtIcon(const QString &iconName)
{
    return IcoPath + "/" + (QFileInfo(iconName).suffix().isEmpty() ? iconName + ".png" : iconName);
}

QString BCore::docsDir()
{
    return localeBasedDirName(_m_sharedDocsDir);
}

//plugins

void BCore::addPluginHandlingObject(QObject *object)
{
    _m_pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    QPointer<QObject> op(object);
    if ( !op || _m_pluginHandlingObjects.contains(op) )
        return;
    _m_pluginHandlingObjects << op;
    QList<QPluginLoader *> values = _m_pluginMap.values();
    for (int i = 0; i < values.size(); ++i)
    {
        BPluginInterface *interface = qobject_cast<BPluginInterface *>( values.at(i)->instance() );
        if (interface)
            interface->handleLoad( op.data() );
    }
}

void BCore::removePluginHandlingObject(QObject *object)
{
    _m_pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    QPointer<QObject> op(object);
    if ( !op || !_m_pluginHandlingObjects.contains(op) )
        return;
    _m_pluginHandlingObjects.removeAll(op);
    QList<QPluginLoader *> values = _m_pluginMap.values();
    for (int i = 0; i < values.size(); ++i)
    {
        BPluginInterface *interface = qobject_cast<BPluginInterface *>( values.at(i)->instance() );
        if (interface)
            interface->handleUnload( op.data() );
    }
}

void BCore::setPluginValidityChecker( bool (*function)(const QObject *) )
{
    _m_pluginValidityChecker = function;
}

QList<QObject *> BCore::plugins()
{
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    QList<QObject *> list;
    for (int i = 0; i < loaders.size(); ++i)
        list << loaders.at(i)->instance();
    return list;
}

QString BCore::pluginsDir(bool user)
{
    return user ? _m_userPluginsDir : _m_sharedPluginsDir;
}

//filesystem

void BCore::createUserPath(const QString &key, bool file)
{
    if (file)
    {
        if ( !_m_fileMap.contains(key) )
            return;
        QFile f( user(key, true) );
        f.open(QFile::WriteOnly);
        f.close();
    }
    else
    {
        if ( !_m_dirMap.contains(key) )
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

QString BCore::localeBasedFileName(const QString &fileName, const QString &defaultFileName,
                                   const QString &possibleSuffix)
{
    if ( fileName.isEmpty() )
        return "";
    QFileInfo fi(fileName);
    QString bfn = fi.path() + "/" + fi.baseName();
    QString suff = fi.suffix();
    suff = suff.isEmpty() ? possibleSuffix : "";
    if ( !suff.isEmpty() )
        suff.prepend('.');
    QString lang = _m_locale.name().left(2);
    QFile f(bfn + "_" + lang);
    if ( !f.exists() )
        f.setFileName(bfn + "_" + lang + suff);
    if ( !f.exists() )
        f.setFileName(defaultFileName);
    if ( !f.exists() )
        f.setFileName(defaultFileName + suff);
    if ( !f.exists() )
        return "";
    return f.fileName();
}

QString BCore::localeBasedDirName(const QString &dir)
{
    if ( dir.isEmpty() )
        return "";
    QDir d( dir + "/" + _m_locale.name().left(2) );
    if ( !d.exists() )
        d.setPath(dir + "/" + "en");
    if ( !d.exists() )
        d.setPath(dir);
    if ( !d.exists() )
        return "";
    return d.path();
}

//

void BCore::_m_loadPlugin(const QString &fileName)
{
    _m_pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    if ( fileName.isEmpty() || !_m_PluginSuffixes.contains( "*." + QFileInfo(fileName).suffix() ) )
        return;
    if ( _m_pluginMap.contains(fileName) )
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
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
    {
        if (qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->uniqueId() == id)
        {
            pl->unload();
            return pl->deleteLater();
        }
    }
    if ( _m_pluginValidityChecker && !_m_pluginValidityChecker(plugin) )
    {
        pl->unload();
        return pl->deleteLater();
    }
    addTranslatorPrefix( interface->translationPrefix() );
    _m_pluginMap.insert(fileName, pl);
    for (int i = 0; i < _m_pluginHandlingObjects.size(); ++i)
        interface->handleLoad( _m_pluginHandlingObjects.at(i).data() );
}

void BCore::_m_loadPlugins(const QString &dir)
{
    QDir d(dir);
    QStringList list = d.entryList(_m_PluginSuffixes, QDir::Files);
    for (int i = 0; i < list.size(); ++i)
        _m_loadPlugin( d.absoluteFilePath( list.at(i) ) );
}

//

BCore::BCore()
{
    //
}

BCore::~BCore()
{
    //
}

//

void BCore::emitLocaleChanged()
{
    emit localeChanged();
}
