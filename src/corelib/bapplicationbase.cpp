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
#include "babstractlocationprovider.h"

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

static QString toLowerNoSpaces(const QString &string)
{
    return string.toLower().replace(QRegExp("\\s"), "-");
}

static QString platformSpecificAppName(const QString &appName)
{
#if defined(Q_OS_UNIX)
    return appName.toLower().replace(QRegExp("\\s"), "-");
#endif
    return appName;
}

static QString confFileName(const QString &path, const QString &appName)
{
    if (path.isEmpty() || appName.isEmpty())
        return QString();
    return path + "/settings/" + platformSpecificAppName(appName) + ".conf";
}

/*============================================================================
================================ BInternalLocationProvider ===================
============================================================================*/

class BInternalLocationProvider : public BAbstractLocationProvider
{
public:
    explicit BInternalLocationProvider(const QString &appName, const QString &orgName, bool portable);
    ~BInternalLocationProvider();
public:
    QStringList locationNames() const;
    QString locationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
    bool canCreateLocationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
    bool createLocationPath(const QString &locationName, BApplicationBase::ResourceType type);
private:
    static QStringList createNames();
    static QString createAppName(const QString &appName);
    static QString createOrgName(const QString &orgName);
    static bool testPortable(BApplicationBase::Portability portability, const QString &appName);
    static QString createUserPrefix(const QString &appName, const QString &orgName);
    static QString createSharedPrefix();
    static QString sharedPrefix(const QString &sharedPrefix, bool plugins = false);
private:
    const QStringList Names;
    const QString AppName;
    const QString OrgName;
    const bool Portable;
    const QString UserPrefix;
    const QString SharedPrefix;
};

/*============================================================================
================================ BInternalLocationProvider ===================
============================================================================*/

/*============================== Public constructors =======================*/

BInternalLocationProvider::BInternalLocationProvider(const QString &appName, const QString &orgName, bool portable) :
    Names(createNames()), AppName(appName), OrgName(orgName), Portable(portable),
    UserPrefix(createUserPrefix(AppName, OrgName)), SharedPrefix(createSharedPrefix())
{

}

BInternalLocationProvider::~BInternalLocationProvider()
{
    //
}

/*============================== Public methods ============================*/

QStringList BInternalLocationProvider::locationNames() const
{
    return Names;
}

QString BInternalLocationProvider::locationPath(const QString &locationName, BApplicationBase::ResourceType type) const
{
    if (locationName.isEmpty() || !Names.contains(locationName))
        return QString();
    bool plugins = ("plugins" == locationName);
    QString prefix;
    switch (type)
    {
    case BApplicationBase::UserResource:
        prefix = Portable ? sharedPrefix(SharedPrefix, plugins) : UserPrefix;
        break;
    case BApplicationBase::SharedResource:
        prefix = sharedPrefix(SharedPrefix, plugins);
        break;
    case BApplicationBase::BuiltinResource:
        prefix = ":";
        break;
    default:
        break;
    }
    if (prefix.isEmpty())
        return QString();
    if (plugins)
        return prefix + "/lib/" + toLowerNoSpaces(AppName) + "/plugins";
    else if ("data" == locationName)
        return prefix;
    else if ("documentation" == locationName)
        return prefix + "/doc";
    else if ("settings" == locationName)
        return prefix + "/settings";
    else if ("translations" == locationName)
        return prefix + "/translations";
    else if ("beqt" == locationName)
        return prefix + "/beqt";
    return QString();
}

bool BInternalLocationProvider::canCreateLocationPath(const QString &locationName,
                                                      BApplicationBase::ResourceType type) const
{
    //TODO: For some locations there may not exist shared or user component
    if (!Names.contains(locationName))
        return false;
    if (BApplicationBase::UserResource == type)
        return true;
#if defined(Q_OS_WIN)
    if (BApplicationBase::SharedResource == type)
        return true;
#endif
    return false;
}

bool BInternalLocationProvider::createLocationPath(const QString &locationName, BApplicationBase::ResourceType type)
{
    if (!canCreateLocationPath(locationName, type))
        return false;
    QString s = locationPath(locationName, type);
    return !s.isEmpty() && BDirTools::mkpath(s);
}

/*============================== Static private methods ====================*/

QStringList BInternalLocationProvider::createNames()
{
    QStringList names;
    names << "data";
    names << "documentation";
    names << "plugins";
    names << "settings";
    names << "translations";
    names << "beqt";
    return names;
}

