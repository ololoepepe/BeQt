/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCore module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

class QWidget;

#include "bapplicationbase.h"
#include "bglobal.h"
#include "bapplicationbase_p.h"
#include "bdirtools.h"
#include "btranslator.h"
#include "bpluginwrapper.h"
#include "btranslator_p.h"
#include "bpluginwrapper_p.h"
#include "bpersoninfoprovider.h"
#include "blogger.h"
#include "bsignaldelayproxy.h"
#include "bversion.h"

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
#include <QUrl>

#include <QDebug>

/*============================================================================
================================ BApplicationBasePrivate =====================
============================================================================*/

/*============================== Static public constants ===================*/

#if defined(Q_OS_MAC)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList("*.dylib");
#elif defined(Q_OS_LINUX)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList("*.so");
#elif defined(Q_OS_WIN)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList("*.dll");
#endif

/*============================== Public constructors =======================*/

BApplicationBasePrivate::BApplicationBasePrivate(BApplicationBase *q) :
    BBaseObjectPrivate(q)
{
    //
}

BApplicationBasePrivate::~BApplicationBasePrivate()
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

BApplicationBasePrivate *BApplicationBasePrivate::instance()
{
    return qs_func() ? qs_func()->ds_func() : 0;
}

QString BApplicationBasePrivate::toLowerNoSpaces(const QString &string)
{
    return string.toLower().replace(QRegExp("\\s"), "-");
}

QString BApplicationBasePrivate::subdir(BApplicationBase::Location loc)
{
    switch (loc)
    {
    case BApplicationBase::DocumentationPath:
        return "doc";
    case BApplicationBase::PluginsPath:
        return "plugins";
    case BApplicationBase::SettingsPath:
        return "settings";
    case BApplicationBase::TranslationsPath:
        return "translations";
    case BApplicationBase::BeqtPath:
        return "beqt";
    case BApplicationBase::DataPath:
    default:
        return "";
    }
}

bool BApplicationBasePrivate::testCoreInit(const char *where)
{
    const char *w = where ? where : "BApplicationBase";
    B_QS(BApplicationBase);
    return bTest(qs, w, "There must be a BApplicationBase instance");
}

bool BApplicationBasePrivate::testCoreUnique()
{
    return bTest(!qs_func(), "BApplicationBase", "There must be only one instance of BApplicationBase");
}

BApplicationBase::LocaleSupportInfo BApplicationBasePrivate::createLocaleSupportInfo()
{
    BApplicationBase::LocaleSupportInfo info;
    info.locale = QLocale(QLocale::English);
    info.supports = 0;
    info.total = 0;
    return info;
}

/*============================== Public methods ============================*/

void BApplicationBasePrivate::init()
{
    destructorCalled = false;
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtcore);
    Q_INIT_RESOURCE(beqt_translations);
#endif
    bTest(QCoreApplication::instance(), "BApplicationBase", "Missing QCoreApplication instance");
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
    connect(languageChangeProxy, SIGNAL(triggered()), this, SLOT(languageChanged()));
    languageChangeEventProxy = new BSignalDelayProxy(100, 200, this);
    connect(languageChangeEventProxy, SIGNAL(triggered()), this, SLOT(sendLanguageChangeEvent()));
    blockLanguageChangeEvent = true;
}

bool BApplicationBasePrivate::eventFilter(QObject *o, QEvent *e)
{
    if (!blockLanguageChangeEvent || e->type() != QEvent::LanguageChange || o != QCoreApplication::instance())
        return BBaseObjectPrivate::eventFilter(o, e);
    languageChangeEventProxy->trigger();
    return true;
}

QString BApplicationBasePrivate::getAppName() const
{
    if ( !appName.isEmpty() )
        return appName;
    QString an = QCoreApplication::applicationName();
    if ( !bTest(!an.isEmpty(), "BApplicationBase", "missing application name") )
        return "";
    appName = an;
    return an;
}

