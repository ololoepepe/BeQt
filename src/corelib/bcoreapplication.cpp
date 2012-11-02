#include "bcoreapplication.h"
#include "bcoreapplication_p.h"

#include <BeQt>
#include <BDirTools>
#include <BTranslator>
#include <BPlugin>

#include <private/btranslator_p.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QSysInfo>
#include <QFileInfo>
#include <QFile>
#include <QList>
#include <QtGlobal>
#include <QSettings>
#include <QMetaObject>
#include <QTranslator>

QString BCoreApplicationPrivate::toLowerNoSpaces(const QString &string)
{
    return string.toLower().replace(QRegExp("\\s"), "-");
}

QString BCoreApplicationPrivate::subdir(BCoreApplication::Location loc)
{
    switch (loc)
    {
    case BCoreApplication::DocumentationPath:
        return "doc";
    case BCoreApplication::PluginsPath:
        return "plugins";
    case BCoreApplication::SettingsPath:
        return "settings";
    case BCoreApplication::TranslationsPath:
        return "translations";
    case BCoreApplication::BeqtPath:
        return "beqt";
    case BCoreApplication::DataPath:
    default:
        return "";
    }
}

bool BCoreApplicationPrivate::testCoreInit(const char *where)
{
    const char *w = where ? where : "BCoreApplication";
    if ( !bTest(BCoreApplication::_m_self, w, "There must be a BCoreApplication instance") )
        return false;
    else if ( !bTest(BCoreApplication::_m_self->d_func()->initialized, w, "BCoreApplication must be initialized") )
        return false;
    else
        return true;
}

//

BCoreApplicationPrivate::BCoreApplicationPrivate(BCoreApplication *q, const BCoreApplication::AppOptions &options) :
    _m_q(q)
{
    initialized = false;
    portable = false;
    init(options);
}

BCoreApplicationPrivate::~BCoreApplicationPrivate()
{
    foreach (BTranslator *t, internalTranslators)
        t->deleteLater();
    foreach (BTranslator *t, userTranslators)
        removeTranslator(t);
    foreach (BPlugin *pl, plugins)
        pl->deleteLater();
}

//

QString BCoreApplicationPrivate::confFileName(const QString &path, const QString &name, bool create) const
{
    if ( path.isEmpty() || name.isEmpty() )
        return QString();
    QString bfn = name;
#if defined(B_OS_UNIX)
    bfn = toLowerNoSpaces(bfn);
#endif
    QString fn = path + "/" + bfn + ".conf";
    QFile f(fn);
    bool exists = f.exists();
    if (create)
    {
        if ( !exists && !f.open(QFile::WriteOnly) )
            return QString();
        f.close();
    }
    else if (!exists)
    {
        return QString();
    }
    return fn;
}

QString BCoreApplicationPrivate::prefix(BCoreApplication::ResourcesType type) const
{
    switch (type)
    {
    case BCoreApplication::UserResources :
        return userPrefix;
    case BCoreApplication::SharedResources :
        return sharedPrefix;
#if defined(B_OS_MAC)
    case BCoreApplication::BundleResources :
        return bundlePrefix;
#endif
    default:
        return "";
    }
}

void BCoreApplicationPrivate::emitPluginActivated(BPlugin *plugin)
{
    deactivatedPlugins.removeAll( plugin->name() );
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPlugin *, plugin) );
}

void BCoreApplicationPrivate::emitPluginAboutToBeDeactivated(BPlugin *plugin)
{
    if ( !deactivatedPlugins.contains( plugin->name() ) )
        deactivatedPlugins << plugin->name();
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPlugin *, plugin) );
}

void BCoreApplicationPrivate::installTranslator(BTranslator *translator)
{
    if (!translator)
        return;
    translator->d_func()->install();
}

void BCoreApplicationPrivate::removeTranslator(BTranslator *translator)
{
    if (!translator)
        return;
    translator->d_func()->remove();
}

//

#if defined(B_OS_MAC)
const QStringList BCoreApplicationPrivate::PluginSuffixes = QStringList("*.dylib");
#elif defined(B_OS_UNIX)
const QStringList BCoreApplicationPrivate::PluginSuffixes = QStringList("*.so");
#elif defined(B_OS_WIN)
const QStringList BCoreApplicationPrivate::PluginSuffixes = QStringList("*.dll");
#endif
const QString BCoreApplicationPrivate::SettingsGroupBeqt = "BeQt";
const QString BCoreApplicationPrivate::SettingsKeyDeactivatedPlugins = "deactivated_plugins";
const QString BCoreApplicationPrivate::SettingsKeyLocale = "locale";

//