QString BInternalLocationProvider::createUserPrefix(const QString &appName, const QString &orgName)
{
    Q_UNUSED(orgName)
    if (appName.isEmpty())
        return QString();
#if !defined (Q_OS_LINUX)
    if (orgName.isEmpty())
        return QString();
#endif
#if defined(Q_OS_MAC)
    return QDir::homePath() + "/Library/Application Support/" + orgName + "/" + appName;
#elif defined (Q_OS_LINUX)
    return QDir::homePath() + "/." + platformSpecificAppName(appName);
#elif defined(Q_OS_WIN)
    if (QSysInfo::windowsVersion() ==  QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
        return QDir::homePath() + "/Application Data/" + orgName + "/" + appName;
    else
        return QDir::homePath() + "/AppData/Roaming/" + orgName + "/" + appName;
#endif
}

QString BInternalLocationProvider::createSharedPrefix()
{
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../shared");
}

QString BInternalLocationProvider::sharedPrefix(const QString &sharedPrefix, bool plugins)
{
    if (sharedPrefix.isEmpty())
        return QString();
    return plugins ? QDir::cleanPath(sharedPrefix + "/..") : sharedPrefix;
}

/*============================================================================
================================ BApplicationBasePrivate =====================
============================================================================*/

/*============================== Static public constants ===================*/

#if defined(Q_OS_MAC)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList() << "*.dylib" << "*.so";
#elif defined(Q_OS_LINUX)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList("*.so");
#elif defined(Q_OS_WIN)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList("*.dll");
#endif

/*============================== Static private constants ==================*/

const QString BApplicationBasePrivate::DefaultAppName = "BeQt-based Application";
const QString BApplicationBasePrivate::DefaultOrgName = "BeQt";

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
    foreach (BAbstractLocationProvider *p, locationProviders)
        delete p;
    if (!settings.isNull()) {
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

bool BApplicationBasePrivate::testInit(const char *where)
{
    const char *w = where ? where : "BApplicationBase";
    B_QS(BApplicationBase);
    return bTest(qs, w, "There must be a BApplicationBase instance");
}

bool BApplicationBasePrivate::testUnique()
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

void BApplicationBasePrivate::init(BApplicationBase::Portability portability)
{
    destructorCalled = false;
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtcore);
    Q_INIT_RESOURCE(beqt_translations);
#endif
    appName = QCoreApplication::applicationName();
    if (!bTest(!appName.isEmpty(), "BApplicationBase", "Application name not specified"))
        appName = DefaultAppName;
    orgName = QCoreApplication::organizationName();
    if (!bTest(!orgName.isEmpty(), "BApplicationBase", "Organization name not specified"))
        orgName = DefaultOrgName;
    if (BApplicationBase::AutoDetect == portability) {
        portable = QFileInfo(confFileName(QCoreApplication::applicationDirPath() + "/..", appName)).isFile();
    } else {
        portable = (BApplicationBase::Portable == portability);
    }
    internalLocationProvider = new BInternalLocationProvider(appName, orgName, portable);
    locationProviders << internalLocationProvider;
    QString fn = confFileName(location("settings", BApplicationBase::UserResource), appName);
    settings = new QSettings(fn, QSettings::IniFormat);
    settings->setIniCodec("UTF-8");
    QCoreApplication::instance()->installEventFilter(this);
    //infos
    QStringList locs;
    locs << location("data", BApplicationBase::SharedResource);
    locs << location("data", BApplicationBase::BuiltinResource);
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

QString BApplicationBasePrivate::location(const QString &name, BApplicationBase::ResourceType type) const
{
    foreach (BAbstractLocationProvider *p, locationProviders) {
        QString s = p->locationPath(name, type);
        if (s.isEmpty())
            continue;
        if (QDir(s).exists())
            return s;
        return p->createLocationPath(name, type) ? s : QString();
    }
    return QString();
}

QStringList BApplicationBasePrivate::locations(const QString &name) const
{
    QStringList sl;
    QString s = location(name, BApplicationBase::UserResource);
    if (!s.isEmpty())
        sl << s;
    s = location(name, BApplicationBase::SharedResource);
    if (!s.isEmpty())
        sl << s;
    s = location(name, BApplicationBase::BuiltinResource);
    if (!s.isEmpty())
        sl << s;
    return sl;
}

QLocale BApplicationBasePrivate::getLocale() const
{
    return !settings.isNull() ? settings->value("BeQt/Core/locale", QLocale::system()).toLocale() : QLocale::system();
}

void BApplicationBasePrivate::setLocale(const QLocale &l)
{
    foreach (BTranslator *t, translators)
        t->setLocale(l);
    if (!settings.isNull())
        settings->setValue("BeQt/Core/locale", l);
    languageChangeProxy->trigger();
}

bool BApplicationBasePrivate::isPluginActivated(const QString &pluginName) const
{
    if (pluginName.isEmpty())
        return false;
    if (settings.isNull())
        return true;
    return settings->value("BeQt/Core/deactivated_plugins").toStringList().contains(pluginName);
}

void BApplicationBasePrivate::setPluginActivated(const QString &pluginName, bool activated)
{
    if (pluginName.isEmpty())
        return;
    if (settings.isNull())
        return;
    QStringList list = settings->value("BeQt/Core/deactivated_plugins").toStringList();
    if (activated)
        list.removeAll(pluginName);
    else if (!list.contains(pluginName))
        list << pluginName;
    settings->setValue("BeQt/Core/deactivated_plugins", list);
}

void BApplicationBasePrivate::installTranslator(BTranslator *translator)
{
    if (!translator || !translator->isValid() || translators.contains(translator->fileName()))
        return;
    translator->setLocale(getLocale());
    translators.insert(translator->fileName(), translator);
    translator->install();
    languageChangeProxy->trigger();
}

void BApplicationBasePrivate::removeTranslator(BTranslator *translator)
{
    if (!translator || !translators.contains(translator->fileName()))
        return;
    translators.remove(translator->fileName());
    translator->remove();
    languageChangeProxy->trigger();
}

void BApplicationBasePrivate::installPlugin(BPluginWrapper *plugin)
{
    if (!plugin)
        return;
    if (plugins.contains(plugin))
        return;
    plugins.insert(plugin, plugin);
    connect(plugin, SIGNAL(activated()), this, SLOT(pluginActivated()));
    connect(plugin, SIGNAL(aboutToBeDeactivated()), this, SLOT(pluginAboutToBeDeactivated()));
    connect(plugin, SIGNAL(destroyed(QObject*)), this, SLOT(pluginDestroyed(QObject*)));
}

void BApplicationBasePrivate::removePlugin(QObject *plugin)
{
    if (!plugin)
        return;
    if (!plugins.contains(plugin))
        return;
    disconnect(plugin, SIGNAL(activated()), this, SLOT(pluginActivated()));
    disconnect(plugin, SIGNAL(aboutToBeDeactivated()), this, SLOT(pluginAboutToBeDeactivated()));
    disconnect(plugin, SIGNAL(destroyed(QObject*)), this, SLOT(pluginDestroyed(QObject*)));
    plugins.remove(plugin);
}

/*============================== Public slots ==============================*/


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

void BApplicationBasePrivate::pluginActivated()
{
    BPluginWrapper *pw = qobject_cast<BPluginWrapper *>(sender());
    if (!pw)
        return;
    if (!plugins.contains(pw))
        return;
    emitPluginActivated(pw);
    setPluginActivated(pw->name(), true);
}

void BApplicationBasePrivate::pluginAboutToBeDeactivated()
{
    BPluginWrapper *pw = qobject_cast<BPluginWrapper *>(sender());
    if (!pw)
        return;
    if (!plugins.contains(pw))
        return;
    emitPluginAboutToBeDeactivated(pw);
    setPluginActivated(pw->name(), false);
}

void BApplicationBasePrivate::pluginDestroyed(QObject *obj)
{
    removePlugin(obj);
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

BApplicationBase::BApplicationBase(BApplicationBasePrivate &d, const QString &applicationName,
                                   const QString &organizationName) :
    BBaseObject(d)
{
    if (!applicationName.isEmpty())
        QCoreApplication::setApplicationName(applicationName);
    if (!organizationName.isEmpty())
        QCoreApplication::setOrganizationName(organizationName);
    d_func()->init();
    BApplicationBasePrivate::testUnique();
    _m_self = this;
}

BApplicationBase::BApplicationBase(BApplicationBasePrivate &d, const InitialSettings &s) :
    BBaseObject(d)
{
    if (!s.applicationName.isEmpty())
        QCoreApplication::setApplicationName(s.applicationName);
    if (!s.organizationName.isEmpty())
        QCoreApplication::setOrganizationName(s.organizationName);
    d_func()->init(s.portability);
    BApplicationBasePrivate::testUnique();
    _m_self = this;
}

/*============================== Static public methods =====================*/

BApplicationBase *BApplicationBase::binstance()
{
    return _m_self;
}

void BApplicationBase::installLocationProvider(BAbstractLocationProvider *p)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (!p)
        return;
    if (ds_func()->locationProviders.contains(p))
        return;
    ds_func()->locationProviders << p;
}

void BApplicationBase::installLocationProviders(const QList<BAbstractLocationProvider *> &list)
{
    foreach (BAbstractLocationProvider *p, list)
        installLocationProvider(p);
}

void BApplicationBase::removeLocationProvider(BAbstractLocationProvider *p)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (!p)
        return;
    if (!ds_func()->locationProviders.contains(p))
        return;
    ds_func()->locationProviders.removeAll(p);
}

void BApplicationBase::removeLocationProviders(const QList<BAbstractLocationProvider *> &list)
{
    foreach (BAbstractLocationProvider *p, list)
        removeLocationProvider(p);
}

QString BApplicationBase::location(Location loc, ResourceType type)
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    QString name;
    switch (loc)
    {
    case DataPath:
        name = "data";
        break;
    case DocumentationPath:
        name = "documentation";
        break;
    case PluginsPath:
        name = "plugins";
        break;
    case SettingsPath:
        name = "settings";
        break;
    case TranslationsPath:
        name = "translations";
        break;
    case BeqtPath:
        name = "beqt";
        break;
    default:
        break;
    }
    if (name.isEmpty())
        return QString();
    return ds_func()->location(name, type);
}

