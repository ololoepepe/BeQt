class QWidget;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bcoreapplication_p.h"
#include "bdirtools.h"
#include "btranslator.h"
#include "bpluginwrapper.h"
#include "btranslator_p.h"
#include "bpluginwrapper_p.h"
#include "bpersoninfoprovider.h"
#include "blogger.h"
#include "bsignaldelayproxy.h"

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
#include <QVariantMap>
#include <QTextCodec>

#include <QDebug>

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

/*============================== Public constructors =======================*/

BCoreApplicationPrivate::BCoreApplicationPrivate(BCoreApplication *q) :
    BBasePrivate(q)
{
    //
}

BCoreApplicationPrivate::~BCoreApplicationPrivate()
{
    if (!qApp)
        qApp->removeEventFilter(this);
    if (!settings.isNull())
    {
        disconnect(settings.data(), SIGNAL(destroyed()), this, SLOT(initSettings()));
        settings->sync();
        delete settings;
    }
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_CLEANUP_RESOURCE(beqtcore);
    Q_CLEANUP_RESOURCE(beqt_translations);
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
    return bTest(qs, w, "There must be a BCoreApplication instance");
}

bool BCoreApplicationPrivate::testCoreUnique()
{
    return bTest(!qs_func(), "BCoreApplication", "There must be only one instance of BCoreApplication");
}

BCoreApplication::LocaleSupportInfo BCoreApplicationPrivate::createLocaleSupportInfo()
{
    BCoreApplication::LocaleSupportInfo info;
    info.locale = QLocale(QLocale::English);
    info.supports = 0;
    info.total = 0;
    return info;
}

/*QString BCoreApplicationPrivate::personInfoString(BPersonInfoProvider *prov, const QLocale &loc, bool noDefault)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BCoreApplicationPrivate") )
        return "";
    if (!prov)
        return "";
    QString s;
    foreach ( const BPersonInfoProvider::PersonInfo &info, prov->infos(noDefault, loc) )
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
}*/

/*============================== Public methods ============================*/

void BCoreApplicationPrivate::init()
{
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtcore);
    Q_INIT_RESOURCE(beqt_translations);
#endif
    bTest(QCoreApplication::instance(), "BCoreApplication", "Missing QCoreApplication instance");
    initSettings();
    QCoreApplication::instance()->installEventFilter(this);
    //infos
    QStringList locs;
    locs << getSharedPrefix();
#if defined(Q_OS_MAC)
    locs << getBundlePrefix();
#endif
    locs << ":";
    locs.removeAll("");
    QString spref = "beqt/infos/";
    beqtAuthors = new BPersonInfoProvider(BDirTools::findResource(spref + "authors.beqt-info", locs), this);
    beqtTranslations = new BPersonInfoProvider(BDirTools::findResource(spref + "translators.beqt-info", locs), this);
    beqtThanksTo = new BPersonInfoProvider(BDirTools::findResource(spref + "thanks-to.beqt-info", locs), this);
    appAuthors = new BPersonInfoProvider(this);
    appTranslations = new BPersonInfoProvider(this);
    appThanksTo = new BPersonInfoProvider(this);
    logger = new BLogger(this);
    languageChangeProxy = new BSignalDelayProxy(100, 200, this);
    connect(languageChangeProxy, SIGNAL(triggered()), q_func(), SIGNAL(languageChanged()));
    languageChangeEventProxy = new BSignalDelayProxy(100, 200, this);
    connect(languageChangeEventProxy, SIGNAL(triggered()), this, SLOT(sendLanguageChangeEvent()));
    blockLanguageChangeEvent = true;
}

bool BCoreApplicationPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (!blockLanguageChangeEvent || e->type() != QEvent::LanguageChange || o != QCoreApplication::instance())
        return BBasePrivate::eventFilter(o, e);
    languageChangeEventProxy->trigger();
    return true;
}

QString BCoreApplicationPrivate::getAppName() const
{
    if ( !appName.isEmpty() )
        return appName;
    QString an = QCoreApplication::applicationName();
    if ( !bTest(!an.isEmpty(), "BCoreApplication", "missing application name") )
        return "";
    appName = an;
    return an;
}

QString BCoreApplicationPrivate::getOrgName() const
{
    if ( !orgName.isEmpty() )
        return orgName;
    QString on = QCoreApplication::organizationName();
    if ( !bTest(!on.isEmpty(), "BCoreApplication", "Missing organization name") )
        return "";
    orgName = on;
    return on;
}

QString BCoreApplicationPrivate::getAppPath() const
{
    if ( !appPath.isEmpty() )
        return appPath;
    QString ap = QCoreApplication::applicationDirPath().remove( QRegExp("/(b|B)(i|I)(n|N)$") );
    if ( !bTest(QFileInfo(ap).isDir(), "BCoreApplicationPrivate", "Unable to get application directory") )
        return "";
    appPath = ap;
    return ap;
}