void BCoreApplicationPrivate::init(const BCoreApplication::AppOptions &options)
{
    //checks
    QString an = QCoreApplication::applicationName();
    QString on = QCoreApplication::organizationName();
    QString ap = QCoreApplication::applicationDirPath().remove( QRegExp("/(b|B)(i|I)(n|N)$") );
    if ( !bTest(BCoreApplication::instance(), "BCoreApplication", "missing BCoreApplication instance") )
        return;
    if ( !bTest(QCoreApplication::instance(), "BCoreApplication", "missing QCoreApplication instance") )
        return;
    if ( !bTest(!an.isEmpty(), "BCoreApplication", "missing application name") )
        return;
    if ( !bTest(!on.isEmpty(), "BCoreApplication", "missing organization name") )
        return;
    if ( !bTest(QFileInfo(ap).isDir(), "BCoreApplication", "unable to get application directory") )
        return;
    appName = an;
    orgName = on;
    appPath = ap;
    QString anls = toLowerNoSpaces(appName);
    //vars
#if defined(B_OS_MAC)
    userPrefix = QDir::homePath() + "/Library/Application Support/" + orgName + "/" + appName;
    sharedPrefix = "/Library/Application Support/" + orgName + "/" + appName;
    bundlePrefix = QDir(appPath + "/../Resources").absolutePath();
#elif defined (B_OS_UNIX)
    userPrefix = QDir::homePath() + "/." + anls;
    sharedPrefix = "/usr/share/" + anls;
#elif defined(B_OS_WIN)
    if (QSysInfo::windowsVersion() ==  QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
        userPrefix = QDir::homePath() + "/Application Data/" + orgName + "/" + appName;
    else
        userPrefix = QDir::homePath() + "/AppData/Roaming/" + orgName + "/" + appName;
    /*bool sws = (BCoreApplication::SM_SystemWide == options.settingsMode);
    if (QSysInfo::windowsVersion() ==  QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
    {
        QString x = QDir::rootPath() + "/Documents and Settings/All Users";
        userPrefix = (!sws ? QDir::homePath() : x) + "/Application Data/" + orgName + "/" + appName;
    }
    else
    {
        QString x = QDir::rootPath() + "/Users/Default";
        userPrefix = (!sws ? QDir::homePath() : x) + "/AppData/Roaming/" + orgName + "/" + appName;
    }*/
    sharedPrefix = appPath;
#endif
    //app mode
    portable = QFileInfo( confFileName(appPath, appName, false) ).isFile();
    if (portable)
    {
        userPrefix = appPath;
        sharedPrefix = appPath;
#if defined(B_OS_MAC)
        bundlePrefix = appPath;
#endif
    }
    //default locale
    locale = options.defaultLocale;
    //reading beqt settings
    if (!options.noSettingsDir)
    {
        QSettings *s = new QSettings(confFileName(userPrefix, appName, true), QSettings::IniFormat);
        if (s)
        {
            s->beginGroup(SettingsGroupBeqt);
            deactivatedPlugins = s->value(SettingsKeyDeactivatedPlugins).toStringList();
            locale = s->value(SettingsKeyLocale, locale).toLocale();
            s->endGroup();
            s->deleteLater();
        }
    }
    //installing basic translators
    BTranslator *t = new BTranslator("qt");
    installTranslator(t);
    internalTranslators << t;
    t = new BTranslator("beqt");
    internalTranslators << t;
    installTranslator(t);
    //creating settings dir
    if (!portable && !options.noSettingsDir)
        BDirTools::mkpath( userPrefix + "/" + subdir(BCoreApplication::SettingsPath) );
    //initialized
    initialized = true;
}

//

BCoreApplication *BCoreApplication::instance()
{
    return _m_self;
}

QString BCoreApplication::location(Location loc, ResourcesType type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return "";
    QDir d( _m_self->d_func()->prefix(type) + "/" + BCoreApplicationPrivate::subdir(loc) );
    return d.exists() ? d.absolutePath() : "";
}

QString BCoreApplication::location(const QString &subdir, ResourcesType type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return "";
    if ( subdir.isEmpty() )
        return "";
    QDir d(_m_self->d_func()->prefix(type) + "/" + subdir);
    return d.exists() ? d.absolutePath() : "";
}

QStringList BCoreApplication::locations(Location loc)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QStringList();
    QStringList sl;
    sl << location(loc, UserResources);
    sl << location(loc, SharedResources);
#if defined(B_OS_MAC)
    sl << location(loc, BundleResources);
#endif
    sl.removeAll("");
    sl.removeDuplicates();
    return sl;
}

QStringList BCoreApplication::locations(const QString &subdir)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QStringList();
    QStringList sl;
    sl << location(subdir, UserResources);
    sl << location(subdir, SharedResources);
#if defined(B_OS_MAC)
    sl << location(subdir, BundleResources);
#endif
    sl.removeAll("");
    sl.removeDuplicates();
    return sl;
}

void BCoreApplication::createUserLocation(Location loc)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BDirTools::mkpath( _m_self->d_func()->userPrefix + "/" + BCoreApplicationPrivate::subdir(loc) );
}

void BCoreApplication::createUserLocation(const QString &subdir)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BDirTools::mkpath(_m_self->d_func()->userPrefix + "/" + subdir);
}

void BCoreApplication::createUserLocations(const QStringList &subdirs)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    foreach (QString subdir, subdirs)
        createUserLocation(subdir);
}

QSettings *BCoreApplication::createAppSettingsInstance(bool createFile)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return 0;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    return new QSettings(d->confFileName(d->userPrefix, d->appName, createFile), QSettings::IniFormat);
}

