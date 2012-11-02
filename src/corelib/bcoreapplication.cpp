#include "bcoreapplication.h"
#include "bcoreapplication_p.h"

#include <BeQt>
#include <BDirTools>
#include <BTranslator>
#include <BPluginWrapper>

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
    foreach (BTranslator *t, translators)
        removeTranslator(t, false);
    foreach (BPluginWrapper *pw, plugins)
        pw->deleteLater();
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

void BCoreApplicationPrivate::emitPluginActivated(BPluginWrapper *pluginWrapper)
{
    deactivatedPlugins.removeAll( pluginWrapper->name() );
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPluginWrapper *, pluginWrapper) );
}

void BCoreApplicationPrivate::emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    if ( !deactivatedPlugins.contains( pluginWrapper->name() ) )
        deactivatedPlugins << pluginWrapper->name();
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPluginWrapper *, pluginWrapper) );
}

void BCoreApplicationPrivate::emitLanguageChange()
{
    QMetaObject::invokeMethod(q_func(), "languageChanged");
}

void BCoreApplicationPrivate::installTranslator(BTranslator *translator, bool languageChange)
{
    if ( !translator || !translator->isValid() || translators.contains( translator->fileName() ) )
        return;
    translators.insert(translator->fileName(), translator);
    translator->d_func()->install();
    if (languageChange)
        emitLanguageChange();
}

void BCoreApplicationPrivate::removeTranslator(BTranslator *translator, bool languageChange)
{
    if ( !translator || !translators.contains( translator->fileName() ) )
        return;
    translators.remove( translator->fileName() );
    translator->d_func()->remove();
    if (languageChange)
        emitLanguageChange();
}

void BCoreApplicationPrivate::loadSettings()
{
    QSettings *s = BCoreApplication::createAppSettingsInstance(true);
    if (!s)
        return;
    s->beginGroup(SettingsGroupBeqt);
      s->beginGroup(SettingsGroupCore);
        deactivatedPlugins = s->value(SettingsKeyDeactivatedPlugins).toStringList();
        locale = s->value(SettingsKeyLocale, locale).toLocale();
      s->endGroup();
    s->endGroup();
    s->deleteLater();
    QMetaObject::invokeMethod(q_func(), "settingsLoaded");
}

void BCoreApplicationPrivate::saveSettings()
{
    QSettings *s = BCoreApplication::createAppSettingsInstance(true);
    if (!s)
        return;
    s->beginGroup(SettingsGroupBeqt);
      s->beginGroup(SettingsGroupCore);
        s->setValue(SettingsKeyDeactivatedPlugins, deactivatedPlugins);
        s->setValue(SettingsKeyLocale, locale);
      s->endGroup();
    s->endGroup();
    QMetaObject::invokeMethod(q_func(), "settingsSaved");
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
  const QString BCoreApplicationPrivate::SettingsGroupCore = "Core";
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

void BCoreApplication::registerPluginWrapper(BPluginWrapper *plugin)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    if (!plugin)
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    if ( d->plugins.contains(plugin) )
        return;
    d->plugins << plugin;
}

void BCoreApplication::unregisterPluginWrapper(BPluginWrapper *plugin)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    if (!plugin)
        return;
    _m_self->d_func()->plugins.removeAll(plugin);
}

void BCoreApplication::loadPlugins(const QStringList &acceptableTypes,
                                   BPluginWrapper::InterfaceTestFunction function, bool reload)
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
            BPluginWrapper *pw = new BPluginWrapper( dir.absoluteFilePath(file) );
            pw->setAcceptableTypes(acceptableTypes);
            pw->setInterfaceTestFunction(function);
            pw->activate();
            if ( !pw->isValid() ) //Validity checking includes checking that the plugin is unique (global map)
            {
                pw->deleteLater();
                continue;
            }
            if ( d->deactivatedPlugins.contains( pw->name() ) )
                pw->deactivate();
            else
                d->emitPluginActivated(pw);
            d->plugins << pw;
        }
    }
}

QList<BPluginWrapper *> BCoreApplication::pluginWrappers(const QString &type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<BPluginWrapper *>();
    QList<BPluginWrapper *> list;
    foreach (BPluginWrapper *pw, _m_self->d_func()->plugins)
        if (pw->type() == type)
            list << pw;
    return list;
}

void BCoreApplication::installTranslator(BTranslator *translator, bool noLanguageChange)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    _m_self->d_func()->installTranslator(translator, !noLanguageChange);
}

void BCoreApplication::removeTranslator(BTranslator *translator, bool noLanguageChange)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    _m_self->d_func()->removeTranslator(translator, !noLanguageChange);
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

QList<BCoreApplication::LocaleSupportInfo> BCoreApplication::availableLocales(bool alwaysIncludeEnglish)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<BCoreApplication::LocaleSupportInfo>();
    BCoreApplicationPrivate *const d = _m_self->d_func();
    QList< QList<QLocale> > lll;
    foreach (BTranslator *t, d->translators)
        lll << t->availableLocales();
    QList<QLocale> llist;
    QList<BCoreApplication::LocaleSupportInfo> list;
    if (alwaysIncludeEnglish)
    {
        LocaleSupportInfo en;
        en.total = d->translators.size();
        en.supports = en.total;
        list << en;
    }
    foreach (const QList<QLocale> &ll, lll)
    {
        foreach (const QLocale &l, ll)
        {
            if ( !llist.contains(l) )
            {
                llist << l;
                LocaleSupportInfo lsi;
                lsi.locale = l;
                lsi.total = d->translators.size();
                list << lsi;
            }
        }
    }
    for (int i = 0; i < list.size(); ++i)
    {
        LocaleSupportInfo &lsi = list[i];
        foreach (const QList<QLocale> &ll, lll)
            if ( ll.contains(lsi.locale) )
                ++lsi.supports;
    }
    return list;
}

void BCoreApplication::retranslateUi()
{
    //TODO: Block QCoreApplication LanguageChange events
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    BCoreApplicationPrivate *const d = _m_self->d_func();
    foreach (BTranslator *t, d->translators)
    {
        d->removeTranslator(t, false);
        d->installTranslator(t, false);
    }
    QMetaObject::invokeMethod(_m_self, "languageChanged");
}

void BCoreApplication::loadSettings()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    _m_self->d_func()->loadSettings();
}

void BCoreApplication::saveSettings()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    _m_self->d_func()->saveSettings();
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