QString BApplicationBasePrivate::getOrgName() const
{
    if ( !orgName.isEmpty() )
        return orgName;
    QString on = QCoreApplication::organizationName();
    if ( !bTest(!on.isEmpty(), "BApplicationBase", "Missing organization name") )
        return "";
    orgName = on;
    return on;
}

QString BApplicationBasePrivate::getAppPath() const
{
    if ( !appPath.isEmpty() )
        return appPath;
    QString ap = QCoreApplication::applicationDirPath().remove( QRegExp("/(b|B)(i|I)(n|N)$") );
    if ( !bTest(QFileInfo(ap).isDir(), "BApplicationBasePrivate", "Unable to get application directory") )
        return "";
    appPath = ap;
    return ap;
}

QString BApplicationBasePrivate::getUserPrefix() const
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

QString BApplicationBasePrivate::getSharedPrefix(bool forPlugins) const
{
#if defined(Q_OS_LINUX)
    if (forPlugins)
        return getSharedPrefixPlugins();
#endif
    getIsPortable();
    if (!sharedPrefix.isEmpty())
        return sharedPrefix;
#if defined(Q_OS_MAC)
    QString an = getAppName();
    QString on = getOrgName();
    if (an.isEmpty() || on.isEmpty())
        return "";
    sharedPrefix = "/Library/Application Support/" + on + "/" + an;
#elif defined (Q_OS_LINUX)
    QString an = getAppName();
    if (an.isEmpty())
        return "";
    sharedPrefix = "/usr/share/" + toLowerNoSpaces(an);
#elif defined(Q_OS_WIN)
    QString ap = getAppPath();
    if (ap.isEmpty())
        return "";
    sharedPrefix = ap;
#endif
    return sharedPrefix;
}

#if defined(Q_OS_LINUX)
QString BApplicationBasePrivate::getSharedPrefixPlugins() const
{
    getIsPortable();
    if (!sharedPrefixPlugins.isEmpty())
        return sharedPrefixPlugins;
    QString an = getAppName();
    if (an.isEmpty())
        return "";
    sharedPrefixPlugins = "/usr/lib/" + toLowerNoSpaces(an);
    return sharedPrefixPlugins;
}
#endif

#if defined(Q_OS_MAC)
QString BApplicationBasePrivate::getBundlePrefix() const
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

bool BApplicationBasePrivate::getIsPortable() const
{
    QString ap = getAppPath();
    QString an = getAppName();
    if ( ap.isEmpty() || an.isEmpty() )
        return false;
    bool b = QFileInfo(confFileName(ap, an)).isFile();
    if (b)
    {
        userPrefix = ap;
        sharedPrefix = ap;
#if defined(Q_OS_LINUX)
        sharedPrefixPlugins = ap;
#endif
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
#if defined(Q_OS_LINUX)
        if (ap == sharedPrefixPlugins)
            sharedPrefixPlugins.clear();
#endif
#if defined(Q_OS_MAC)
        if (ap == bundlePrefix)
            bundlePrefix.clear();
#endif
    }
    return b;
}

QLocale BApplicationBasePrivate::getLocale() const
{
    return settings->value("BeQt/Core/locale", QLocale::system()).toLocale();
}

void BApplicationBasePrivate::setLocale(const QLocale &l)
{
    foreach (BTranslator *t, translators)
        t->setLocale(l);
    settings->setValue("BeQt/Core/locale", l);
    languageChangeProxy->trigger();
}

QStringList BApplicationBasePrivate::getDeactivatedPlugins() const
{
    return settings->value("BeQt/Core/deactivated_plugins").toStringList();
}

void BApplicationBasePrivate::setDeactivatedPlugins(const QStringList &list)
{
    QStringList nlist = list;
    nlist.removeAll("");
    nlist.removeDuplicates();
    settings->setValue("BeQt/Core/deactivated_plugins", nlist);
}

void BApplicationBasePrivate::addDeactivatedPlugin(const QString &pluginName)
{
    if ( pluginName.isEmpty() )
        return;
    QStringList list = getDeactivatedPlugins();
    if ( list.contains(pluginName) )
        return;
    list << pluginName;
    setDeactivatedPlugins(list);
}