QString BApplicationBase::location(const QString &subdir, ResourceType type)
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    if (subdir.isEmpty())
        return QString();
    return ds_func()->location(subdir, type);
}

QStringList BApplicationBase::locations(Location loc)
{
    if (!BApplicationBasePrivate::testInit())
        return QStringList();
    QStringList sl;
    sl << location(loc, UserResource);
    sl << location(loc, SharedResource);
    sl << location(loc, BuiltinResource);
    sl.removeAll("");
    sl.removeDuplicates();
    return sl;
}

QStringList BApplicationBase::locations(const QString &subdir)
{
    if (!BApplicationBasePrivate::testInit())
        return QStringList();
    QStringList sl;
    sl << location(subdir, UserResource);
    sl << location(subdir, SharedResource);
    sl << location(subdir, BuiltinResource);
    sl.removeAll("");
    sl.removeDuplicates();
    return sl;
}

QSettings *BApplicationBase::settingsInstance()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->settings.data();
}

bool BApplicationBase::isPortable()
{
    if (!BApplicationBasePrivate::testInit())
        return false;
    return ds_func()->portable;
}

void BApplicationBase::installPlugin(BPluginWrapper *plugin)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->installPlugin(plugin);
}

void BApplicationBase::removePlugin(BPluginWrapper *plugin)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->removePlugin(plugin);
}