void BCoreApplication::loadPlugins(const QStringList &acceptableTypes,
                                   bool (*testFunction)(const QObject *), bool reload)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    //loading plugins
    //TODO: No means to determine from which dir the plugin is loaded.
    //Plugins from user dir should replace plugins from other dirs, even if they are already loaded
    //For now it's only possible to reload ALL plugins, so ones located in user dir are loaded first
    QStringList dirs = locations(PluginsPath);
    foreach (QString dirName, dirs)
    {
        QDir dir(dirName);
        QStringList files = dir.entryList(BCoreApplicationPrivate::PluginSuffixes, QDir::Files);
        foreach (QString file, files)
        {
            BPlugin *p = new BPlugin( dir.absoluteFilePath(file) );
            if ( !p->isValid() )
            {
                p->deleteLater();
                continue;
            }
            QString name = p->name();
            if ( d->plugins.contains(name) )
            {
                if (reload)
                {
                    BPlugin *pp = d->plugins.take(name);
                    //Notifying from plugin itself
                    pp->deactivate();
                    pp->deleteLater();
                }
                else
                {
                    p->deleteLater();
                    continue;
                }
            }
            if ( !acceptableTypes.isEmpty() && !acceptableTypes.contains( p->type() ) )
            {
                p->deleteLater();
                continue;
            }
            if ( testFunction && !testFunction( p->instance() ) )
            {
                p->deleteLater();
                continue;
            }
            if ( d->deactivatedPlugins.contains(name) )
                p->deactivate();
            else
                p->activate();
            d->plugins.insert(name, p);
            //Notifying from plugin itself
        }
    }
}

BPlugin *BCoreApplication::plugin(const QString &name)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return 0;
    return _m_self->d_func()->plugins.value(name);
}

QList<BPlugin *> BCoreApplication::plugins(const QString &type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<BPlugin *>();
    QList<BPlugin *> list;
    foreach (BPlugin *pl, _m_self->d_func()->plugins)
        if (pl->type() == type)
            list << pl;
    return list;
}

void BCoreApplication::installTranslator(BTranslator *translator)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    if ( !translator || !translator->isValid() )
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    if ( d->userTranslators.contains(translator) )
        return;
    d->userTranslators << translator;
    d->installTranslator(translator);
}

void BCoreApplication::removeTranslator(BTranslator *translator)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    if (!translator)
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    if ( !d->userTranslators.removeAll(translator) )
        return;
    d->removeTranslator(translator);
}

void BCoreApplication::setLocale(const QLocale &l)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    if (l == d->locale)
        return;
    d->locale = l;
    retranslateUi();
}

QLocale BCoreApplication::locale()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QLocale::system();
    return _m_self->d_func()->locale;
}

QList<QLocale> BCoreApplication::availableLocales()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<QLocale>();
    QList<QLocale> list;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    foreach (BTranslator *t, d->internalTranslators)
        list << t->availableLocales();
    foreach (BTranslator *t, d->userTranslators)
        list << t->availableLocales();
    return list;
}

BCoreApplication::LocaleSupport BCoreApplication::localeSupport()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return LS_No;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    bool beqtSupport = false;
    foreach (BTranslator *t, d->internalTranslators)
        if (t->fileName() == "beqt")
            beqtSupport = t->availableLocales().contains(d->locale);
    int userCount = d->userTranslators.size();
    int userSupportedCount = 0;
    foreach (BTranslator *t, d->userTranslators)
        if ( t->availableLocales().contains(d->locale) )
            ++userSupportedCount;
    if (beqtSupport && userSupportedCount == userCount)
        return LS_Full;
    else if (userSupportedCount == userCount)
        return LS_Good;
    else if (beqtSupport)
        return LS_Partial;
    else
        return LS_No;
}

void BCoreApplication::retranslateUi()
{
    //TODO: Block QCoreApplication LanguageChange events (maybe inside BPlugin)
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    foreach (BTranslator *t, d->internalTranslators)
    {
        d->removeTranslator(t);
        d->installTranslator(t);
    }
    foreach (BTranslator *t, d->userTranslators)
    {
        d->removeTranslator(t);
        d->installTranslator(t);
    }
    //TODO: plugins' translators?
    QMetaObject::invokeMethod(_m_self, "languageChanged");
}

void BCoreApplication::saveSettings()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    QSettings *s = createAppSettingsInstance(true);
    if (!s)
        return;
    s->beginGroup(BCoreApplicationPrivate::SettingsGroupBeqt);
    s->setValue(BCoreApplicationPrivate::SettingsKeyDeactivatedPlugins, d->deactivatedPlugins);
    s->setValue(BCoreApplicationPrivate::SettingsKeyLocale, d->locale);
    s->endGroup();
}

//

BCoreApplication *BCoreApplication::_m_self = 0;

//

BCoreApplication::BCoreApplication(const AppOptions &options) :
    QObject(0), _m_d( new BCoreApplicationPrivate(this, options) )
{
    delete _m_d;
}

BCoreApplication::~BCoreApplication()
{
    //
}