QSettings *BApplicationBasePrivate::createSettingsInstance(const QString &fileName) const
{
    QSettings *s = new QSettings(confFileName(getUserPrefix(), fileName), QSettings::IniFormat);
    s->setIniCodec("UTF-8");
    return s;
}

QString BApplicationBasePrivate::confFileName(const QString &path, const QString &name) const
{
    if ( path.isEmpty() || name.isEmpty() )
        return QString();
    QString bfn = name;
#if defined(Q_OS_UNIX)
    bfn = toLowerNoSpaces(bfn);
#endif
    return path + "/settings/" + bfn + ".conf";
}

QString BApplicationBasePrivate::prefix(BApplicationBase::ResourcesType type, bool forPlugins) const
{
    switch (type)
    {
    case BApplicationBase::UserResources :
        return getUserPrefix();
    case BApplicationBase::SharedResources :
        return getSharedPrefix(forPlugins);
#if defined(Q_OS_MAC)
    case BApplicationBase::BundleResources :
        return getBundlePrefix();
#endif
    case BApplicationBase::BuiltinResources:
        return ":";
    default:
        return "";
    }
}

void BApplicationBasePrivate::pluginActivated(BPluginWrapper *pluginWrapper)
{
    QStringList list = getDeactivatedPlugins();
    list.removeAll(pluginWrapper->name());
    setDeactivatedPlugins(list);
    emitPluginActivated(pluginWrapper);
}

void BApplicationBasePrivate::pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper)
{
    if (!destructorCalled)
        addDeactivatedPlugin(pluginWrapper->name());
    emitPluginAboutToBeDeactivated(pluginWrapper);
}

void BApplicationBasePrivate::installTranslator(BTranslator *translator)
{
    if (!translator || !translator->isValid() || translators.contains(translator->fileName()))
        return;
    translator->setLocale(getLocale());
    if (!translator->d_func()->install())
        return;
    translators.insert(translator->fileName(), translator);
    languageChangeProxy->trigger();
}

void BApplicationBasePrivate::removeTranslator(BTranslator *translator)
{
    if (!translator || !translators.contains(translator->fileName()))
        return;
    translators.remove(translator->fileName());
    translator->d_func()->remove();
    languageChangeProxy->trigger();
}

void BApplicationBasePrivate::loadSettings()
{
    if ( settings.isNull() )
        return;
    emitSettingsLoaded(settings.data());
}

void BApplicationBasePrivate::saveSettings()
{
    if ( settings.isNull() )
        return;
    emitSettingsSaved(settings.data());
    if ( !settings.isNull() )
        settings->sync();
}

/*============================== Public slots ==============================*/

void BApplicationBasePrivate::initSettings()
{
    settings = createSettingsInstance( getAppName() );
    if ( !settings.isNull() )
        connect( settings.data(), SIGNAL( destroyed() ), this, SLOT( initSettings() ) );
}

void BApplicationBasePrivate::sendLanguageChangeEvent()
{
    blockLanguageChangeEvent = false;
    QEvent e(QEvent::LanguageChange);
    QCoreApplication::sendEvent(qApp, &e);
    blockLanguageChangeEvent = true;
}

void BApplicationBasePrivate::languageChanged()
{
    emitLanguageChanged();
}

/*============================================================================
================================ BApplicationBase ============================
============================================================================*/

/*============================== Public constructors =======================*/

BApplicationBase::~BApplicationBase()
{
    ds_func()->destructorCalled = true;
    foreach (BTranslator *t, d_func()->translators)
        delete t;
    foreach (BPluginWrapper *pw, d_func()->plugins)
        delete pw;
    _m_self = 0;
}

/*============================== Protected constructors ====================*/

BApplicationBase::BApplicationBase(BApplicationBasePrivate &d, const QString &applicationName) :
    BBaseObject(d)
{
    if (!applicationName.isEmpty())
        QCoreApplication::setApplicationName(applicationName);
    d_func()->init();
    BApplicationBasePrivate::testCoreUnique();
    _m_self = this;
}