void BApplicationBase::loadPlugins()
{
    loadPlugins(BPluginWrapper::acceptableTypes(), BPluginWrapper::interfaceTestFunction());
}

void BApplicationBase::loadPlugins(const QStringList &acceptableTypes)
{
    loadPlugins(acceptableTypes, BPluginWrapper::interfaceTestFunction());
}

void BApplicationBase::loadPlugins(const QStringList &acceptableTypes, InterfaceTestFunction function)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    QStringList prevAcceptableTypes = BPluginWrapper::acceptableTypes();
    InterfaceTestFunction prevFunction = BPluginWrapper::interfaceTestFunction();
    BPluginWrapper::setAcceptableTypes(acceptableTypes);
    BPluginWrapper::setInterfaceTestFunction(function);
    foreach (QString dir, locations(PluginsPath))
    {
        foreach (QString file, BDirTools::entryList(dir, BApplicationBasePrivate::PluginSuffixes, QDir::Files))
        {
            BPluginWrapper *pw = new BPluginWrapper(file);
            if (!pw->load())
            {
                pw->deleteLater();
                continue;
            }
            installPlugin(pw);
            if (ds_func()->isPluginActivated(pw->name()))
                pw->activate();
            else
                pw->unload();
        }
    }
    BPluginWrapper::setAcceptableTypes(prevAcceptableTypes);
    BPluginWrapper::setInterfaceTestFunction(prevFunction);
}

