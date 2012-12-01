#include "bcoreapplication.h"
#include "bglobal.h"
#include "bcoreapplication_p.h"
#include "bdirtools.h"
#include "btranslator.h"
#include "bpluginwrapper.h"
#include "btranslator_p.h"
#include "bpluginwrapper_p.h"
#include "bpersoninfoprovider.h"

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
#include <QPointer>
#include <QEvent>

#include <QDebug>

/*============================================================================
================================ BCoreApplicationEventFilter =================
============================================================================*/

/*============================== Public constructors =======================*/

BCoreApplicationEventFilter::BCoreApplicationEventFilter(BCoreApplicationPrivate *p) :
    QObject(p), _m_p(p)
{
    //
}

BCoreApplicationEventFilter::~BCoreApplicationEventFilter()
{
    //
}

/*============================== Public methods ============================*/

bool BCoreApplicationEventFilter::eventFilter(QObject *o, QEvent *e)
{
    if ( !_m_p->blockLanguageChange || e->type() != QEvent::LanguageChange ||
         o != (QObject *) QCoreApplication::instance() )
        return QObject::eventFilter(o, e);
    return true;
}

/*============================================================================
================================ BCoreApplicationPrivate =====================
============================================================================*/

/*============================== Static public constants ===================*/

#if defined(Q_OS_MAC)
const QStringList BCoreApplicationPrivate::PluginSuffixes = QStringList("*.dylib");
#elif defined(Q_OS_LINUX)
const QStringList BCoreApplicationPrivate::PluginSuffixes = QStringList("*.so");
#elif defined(Q_OS_WIN)
const QStringList BCoreApplicationPrivate::PluginSuffixes = QStringList("*.dll");
#endif
const QString BCoreApplicationPrivate::SettingsGroupBeqt = "BeQt";
  const QString BCoreApplicationPrivate::SettingsGroupCore = "Core";
    const QString BCoreApplicationPrivate::SettingsKeyDeactivatedPlugins = "deactivated_plugins";
    const QString BCoreApplicationPrivate::SettingsKeyLocale = "locale";

/*============================== Public constructors =======================*/

BCoreApplicationPrivate::BCoreApplicationPrivate(BCoreApplication *q) :
    BBasePrivate(q)
{
    //
}

BCoreApplicationPrivate::~BCoreApplicationPrivate()
{
    foreach (BTranslator *t, translators)
        removeTranslator(t, false);
    foreach (BPluginWrapper *pw, plugins)
        pw->deleteLater();
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(beqtcore);
#endif
}

/*============================== Static public methods =====================*/

BCoreApplicationPrivate *BCoreApplicationPrivate::instance()
{
    return bApp ? bApp->ds_func() : 0;
}

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
    B_QS(BCoreApplication);
    if ( !bTest(qs, w, "There must be a BCoreApplication instance") )
        return false;
    else if ( !bTest(qs->d_func()->initialized, w, "BCoreApplication must be initialized") )
        return false;
    else
        return true;
}

bool BCoreApplicationPrivate::testCoreUnique()
{
    return bTest(!qs_func(), "BCoreApplication", "There must be only one instance of BCoreApplication");
}

QSettings *BCoreApplicationPrivate::createSettingsInstance(const QString &fileName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BCoreApplicationPrivate") )
        return 0;
    B_QS(BCoreApplication);
    return new QSettings(qs->d_func()->confFileName(qs->d_func()->userPrefix, fileName), QSettings::IniFormat);
}

BCoreApplication::LocaleSupportInfo BCoreApplicationPrivate::createLocaleSupportInfo()
{
    BCoreApplication::LocaleSupportInfo info;
    info.locale = QLocale(QLocale::English);
    info.supports = 0;
    info.total = 0;
    return info;
}

QString BCoreApplicationPrivate::personInfoString(BPersonInfoProvider *prov, const QLocale &loc)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BCoreApplicationPrivate") )
        return "";
    if (!prov)
        return "";
    QString s;
    foreach ( const BPersonInfoProvider::PersonInfo &info, prov->infos(loc) )
    {
        if ( info.name.isEmpty() )
            continue;
        s += tr("Name:", "info") + " " + info.name + "\n";
        if ( !info.role.isEmpty() )
            s += tr("Role:", "info") + " " + info.role + "\n";
        if ( !info.site.isEmpty() )
            s += tr("Website:", "info") + " " + info.site + "\n";
        if ( !info.mail.isEmpty() )
            s += tr("E-mail:", "info") + " " + info.mail + "\n";
    }
    if ( !s.isEmpty() )
        s.remove(s.length() - 1, 1);
    return s;
}

