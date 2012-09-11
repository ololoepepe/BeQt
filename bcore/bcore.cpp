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

#include <QDebug>

const QLocale _m_DefLocale = QLocale(QLocale::English, QLocale::UnitedStates);
//
const QString _m_GroupCore = "beqt_core";
  const QString _m_KeyLocale = "locale";

//

QStringList _m_translatorPaths;
QList<QTranslator *> _m_translators;
QLocale _m_locale = _m_DefLocale;
QString _m_sharedRoot;
QString _m_userRoot;
QMap<QString, QString> _m_pathsDir;
QMap<QString, QString> _m_pathsFile;
QVariantMap _m_data;
QList< QPointer<QObject> > _m_pluginHandlingObjects;
QMap<QString, QPluginLoader *> _m_pluginMap;
bool (*_m_pluginValidityChecker)(QObject *) = 0;

//

namespace BCore
{

QSettings *newSettingsInstance()
{
    QString fn = QCoreApplication::applicationDirPath() + "/settings.ini";
    return QFile(fn).exists() ? new QSettings(fn, QSettings::IniFormat) : new QSettings;
}

void saveSettings()
{
    QScopedPointer<QSettings> s( newSettingsInstance() );
    if (!s)
        return;
    s->remove(_m_GroupCore);
    s->beginGroup(_m_GroupCore);
      s->setValue(_m_KeyLocale, _m_locale);
    s->endGroup();
    //
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
        qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->saveSettings();
}

void loadSettings()
{
    QScopedPointer<QSettings> s( newSettingsInstance() );
    if (!s)
        return;
    s->beginGroup(_m_GroupCore);
      QLocale l = s->value( _m_KeyLocale, QLocale::system() ).toLocale();
      if ( QLocale::system() == l && !availableLocales().contains(l) )
          l = QLocale(QLocale::system().language(), QLocale::AnyCountry);
      setLocale(l);
    s->endGroup();
    //
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    for (int i = 0; i < loaders.size(); ++i)
        qobject_cast<BPluginInterface *>( loaders.at(i)->instance() )->loadSettings();
}

void applySettings(const QVariantMap &settings)
{
    if ( settings.contains(GeneralSettingsTabIdLocale) )
        setLocale( settings.value(GeneralSettingsTabIdLocale).toLocale() );
}

void addStandardTranslator(Translator translator)
{
    addTranslator( standardTranslatorPath(translator) );
}

void setStandardTranslators(const QList<Translator> &translators)
{
    for (int i = 0; i < translators.size(); ++i)
        addTranslator( standardTranslatorPath( translators.at(i) ) );
}

void addTranslator(const QString &path)
{
    if ( path.isEmpty() || _m_translatorPaths.contains(path) )
        return;
    _m_translatorPaths << path;
}

void setTranslators(const QStringList &paths)
{
    _m_translatorPaths = paths;
}

void setSharedRoot(const QString &path)
{
    _m_sharedRoot = path;
}

void setUserRoot(const QString &path)
{
    _m_userRoot = path;
}

void setPath(const QString &key, const QString &path, bool file)
{
    if ( key.isEmpty() )
        return;
    if (file)
        _m_pathsFile[key] = path;
    else
        _m_pathsDir[key] = path;
}

void setData(const QString &key, const QVariant &data)
{
    _m_data[key] = data;
}

void createUserPath(const QString &key, bool file)
{
    if (file)
    {
        if ( !_m_pathsFile.contains(key) )
            return;
        QFile f( user(key, true) );
        f.open(QFile::WriteOnly);
        f.close();
    }
    else
    {
        if ( !_m_pathsDir.contains(key) )
            return;
        QString path = user(key);
        QDir(path).mkpath(path);
    }
}

bool copyResource(const QString &key)
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

bool setLocale(const QLocale &locale)
{
    if ( !availableLocales().contains(locale) )
        return false;
    if (locale == _m_locale)
        return true;
    for (int i = 0; i < _m_translators.size(); ++i)
    {
        QTranslator *t = _m_translators.at(i);
        QCoreApplication::removeTranslator(t);
        t->deleteLater();
    }
    _m_translators.clear();
    _m_locale = locale;
    if (_m_DefLocale == _m_locale)
        return true;
    QString ln = _m_locale.name();
    for (int i = 0; i < _m_translatorPaths.size(); ++i)
    {
        const QString &path = _m_translatorPaths.at(i);
        QTranslator *t = new QTranslator;
        if ( !t->load(path + "_" + ln) && !t->load( path + "_" + ln.left(2) ) )
        {
            t->deleteLater();
        }
        else
        {
            QCoreApplication::installTranslator(t);
            _m_translators << t;
        }
    }
    return true;
}

const QLocale &locale()
{
    return _m_locale;
}

const QList<QLocale> availableLocales()
{
    QLocale::setDefault(_m_DefLocale);
    QMap<QString, QStringList> m;
    for (int i = 0; i < _m_translatorPaths.size(); ++i)
    {
        QFileInfo fi( _m_translatorPaths.at(i) );
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
    locales << _m_DefLocale;
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

QString standardTranslatorPath(Translator translator)
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

QString shared(const QString &key, bool file)
{
    if ( key.isEmpty() )
        return _m_sharedRoot;
    if ( file && _m_pathsFile.contains(key) )
        return _m_sharedRoot + Sep + _m_pathsFile.value(key);
    else if ( !file && _m_pathsDir.contains(key) )
        return _m_sharedRoot + Sep + _m_pathsDir.value(key);
    else
        return "";
}

QString user(const QString &key, bool file)
{
    if ( key.isEmpty() )
        return _m_userRoot;
    if ( file && _m_pathsFile.contains(key) )
        return _m_userRoot + Sep + _m_pathsFile.value(key);
    else if ( !file && _m_pathsDir.contains(key) )
        return _m_userRoot + Sep + _m_pathsDir.value(key);
    else
        return "";
}

QVariant data(const QString &key, const QVariant &def)
{
    return _m_data.value(key, def);
}

QString dataS(const QString &key, const QString &def)
{
    return data( key, QVariant(def) ).toString();
}

void loadPlugin(const QString &fileName)
{
    _m_pluginHandlingObjects.removeAll( QPointer<QObject>(0) );
    if ( fileName.isEmpty() || !PluginSuffixes.contains( "*." + QFileInfo(fileName).suffix() ) )
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
    addTranslator( interface->translatorPath() );
    _m_pluginMap.insert(fileName, pl);
    for (int i = 0; i < _m_pluginHandlingObjects.size(); ++i)
        interface->handleLoad( _m_pluginHandlingObjects.at(i).data() );
}

void loadPlugins(const QString &dir)
{
    QDir d(dir);
    QStringList list = d.entryList(PluginSuffixes, QDir::Files);
    for (int i = 0; i < list.size(); ++i)
        loadPlugin( d.absoluteFilePath( list.at(i) ) );
}

QList<QObject *> plugins()
{
    QList<QPluginLoader *> loaders = _m_pluginMap.values();
    QList<QObject *> list;
    for (int i = 0; i < loaders.size(); ++i)
        list << loaders.at(i)->instance();
    return list;
}

void addPluginHandlingObject(QObject *object)
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

void removePluginHandlingObject(QObject *object)
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

void setPluginValidityChecker( bool (*function)(QObject *) )
{
    _m_pluginValidityChecker = function;
}

bool removeDir(const QString &path)
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

void copyDir(const QString &path, const QString &newPath, bool recursive)
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

}