BApplicationBase::BApplicationBase(BApplicationBasePrivate &d, const InitialSettings &s) :
    BBaseObject(d)
{
    if (!s.applicationName.isEmpty())
        QCoreApplication::setApplicationName(s.applicationName);
    if (!s.organizationName.isEmpty())
        QCoreApplication::setOrganizationName(s.organizationName);
    if (s.organizationDomain.isValid())
        QCoreApplication::setOrganizationDomain(s.organizationDomain.toString());
    if (s.applicationVersion.isValid())
        QCoreApplication::setApplicationVersion(s.applicationVersion.toString());
    d_func()->init();
    BApplicationBasePrivate::testCoreUnique();
    _m_self = this;
}

/*============================== Static public methods =====================*/

BApplicationBase *BApplicationBase::binstance()
{
    return _m_self;
}

QString BApplicationBase::location(Location loc, ResourcesType type)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return "";
    QDir d(ds_func()->prefix(type, PluginsPath == loc) + "/" + BApplicationBasePrivate::subdir(loc));
    return d.exists() ? d.absolutePath() : "";
}

QString BApplicationBase::location(const QString &subdir, ResourcesType type)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return "";
    if (subdir.isEmpty())
        return "";
    bool forPlugins = false;
#if defined(Q_OS_LINUX)
    forPlugins = !subdir.compare("plugins", Qt::CaseSensitive);
#endif
    QDir d(ds_func()->prefix(type, forPlugins) + "/" + subdir);
    return d.exists() ? d.absolutePath() : "";
}

QStringList BApplicationBase::locations(Location loc)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
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

QStringList BApplicationBase::locations(const QString &subdir)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
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

QSettings *BApplicationBase::settingsInstance()
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return 0;
    return ds_func()->settings.data();
}

bool BApplicationBase::isPortable()
{
    if (!BApplicationBasePrivate::testCoreInit())
        return false;
    return ds_func()->getIsPortable();
}

void BApplicationBase::registerPluginWrapper(BPluginWrapper *plugin)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return;
    if (!plugin)
        return;
    B_DS(BApplicationBase);
    if ( ds->plugins.contains(plugin) )
        return;
    ds->plugins << plugin;
}

void BApplicationBase::unregisterPluginWrapper(BPluginWrapper *plugin)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return;
    if (!plugin)
        return;
    ds_func()->plugins.removeAll(plugin);
}

void BApplicationBase::loadPlugins(const QStringList &acceptableTypes, InterfaceTestFunction function)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    B_DS(BApplicationBase);
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
        QStringList files = dir.entryList(BApplicationBasePrivate::PluginSuffixes, QDir::Files);
        foreach (QString file, files)
        {
            BPluginWrapper *pw = new BPluginWrapper(dir.absoluteFilePath(file));
            if (!pw->load())
            {
                pw->deleteLater();
                continue;
            }
            ds->plugins << pw;
            if (ds->getDeactivatedPlugins().contains(pw->name()))
                pw->unload();
            else
                pw->activate();
        }
    }
}

QList<BPluginWrapper *> BApplicationBase::pluginWrappers(const QString &type)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return QList<BPluginWrapper *>();
    QList<BPluginWrapper *> list;
    foreach (BPluginWrapper *pw, ds_func()->plugins)
        if (type.isEmpty() || pw->type() == type)
            list << pw;
    return list;
}

void BApplicationBase::binstallTranslator(BTranslator *translator)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->installTranslator(translator);
}

void BApplicationBase::bremoveTranslator(BTranslator *translator)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->removeTranslator(translator);
}

QList<BTranslator *> BApplicationBase::translators()
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return QList<BTranslator *>();
    return ds_func()->translators.values();
}

BTranslator *BApplicationBase::translator(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return 0;
    return ds_func()->translators.value(fileName);
}

void BApplicationBase::setLocale(const QLocale &l)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    if (l == locale())
        return;
    ds_func()->setLocale(l);
}

QLocale BApplicationBase::locale()
{
    if (!BApplicationBasePrivate::testCoreInit())
        return QLocale::system();
    return ds_func()->getLocale();
}