void BApplicationBase::unloadPlugins(bool remove)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    foreach (BPluginWrapper *pw, ds_func()->plugins) {
        if (pw->isLoaded())
            pw->unload();
        if (remove)
            ds_func()->removePlugin(pw);
    }
}

QList<BPluginWrapper *> BApplicationBase::pluginWrappers(const QString &type)
{
    if (!BApplicationBasePrivate::testInit())
        return QList<BPluginWrapper *>();
    QList<BPluginWrapper *> list;
    foreach (BPluginWrapper *pw, ds_func()->plugins)
        if (type.isEmpty() || pw->type() == type)
            list << pw;
    return list;
}

void BApplicationBase::installBeqtTranslator(BTranslator *translator)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->installTranslator(translator);
}

void BApplicationBase::installBeqtTranslator(const QString &id)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (id.isEmpty())
        return;
    if (ds_func()->translators.contains(id))
        return;
    ds_func()->installTranslator(new BTranslator(id));
}

void BApplicationBase::removeBeqtTranslator(BTranslator *translator)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->removeTranslator(translator);
}

void BApplicationBase::removeBeqtTranslator(const QString &id)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (id.isEmpty())
        return;
    if (!ds_func()->translators.contains(id))
        return;
    ds_func()->removeTranslator(ds_func()->translators.value(id));
}

QList<BTranslator *> BApplicationBase::beqtTranslators()
{
    if ( !BApplicationBasePrivate::testInit() )
        return QList<BTranslator *>();
    return ds_func()->translators.values();
}

BTranslator *BApplicationBase::beqtTranslator(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->translators.value(fileName);
}

void BApplicationBase::setLocale(const QLocale &l)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (l == locale())
        return;
    ds_func()->setLocale(l);
}

QLocale BApplicationBase::locale()
{
    if (!BApplicationBasePrivate::testInit())
        return QLocale::system();
    return ds_func()->getLocale();
}

QList<BApplicationBase::LocaleSupportInfo> BApplicationBase::availableLocales(bool alwaysIncludeEnglish)
{
    if ( !BApplicationBasePrivate::testInit() )
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

void BApplicationBase::setApplicationCopyrightPeriod(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appCopyrightYears = s;
}

void BApplicationBase::setApplicationExtendedCopyrightInfo(const QList<CopyrightInfo> &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->copyrightInfos = list;
    foreach (int i, bRangeR(ds_func()->copyrightInfos.size() - 1, 0)) {
        const BApplicationBase::CopyrightInfo &ci = ds_func()->copyrightInfos.at(i);
        if (ci.owner.isEmpty() || ci.period.isEmpty())
            ds_func()->copyrightInfos.removeAt(i);
    }
}

void BApplicationBase::setApplicationDescription(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appDescription = s;
    ds_func()->appDescriptionFileName.clear();
}

void BApplicationBase::setApplicationDescriptionFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appDescriptionFileName = fileName;
    ds_func()->appDescription.clear();
}

void BApplicationBase::setApplicationChangeLog(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appChangeLog = s;
    ds_func()->appChangeLogFileName.clear();
}

void BApplicationBase::setApplicationChangeLogFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appChangeLogFileName = fileName;
    ds_func()->appChangeLog.clear();
}

void BApplicationBase::setApplicationLicense(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appLicense = s;
    ds_func()->appLicenseFileName.clear();
}

void BApplicationBase::setApplicationLicenseFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appLicenseFileName = fileName;
    ds_func()->appLicense.clear();
}

void BApplicationBase::setApplicationAuthors(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appAuthorsList = list;
    ds_func()->appAuthors->setFileName("");
}

void BApplicationBase::setApplicationAuthorsFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appAuthors->setFileName(fileName);
    ds_func()->appAuthorsList.clear();
}

void BApplicationBase::setApplicationTranslations(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appTranslationsList = list;
    ds_func()->appTranslations->setFileName("");
}

void BApplicationBase::setApplicationTranslationsFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appTranslations->setFileName(fileName);
    ds_func()->appTranslationsList.clear();
}

void BApplicationBase::setApplicationThanksTo(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appThanksToList = list;
    ds_func()->appThanksTo->setFileName("");
}

void BApplicationBase::setApplicationThanksToFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appThanksTo->setFileName(fileName);
    ds_func()->appThanksToList.clear();
}