QString BCoreApplicationPrivate::getUserPrefix() const
{
    getIsPortable();
    if ( !userPrefix.isEmpty() )
        return userPrefix;
    QString an = getAppName();
    if ( an.isEmpty() )
        return "";
#if defined(Q_OS_MAC)
    QString on = getOrgName();
    if ( on.isEmpty() )
        return "";
    userPrefix = QDir::homePath() + "/Library/Application Support/" + on + "/" + an;
#elif defined (Q_OS_LINUX)
    userPrefix = QDir::homePath() + "/." + toLowerNoSpaces(an);
#elif defined(Q_OS_WIN)
    QString on = getOrgName();
    if ( on.isEmpty() )
        return "";
    if (QSysInfo::windowsVersion() ==  QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
        userPrefix = QDir::homePath() + "/Application Data/" + on + "/" + an;
    else
        userPrefix = QDir::homePath() + "/AppData/Roaming/" + on + "/" + an;
#endif
    return userPrefix;
}

QString BCoreApplicationPrivate::getSharedPrefix() const
{
    getIsPortable();
    if ( !sharedPrefix.isEmpty() )
        return sharedPrefix;
#if defined(Q_OS_MAC)
    QString an = getAppName();
    QString on = getOrgName();
    if ( an.isEmpty() || on.isEmpty() )
        return "";
    sharedPrefix = "/Library/Application Support/" + on + "/" + an;
#elif defined (Q_OS_LINUX)
    QString an = getAppName();
    if ( an.isEmpty() )
        return "";
    sharedPrefix = "/usr/share/" + toLowerNoSpaces(an);
#elif defined(Q_OS_WIN)
    QString ap = getAppPath();
    if ( ap.isEmpty() )
        return "";
    sharedPrefix = ap;
#endif
    return sharedPrefix;
}

#if defined(Q_OS_MAC)
QString BCoreApplicationPrivate::getBundlePrefix() const
{
    getIsPortable();
    if ( !bundlePrefix.isEmpty() )
        return bundlePrefix;
    QString ap = getAppPath();
    if ( ap.isEmpty() )
        return "";
    bundlePrefix = QDir(ap + "/../Resources").absolutePath();
    return bundlePrefix;
}
#endif

bool BCoreApplicationPrivate::getIsPortable() const
{
    QString ap = getAppPath();
    QString an = getAppName();
    if ( ap.isEmpty() || an.isEmpty() )
        return false;
    bool b = QFileInfo( confFileName(ap, an) ).isFile();
    if (b)
    {
        userPrefix = ap;
        sharedPrefix = ap;
    #if defined(Q_OS_MAC)
        bundlePrefix = ap;
    #endif
    }
    else
    {
        if (ap == userPrefix)
            userPrefix.clear();
        if (ap == sharedPrefix)
            sharedPrefix.clear();
#if defined(Q_OS_MAC)
        if (ap == bundlePrefix)
            bundlePrefix.clear();
#endif
    }
    return b;
}

QLocale BCoreApplicationPrivate::getLocale() const
{
    return settings->value("BeQt/Core/locale", QLocale::system()).toLocale();
}

void BCoreApplicationPrivate::setLocale(const QLocale &l)
{
    foreach (BTranslator *t, translators)
        t->setLocale(l);
    settings->setValue("BeQt/Core/locale", l);
    languageChangeProxy->trigger();
}

QStringList BCoreApplicationPrivate::getDeactivatedPlugins() const
{
    return settings->value("BeQt/Core/deactivated_plugins").toStringList();
}

void BCoreApplicationPrivate::setDeactivatedPlugins(const QStringList &list)
{
    QStringList nlist = list;
    nlist.removeAll("");
    nlist.removeDuplicates();
    settings->setValue("BeQt/Core/deactivated_plugins", nlist);
}

void BCoreApplicationPrivate::addDeactivatedPlugin(const QString &pluginName)
{
    if ( pluginName.isEmpty() )
        return;
    QStringList list = getDeactivatedPlugins();
    if ( list.contains(pluginName) )
        return;
    list << pluginName;
    setDeactivatedPlugins(list);
}

QSettings *BCoreApplicationPrivate::createSettingsInstance(const QString &fileName) const
{
    QSettings *s = new QSettings(confFileName(getUserPrefix(), fileName), QSettings::IniFormat);
    s->setIniCodec("UTF-8");
    return s;
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
        return getUserPrefix();
    case BCoreApplication::SharedResources :
        return getSharedPrefix();
#if defined(Q_OS_MAC)
    case BCoreApplication::BundleResources :
        return getBundlePrefix();
#endif
    case BCoreApplication::BuiltinResources:
        return ":";
    default:
        return "";
    }
}