/*============================== Public methods ============================*/

void BCoreApplicationPrivate::init()
{
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtcore);
#endif
    initialized = false;
    portable = false;
    //checks
    QString an = QCoreApplication::applicationName();
    QString on = QCoreApplication::organizationName();
    QString ap = QCoreApplication::applicationDirPath().remove( QRegExp("/(b|B)(i|I)(n|N)$") );
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
#if defined(Q_OS_MAC)
    userPrefix = QDir::homePath() + "/Library/Application Support/" + orgName + "/" + appName;
    sharedPrefix = "/Library/Application Support/" + orgName + "/" + appName;
    bundlePrefix = QDir(appPath + "/../Resources").absolutePath();
#elif defined (Q_OS_LINUX)
    userPrefix = QDir::homePath() + "/." + anls;
    sharedPrefix = "/usr/share/" + anls;
#elif defined(Q_OS_WIN)
    if (QSysInfo::windowsVersion() ==  QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
        userPrefix = QDir::homePath() + "/Application Data/" + orgName + "/" + appName;
    else
        userPrefix = QDir::homePath() + "/AppData/Roaming/" + orgName + "/" + appName;
    sharedPrefix = appPath;
#endif
    //app mode
    portable = QFileInfo( confFileName(appPath, appName) ).isFile();
    if (portable)
    {
        userPrefix = appPath;
        sharedPrefix = appPath;
#if defined(Q_OS_MAC)
        bundlePrefix = appPath;
#endif
    }
    //localization
    locale = QLocale::system();
    appEventFilter = new BCoreApplicationEventFilter(this);
    QCoreApplication::instance()->installEventFilter(appEventFilter);
    //initialized
    initialized = true;
    //infos
    QStringList locs;
    locs << sharedPrefix;
#if defined(Q_OS_MAC)
    locs << bundlePrefix;
#endif
    locs << ":";
    QString spref = "beqt/infos/";
    beqtAuthors = new BPersonInfoProvider(BDirTools::findResource(spref + "authors.beqt-info", locs), this);
    beqtTranslations = new BPersonInfoProvider(BDirTools::findResource(spref + "translators.beqt-info", locs), this);
    beqtThanksTo = new BPersonInfoProvider(BDirTools::findResource(spref + "thanks-to.beqt-info", locs), this);
}

QString BCoreApplicationPrivate::confFileName(const QString &path, const QString &name) const
{
    if ( path.isEmpty() || name.isEmpty() )
        return QString();
    QString bfn = name;
#if defined(Q_OS_UNIX)
    bfn = toLowerNoSpaces(bfn);
#endif
    return path + "/settings/" + bfn + ".conf";
}

QString BCoreApplicationPrivate::prefix(BCoreApplication::ResourcesType type) const
{
    switch (type)
    {
    case BCoreApplication::UserResources :
        return userPrefix;
    case BCoreApplication::SharedResources :
        return sharedPrefix;
#if defined(Q_OS_MAC)
    case BCoreApplication::BundleResources :
        return bundlePrefix;
#endif
    case BCoreApplication::BuiltinResources:
        return ":";
    default:
        return "";
    }
}

void BCoreApplicationPrivate::pluginActivated(BPluginWrapper *pluginWrapper)
{
    deactivatedPlugins.removeAll( pluginWrapper->name() );
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPluginWrapper *, pluginWrapper) );
}

void BCoreApplicationPrivate::pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    deactivatedPlugins << pluginWrapper->name();
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPluginWrapper *, pluginWrapper) );
}

void BCoreApplicationPrivate::emitLanguageChange()
{
    QMetaObject::invokeMethod(q_func(), "languageChanged");
}

void BCoreApplicationPrivate::installTranslator(BTranslator *translator, bool blockLC)
{
    if ( !translator || !translator->isValid() || translators.contains( translator->fileName() ) )
        return;
    translators.insert(translator->fileName(), translator);
    translator->d_func()->install(blockLC);
    emitLanguageChange();
}