QString BApplicationBase::applicationCopyrightPeriod()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appCopyrightYears;
}

QList<BApplicationBase::CopyrightInfo> BApplicationBase::applicationExtendedCopyrightInfo()
{
    if (!BApplicationBasePrivate::testInit())
        return QList<BApplicationBase::CopyrightInfo>();
    return ds_func()->copyrightInfos;
}

QString BApplicationBase::applicationDescription()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appDescription;
}

QString BApplicationBase::applicationDescriptionFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appDescriptionFileName;
}

QString BApplicationBase::applicationChangeLog()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appChangeLog;
}

QString BApplicationBase::applicationChangeLogFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appChangeLogFileName;
}

QString BApplicationBase::applicationLicense()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appLicense;
}

QString BApplicationBase::applicationLicenseFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appLicenseFileName;
}

BPersonInfoList BApplicationBase::applicationAuthors()
{
    if (!BApplicationBasePrivate::testInit())
        return BPersonInfoList();
    return ds_func()->appAuthorsList;
}

BPersonInfoProvider *BApplicationBase::applicationAuthorsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->appAuthors;
}

QString BApplicationBase::applicationAuthorsFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appAuthors ? ds_func()->appAuthors->fileName() : QString();;
}

BPersonInfoList BApplicationBase::applicationTranslations()
{
    if (!BApplicationBasePrivate::testInit())
        return BPersonInfoList();
    return ds_func()->appTranslationsList;
}

BPersonInfoProvider *BApplicationBase::applicationTranslationsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->appTranslations;
}

QString BApplicationBase::applicationTranslationsFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appTranslations ? ds_func()->appTranslations->fileName() : QString();
}

BPersonInfoList BApplicationBase::applicationThanksTo()
{
    if (!BApplicationBasePrivate::testInit())
        return BPersonInfoList();
    return ds_func()->appThanksToList;
}

BPersonInfoProvider *BApplicationBase::applicationThanksToProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->appThanksTo;
}

QString BApplicationBase::applicationThanksToFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appThanksTo ? ds_func()->appThanksTo->fileName() : QString();
}

BPersonInfoProvider *BApplicationBase::beqtAuthorsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->beqtAuthors;
}

BPersonInfoProvider *BApplicationBase::beqtTranslationsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->beqtTranslations;
}

BPersonInfoProvider *BApplicationBase::beqtThanksToProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->beqtThanksTo;
}

QString BApplicationBase::applicationInfo(About type, const QLocale &loc)
{
    if (!BApplicationBasePrivate::testInit())
        return "";
    QString fn;
    switch (type)
    {
    case Copyringt:
    {
        QString crp = (!ds_func()->appCopyrightYears.isEmpty() ? (ds_func()->appCopyrightYears + " ") : QString());
        QString domain = QCoreApplication::organizationDomain();
        if (!domain.isEmpty())
            domain.prepend(" [").append("]");
        return tr("Copyright") + " (C) " + crp + ds_func()->orgName + domain;
    }
    case ExtendedCopyright:
    {
        QString s;
        foreach (const CopyrightInfo &ci, ds_func()->copyrightInfos)
        {
            if (ci.owner.isEmpty() || ci.period.isEmpty())
                continue;
            s += tr("Copyright") + " (C) " + ci.period + " " + ci.owner;
            if (!ci.email.isEmpty())
                s += " (" + ci.email + ")";
            s += "\n";
        }
        if (!s.isEmpty())
            s.remove(s.length() - 1, 1);
        return s;
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
    if (!BApplicationBasePrivate::testInit())
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
    QString dir = location(BeqtPath, SharedResource) + "/";
    QString fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    if (fn.isEmpty())
    {
        dir = location(BeqtPath, BuiltinResource) + "/";
        fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    }
    return BDirTools::readTextFile(fn, "UTF-8");
}

void BApplicationBase::setLogger(BLogger *l)
{
    if (!BApplicationBasePrivate::testInit())
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
    if ( !BApplicationBasePrivate::testInit() )
        return 0;
    return ds_func()->logger;
}

void BApplicationBase::log(const QString &text, BLogger::Level lvl)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    B_DS(BApplicationBase);
    if (!ds->logger)
        return;
    ds->logger->log(text, lvl);
}

/*============================== Static protected variables ================*/

BApplicationBase *BApplicationBase::_m_self = 0;