QList<BApplicationBase::LocaleSupportInfo> BApplicationBase::availableLocales(bool alwaysIncludeEnglish)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return QList<BApplicationBase::LocaleSupportInfo>();
    B_DS(BApplicationBase);
    QList< QList<QLocale> > lll;
    foreach (BTranslator *t, ds->translators)
        lll << t->availableLocales();
    QList<QLocale> llist;
    QList<BApplicationBase::LocaleSupportInfo> list;
    if (alwaysIncludeEnglish)
    {
        LocaleSupportInfo en = BApplicationBasePrivate::createLocaleSupportInfo();
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
                LocaleSupportInfo lsi = BApplicationBasePrivate::createLocaleSupportInfo();
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

void BApplicationBase::loadSettings()
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return;
    ds_func()->loadSettings();
}

void BApplicationBase::saveSettings()
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->saveSettings();
}

void BApplicationBase::setApplicationCopyrightPeriod(const QString &s)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appCopyrightYears = s;
}

void BApplicationBase::setApplicationDescription(const QString &s)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appDescription = s;
    ds_func()->appDescriptionFileName.clear();
}

void BApplicationBase::setApplicationDescriptionFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appDescriptionFileName = fileName;
    ds_func()->appDescription.clear();
}

void BApplicationBase::setApplicationChangeLog(const QString &s)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appChangeLog = s;
    ds_func()->appChangeLogFileName.clear();
}

void BApplicationBase::setApplicationChangeLogFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appChangeLogFileName = fileName;
    ds_func()->appChangeLog.clear();
}

void BApplicationBase::setApplicationLicense(const QString &s)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appLicense = s;
    ds_func()->appLicenseFileName.clear();
}

void BApplicationBase::setApplicationLicenseFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appLicenseFileName = fileName;
    ds_func()->appLicense.clear();
}

void BApplicationBase::setApplicationAuthors(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appAuthorsList = list;
    ds_func()->appAuthors->setFileName("");
}

void BApplicationBase::setApplicationAuthorsFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appAuthors->setFileName(fileName);
    ds_func()->appAuthorsList.clear();
}

void BApplicationBase::setApplicationTranslations(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appTranslationsList = list;
    ds_func()->appTranslations->setFileName("");
}

void BApplicationBase::setApplicationTranslationsFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appTranslations->setFileName(fileName);
    ds_func()->appTranslationsList.clear();
}

void BApplicationBase::setApplicationThanksTo(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appThanksToList = list;
    ds_func()->appThanksTo->setFileName("");
}

void BApplicationBase::setApplicationThanksToFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    ds_func()->appThanksTo->setFileName(fileName);
    ds_func()->appThanksToList.clear();
}

QString BApplicationBase::applicationInfo(About type, const QLocale &loc)
{
    if (!BApplicationBasePrivate::testCoreInit())
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

QString BApplicationBase::beqtInfo(About type, const QLocale &loc)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return "";
    QString bfn;
    switch (type)
    {
    case Copyringt:
        return tr("Copyright") + " (C) 2012-2014 Andrey Bogdanov [https://github.com/the-dark-angel/BeQt]";
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

void BApplicationBase::setLogger(BLogger *l)
{
    if (!BApplicationBasePrivate::testCoreInit())
        return;
    B_DS(BApplicationBase);
    if (ds->logger && (!ds->logger->parent() || ds->logger->parent() == ds_func()))
        ds->logger->deleteLater();
    ds->logger = l;
    if (l && !l->parent())
        l->setParent(ds_func());
}

BLogger *BApplicationBase::logger()
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return 0;
    return ds_func()->logger;
}

void BApplicationBase::log(const QString &text, BLogger::Level lvl)
{
    if ( !BApplicationBasePrivate::testCoreInit() )
        return;
    B_DS(BApplicationBase);
    if (!ds->logger)
        return;
    ds->logger->log(text, lvl);
}

/*============================== Static protected variables ================*/

BApplicationBase *BApplicationBase::_m_self = 0;