void BCoreApplicationPrivate::removeTranslator(BTranslator *translator, bool blockLC)
{
    if ( !translator || !translators.contains( translator->fileName() ) )
        return;
    translators.remove( translator->fileName() );
    translator->d_func()->remove(blockLC);
    emitLanguageChange();
}

void BCoreApplicationPrivate::loadSettings()
{
    QPointer<QSettings> s = BCoreApplication::createAppSettingsInstance();
    if ( s.isNull() )
        return;
    s->beginGroup(SettingsGroupBeqt);
      s->beginGroup(SettingsGroupCore);
        deactivatedPlugins = s->value(SettingsKeyDeactivatedPlugins).toStringList();
        locale = s->value(SettingsKeyLocale, locale).toLocale();
      s->endGroup();
    s->endGroup();
    QMetaObject::invokeMethod( q_func(), "settingsLoaded", Q_ARG( QSettings *, s.data() ) );
    if ( s.isNull() )
        return;
    s->deleteLater();
}

void BCoreApplicationPrivate::saveSettings()
{
    QPointer<QSettings> s = BCoreApplication::createAppSettingsInstance();
    if ( s.isNull() )
        return;
    s->beginGroup(SettingsGroupBeqt);
      s->beginGroup(SettingsGroupCore);
        deactivatedPlugins.removeDuplicates();
        s->setValue(SettingsKeyDeactivatedPlugins, deactivatedPlugins);
        s->setValue(SettingsKeyLocale, locale);
      s->endGroup();
    s->endGroup();
    QMetaObject::invokeMethod( q_func(), "settingsSaved", Q_ARG( QSettings *, s.data() ) );
    if ( s.isNull() )
        return;
    s->sync();
    s->deleteLater();
}

/*============================================================================
================================ BCoreApplication ============================
============================================================================*/

/*============================== Public constructors =======================*/

BCoreApplication::BCoreApplication() :
    QObject(0), BBase( *new BCoreApplicationPrivate(this) )
{
    d_func()->init();
    BCoreApplicationPrivate::testCoreUnique();
    _m_self = this;
}

BCoreApplication::~BCoreApplication()
{
    _m_self = 0;
}

/*============================== Protected constructors ====================*/

BCoreApplication::BCoreApplication(BCoreApplicationPrivate &d) :
    BBase(d)
{
    d_func()->init();
    BCoreApplicationPrivate::testCoreUnique();
    _m_self = this;
}

/*============================== Static public methods =====================*/

BCoreApplication *BCoreApplication::instance()
{
    return _m_self;
}

QString BCoreApplication::location(Location loc, ResourcesType type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return "";
    QDir d( ds_func()->prefix(type) + "/" + BCoreApplicationPrivate::subdir(loc) );
    return d.exists() ? d.absolutePath() : "";
}

QString BCoreApplication::location(const QString &subdir, ResourcesType type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return "";
    if ( subdir.isEmpty() )
        return "";
    QDir d(ds_func()->prefix(type) + "/" + subdir);
    return d.exists() ? d.absolutePath() : "";
}

QStringList BCoreApplication::locations(Location loc)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QStringList();
    QStringList sl;
    sl << location(loc, UserResources);
    sl << location(loc, SharedResources);
#if defined(Q_OS_MAC)
    sl << location(loc, BundleResources);
#endif
    sl << location(loc, BuiltinResources);
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
#if defined(Q_OS_MAC)
    sl << location(subdir, BundleResources);
#endif
    sl << location(subdir, BuiltinResources);
    sl.removeAll("");
    sl.removeDuplicates();
    return sl;
}

QSettings *BCoreApplication::createAppSettingsInstance()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return 0;
    return BCoreApplicationPrivate::createSettingsInstance(ds_func()->appName);
}

void BCoreApplication::registerPluginWrapper(BPluginWrapper *plugin)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    if (!plugin)
        return;
    B_DS(BCoreApplication);
    if ( ds->plugins.contains(plugin) )
        return;
    ds->plugins << plugin;
}

void BCoreApplication::unregisterPluginWrapper(BPluginWrapper *plugin)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    if (!plugin)
        return;
    ds_func()->plugins.removeAll(plugin);
}