void BCoreApplicationPrivate::pluginActivated(BPluginWrapper *pluginWrapper)
{
    QStringList list = getDeactivatedPlugins();
    list.removeAll(pluginWrapper->name());
    setDeactivatedPlugins(list);
    QMetaObject::invokeMethod(q_func(), "pluginActivated", Q_ARG(BPluginWrapper *, pluginWrapper) );
}

void BCoreApplicationPrivate::pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    addDeactivatedPlugin( pluginWrapper->name() );
    QMetaObject::invokeMethod(q_func(), "pluginAboutToBeDeactivated", Q_ARG(BPluginWrapper *, pluginWrapper) );
}

void BCoreApplicationPrivate::installTranslator(BTranslator *translator)
{
    if (!translator || !translator->isValid() || translators.contains(translator->fileName()))
        return;
    translator->setLocale(getLocale());
    if (!translator->d_func()->install())
        return;
    translators.insert(translator->fileName(), translator);
    languageChangeProxy->trigger();
}

void BCoreApplicationPrivate::removeTranslator(BTranslator *translator)
{
    if (!translator || !translators.contains(translator->fileName()))
        return;
    translators.remove(translator->fileName());
    translator->d_func()->remove();
    languageChangeProxy->trigger();
}

void BCoreApplicationPrivate::loadSettings()
{
    if ( settings.isNull() )
        return;
    QMetaObject::invokeMethod( q_func(), "settingsLoaded", Q_ARG( QSettings *, settings.data() ) );
}

void BCoreApplicationPrivate::saveSettings()
{
    if ( settings.isNull() )
        return;
    QMetaObject::invokeMethod( q_func(), "settingsSaved", Q_ARG( QSettings *, settings.data() ) );
    if ( !settings.isNull() )
        settings->sync();
}

/*============================== Public slots ==============================*/

void BCoreApplicationPrivate::initSettings()
{
    settings = createSettingsInstance( getAppName() );
    if ( !settings.isNull() )
        connect( settings.data(), SIGNAL( destroyed() ), this, SLOT( initSettings() ) );
}

void BCoreApplicationPrivate::sendLanguageChangeEvent()
{
    blockLanguageChangeEvent = false;
    QEvent e(QEvent::LanguageChange);
    QCoreApplication::sendEvent(qApp, &e);
    blockLanguageChangeEvent = true;
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
    foreach (BTranslator *t, d_func()->translators)
        delete t;
    foreach (BPluginWrapper *pw, d_func()->plugins)
        delete pw;
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

QSettings *BCoreApplication::settingsInstance()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return 0;
    return ds_func()->settings.data();
}

bool BCoreApplication::isPortable()
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return false;
    return ds_func()->getIsPortable();
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
            if ( !pw->load() )
            {
                pw->deleteLater();
                continue;
            }
            if ( ds->getDeactivatedPlugins().contains( pw->name() ) )
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

void BCoreApplication::installTranslator(BTranslator *translator)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->installTranslator(translator);
}

void BCoreApplication::removeTranslator(BTranslator *translator)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->removeTranslator(translator);
}

QList<BTranslator *> BCoreApplication::translators()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return QList<BTranslator *>();
    return ds_func()->translators.values();
}

BTranslator *BCoreApplication::translator(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return 0;
    return ds_func()->translators.value(fileName);
}

void BCoreApplication::setLocale(const QLocale &l)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    if (l == locale())
        return;
    ds_func()->setLocale(l);
}

QLocale BCoreApplication::locale()
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return QLocale::system();
    return ds_func()->getLocale();
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

void BCoreApplication::loadSettings()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    ds_func()->loadSettings();
}

void BCoreApplication::saveSettings()
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->saveSettings();
}

void BCoreApplication::setApplicationCopyrightPeriod(const QString &s)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appCopyrightYears = s;
}

void BCoreApplication::setApplicationDescription(const QString &s)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appDescription = s;
    ds_func()->appDescriptionFileName.clear();
}

void BCoreApplication::setApplicationDescriptionFile(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appDescriptionFileName = fileName;
    ds_func()->appDescription.clear();
}

void BCoreApplication::setApplicationChangeLog(const QString &s)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appChangeLog = s;
    ds_func()->appChangeLogFileName.clear();
}

void BCoreApplication::setApplicationChangeLogFile(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appChangeLogFileName = fileName;
    ds_func()->appChangeLog.clear();
}

void BCoreApplication::setApplicationLicense(const QString &s)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appLicense = s;
    ds_func()->appLicenseFileName.clear();
}

void BCoreApplication::setApplicationLicenseFile(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appLicenseFileName = fileName;
    ds_func()->appLicense.clear();
}

void BCoreApplication::setApplicationAuthors(const BPersonInfoList &list)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appAuthorsList = list;
    ds_func()->appAuthors->setFileName("");
}

void BCoreApplication::setApplicationAuthorsFile(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appAuthors->setFileName(fileName);
    ds_func()->appAuthorsList.clear();
}

void BCoreApplication::setApplicationTranslations(const BPersonInfoList &list)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appTranslationsList = list;
    ds_func()->appTranslations->setFileName("");
}

void BCoreApplication::setApplicationTranslationsFile(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appTranslations->setFileName(fileName);
    ds_func()->appTranslationsList.clear();
}

void BCoreApplication::setApplicationThanksTo(const BPersonInfoList &list)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appThanksToList = list;
    ds_func()->appThanksTo->setFileName("");
}

void BCoreApplication::setApplicationThanksToFile(const QString &fileName)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return;
    ds_func()->appThanksTo->setFileName(fileName);
    ds_func()->appThanksToList.clear();
}

QString BCoreApplication::applicationInfo(About type, const QLocale &loc)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return "";
    QString fn;
    switch (type)
    {
    case Copyringt:
    {
        QString crp = (!ds_func()->appCopyrightYears.isEmpty() ? (ds_func()->appCopyrightYears + " ") :QString());
        return tr("Copyright") + " (C) " + crp + QCoreApplication::organizationName()
                + " [" + QCoreApplication::organizationDomain() + "]";
    }
    case Description:
        if (!ds_func()->appDescription.isEmpty())
            return ds_func()->appDescription;
        else
            fn = ds_func()->appDescriptionFileName;
        break;
    case ChangeLog:
        if (!ds_func()->appChangeLog.isEmpty())
            return ds_func()->appChangeLog;
        else
            fn = ds_func()->appChangeLogFileName;
        break;
    case License:
        if (!ds_func()->appLicense.isEmpty())
            return ds_func()->appLicense;
        else
            fn = ds_func()->appLicenseFileName;
        break;
    case Authors:
        if (!ds_func()->appAuthorsList.isEmpty())
            return BPersonInfoProvider::infoListToString(ds_func()->appAuthorsList);
        else if (ds_func()->appAuthors)
            return ds_func()->appAuthors->infosString(loc);
        else
            return "";
    case Translators:
        if (!ds_func()->appTranslationsList.isEmpty())
            return BPersonInfoProvider::infoListToString(ds_func()->appTranslationsList);
        else if (ds_func()->appTranslations)
            return ds_func()->appTranslations->infosString(loc, true);
        else
            return "";
    case ThanksTo:
        if (!ds_func()->appThanksToList.isEmpty())
            return BPersonInfoProvider::infoListToString(ds_func()->appThanksToList);
        else if (ds_func()->appThanksTo)
            return ds_func()->appThanksTo->infosString(loc);
        else
            return "";
    default:
        return "";
    }
    return BDirTools::readTextFile(BDirTools::localeBasedFileName(fn), "UTF-8");
}

QString BCoreApplication::beqtInfo(About type, const QLocale &loc)
{
    if (!BCoreApplicationPrivate::testCoreInit())
        return "";
    QString bfn;
    switch (type)
    {
    case Copyringt:
        return tr("Copyright") + " (C) 2012-2013 Andrey Bogdanov [https://github.com/the-dark-angel/BeQt]";
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
        return BPersonInfoProvider::infosString(ds_func()->beqtAuthors, loc);
    case Translators:
        return BPersonInfoProvider::infosString(ds_func()->beqtTranslations, loc, true);
    case ThanksTo:
        return BPersonInfoProvider::infosString(ds_func()->beqtThanksTo, loc);
    default:
        return "";
    }
    QString dir = location(BeqtPath, SharedResources) + "/";
    QString fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    if (fn.isEmpty())
    {
        dir = location(BeqtPath, BuiltinResources) + "/";
        fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    }
#if defined(Q_OS_MAC)
    if (fn.isEmpty())
    {
        dir = location(BeqtPath, BundleResources) + "/";
        fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    }
#endif
    return BDirTools::readTextFile(fn, "UTF-8");
}

void BCoreApplication::setLogger(BLogger *l)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    B_DS(BCoreApplication);
    if ( ds->logger && (!ds->logger->parent() || ds->logger->parent() == _m_self) )
        ds->logger->deleteLater();
    ds->logger = l;
    if ( l && !l->parent() )
        l->setParent(_m_self);
}

BLogger *BCoreApplication::logger()
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return 0;
    return ds_func()->logger;
}

void BCoreApplication::log(const QString &text, BLogger::Level lvl)
{
    if ( !BCoreApplicationPrivate::testCoreInit() )
        return;
    B_DS(BCoreApplication);
    if (!ds->logger)
        return;
    ds->logger->log(text, lvl);
}

/*============================== Static protected variables ================*/

BCoreApplication *BCoreApplication::_m_self = 0;