void BCoreApplication::loadPlugins(const QStringList &acceptableTypes, InterfaceTestFunction function)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    B_DS(BCoreApplication);
    BPluginWrapper::setAcceptableTypes(acceptableTypes);
    BPluginWrapper::setInterfaceTestFunction(function);
    //loading plugins
    //TODO (release 3.0.0): No means to determine from which dir the plugin is loaded.
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
            if ( !pw->load() )
            {
                pw->deleteLater();
                continue;
            }
            if ( ds->deactivatedPlugins.contains( pw->name() ) )
                pw->unload();
            else
                pw->activate();
            ds->plugins << pw;
        }
    }
}

QList<BPluginWrapper *> BCoreApplication::pluginWrappers(const QString &type)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<BPluginWrapper *>();
    QList<BPluginWrapper *> list;
    foreach (BPluginWrapper *pw, ds_func()->plugins)
        if (type.isEmpty() || pw->type() == type)
            list << pw;
    return list;
}

void BCoreApplication::installTranslator(BTranslator *translator, bool blockLanguageChange)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    ds_func()->installTranslator(translator, blockLanguageChange);
}

void BCoreApplication::removeTranslator(BTranslator *translator, bool blockLanguageChange)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    ds_func()->removeTranslator(translator, blockLanguageChange);
}

void BCoreApplication::setLocale(const QLocale &l, bool noRetranslate)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    B_DS(BCoreApplication);
    if (l == ds->locale)
        return;
    ds->locale = l;
    if (!noRetranslate)
        retranslateUi();
}

QLocale BCoreApplication::locale()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QLocale::system();
    return ds_func()->locale;
}

QList<BCoreApplication::LocaleSupportInfo> BCoreApplication::availableLocales(bool alwaysIncludeEnglish)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<BCoreApplication::LocaleSupportInfo>();
    B_DS(BCoreApplication);
    QList< QList<QLocale> > lll;
    foreach (BTranslator *t, ds->translators)
        lll << t->availableLocales();
    QList<QLocale> llist;
    QList<BCoreApplication::LocaleSupportInfo> list;
    if (alwaysIncludeEnglish)
    {
        LocaleSupportInfo en = BCoreApplicationPrivate::createLocaleSupportInfo();
        en.total = ds->translators.size();
        if (!en.total)
            en.total += 1;
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
                LocaleSupportInfo lsi = BCoreApplicationPrivate::createLocaleSupportInfo();
                lsi.locale = l;
                lsi.total = ds->translators.size();
                if (!lsi.total)
                    lsi.total += 1;
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

void BCoreApplication::retranslateUi(bool blockLanguageChange)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    B_DS(BCoreApplication);
    ds->blockLanguageChange = blockLanguageChange;
    foreach (BTranslator *t, ds->translators)
    {
        ds->removeTranslator(t, false);
        ds->installTranslator(t, false);
    }
    ds->blockLanguageChange = false;
    QMetaObject::invokeMethod(_m_self, "languageChanged");
}

void BCoreApplication::loadSettings()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    ds_func()->loadSettings();
}

void BCoreApplication::saveSettings()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    ds_func()->saveSettings();
}

QString BCoreApplication::beqtInfo(BeQtInfo type, const QLocale &loc)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return "";
    QString bfn;
    switch (type)
    {
    case Description:
        bfn = "description/DESCRIPTION.txt";
        break;
    case ChangeLog:
        bfn = "changelog/ChangeLog.txt";
        break;
    case License:
        bfn = "copying/COPYING.txt";
        break;
    case Authors:
        return BCoreApplicationPrivate::personInfoString(ds_func()->beqtAuthors, loc);
    case Translators:
        return BCoreApplicationPrivate::personInfoString(ds_func()->beqtTranslations, loc);
    case ThanksTo:
        return BCoreApplicationPrivate::personInfoString(ds_func()->beqtThanksTo, loc);
    default:
        return "";
    }
    QString dir = location(BeqtPath, SharedResources) + "/";
    QString fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    if ( fn.isEmpty() )
    {
        dir = location(BeqtPath, BuiltinResources) + "/";
        fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    }
#if defined(Q_OS_MAC)
    if ( fn.isEmpty() )
    {
        dir = location(BeqtPath, BundleResources) + "/";
        fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    }
#endif
    return BDirTools::readTextFile(fn, "UTF-8");
}

/*============================== Static protected variables ================*/

BCoreApplication *BCoreApplication::_m_self = 0;
