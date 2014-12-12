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
#include "bapplicationbase_p.h"

#include "babstractlocationprovider.h"
#include "bdirtools.h"
#include "blogger.h"
#include "bpersoninfoprovider.h"
#include "bpluginwrapper.h"
#include "bsignaldelayproxy.h"
#include "btranslator.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QList>
#include <QLocale>
#include <QObject>
#include <QPointer>
#include <QRegExp>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QSysInfo>
#include <QtGlobal>
#include <QVariant>

/*============================================================================
================================ BInternalLocationProvider ===================
============================================================================*/

/*============================== Public constructors =======================*/

BInternalLocationProvider::BInternalLocationProvider(const QString &appName, const QString &orgName, bool portable) :
    AppName(appName), Names(createNames()), OrgName(orgName), Portable(portable),
     SharedPrefix(createSharedPrefix(appName)), UserPrefix(createUserPrefix(AppName, OrgName))
{
    //
}

BInternalLocationProvider::~BInternalLocationProvider()
{
    //
}

/*============================== Static private methods ====================*/

QStringList BInternalLocationProvider::createNames()
{
    QStringList names;
    names << "beqt/icons";
    names << "data";
    names << "doc";
    names << "icons";
    names << "plugins";
    names << "settings";
    names << "translations";
    names << "beqt";
    return names;
}

QString BInternalLocationProvider::createSharedPrefix(const QString &appName)
{
    if (appName.isEmpty())
        return QString();
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../share/"
                           + BApplicationBasePrivate::toLowerNoSpaces(appName));
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
    QString postfix = "/share/" + BApplicationBasePrivate::toLowerNoSpaces(appName);
#if defined(Q_OS_MAC)
    return QDir::homePath() + "/Library/Application Support/" + orgName + "/" + appName + postfix;
#elif defined (Q_OS_LINUX)
    return QDir::homePath() + "/." + BApplicationBasePrivate::platformSpecificAppName(appName) + postfix;
#elif defined(Q_OS_WIN)
    if (QSysInfo::windowsVersion() ==  QSysInfo::WV_XP || QSysInfo::windowsVersion() == QSysInfo::WV_2003)
        return QDir::homePath() + "/Application Data/" + orgName + "/" + appName + postfix;
    else
        return QDir::homePath() + "/AppData/Roaming/" + orgName + "/" + appName + postfix;
#endif
}

/*============================== Public methods ============================*/

bool BInternalLocationProvider::canCreateLocationPath(const QString &locationName,
                                                      BApplicationBase::ResourceType type) const
{
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

QStringList BInternalLocationProvider::locationNames() const
{
    return Names;
}

QString BInternalLocationProvider::locationPath(const QString &locationName, BApplicationBase::ResourceType type) const
{
    if (locationName.isEmpty() || !Names.contains(locationName))
        return QString();
    QString prefix;
    switch (type) {
    case BApplicationBase::UserResource:
        prefix = Portable ? SharedPrefix : UserPrefix;
        break;
    case BApplicationBase::SharedResource:
        prefix = SharedPrefix;
        break;
    case BApplicationBase::BuiltinResource:
        prefix = ":";
        break;
    default:
        break;
    }
    if (prefix.isEmpty())
        return QString();
    if ("data" == locationName)
        return prefix;
    else if ("plugins" == locationName)
        return QDir::cleanPath(prefix + "/../../lib/" + BApplicationBasePrivate::toLowerNoSpaces(AppName) + "/plugins");
    else
        return prefix + "/" + locationName;
}

/*============================================================================
================================ BApplicationBasePrivate =====================
============================================================================*/

/*============================== Static public constants ===================*/

const QString BApplicationBasePrivate::DefaultAppName = "BeQt-based Application";
const QString BApplicationBasePrivate::DefaultOrgName = "BeQt";

#if defined(Q_OS_MAC)
const QStringList BApplicationBasePrivate::PluginSuffixes = QStringList() << "*.dylib" << "*.so";
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
    foreach (BAbstractLocationProvider *p, locationProviders)
        delete p;
    locationProviders.clear();
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

QString BApplicationBasePrivate::confFileName(const QString &path, const QString &appName)
{
    if (path.isEmpty() || appName.isEmpty())
        return QString();
    return path + "/" + platformSpecificAppName(appName) + ".conf";
}

BApplicationBase::LocaleSupportInfo BApplicationBasePrivate::createLocaleSupportInfo()
{
    BApplicationBase::LocaleSupportInfo info;
    info.locale = QLocale(QLocale::English);
    info.supports = 0;
    info.total = 0;
    return info;
}

BApplicationBasePrivate *BApplicationBasePrivate::instance()
{
    return qs_func() ? qs_func()->ds_func() : 0;
}

QString BApplicationBasePrivate::platformSpecificAppName(const QString &appName)
{
#if defined(Q_OS_UNIX)
    return appName.toLower().replace(QRegExp("\\s"), "-");
#endif
    return appName;
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

QString BApplicationBasePrivate::toLowerNoSpaces(const QString &string)
{
    return string.toLower().replace(QRegExp("\\s"), "-");
}

/*============================== Public methods ============================*/

bool BApplicationBasePrivate::eventFilter(QObject *o, QEvent *e)
{
    if (!blockLanguageChangeEvent || e->type() != QEvent::LanguageChange || o != QCoreApplication::instance())
        return BBaseObjectPrivate::eventFilter(o, e);
    languageChangeEventProxy->trigger();
    return true;
}

QLocale BApplicationBasePrivate::getLocale() const
{
    return !settings.isNull() ? settings->value("BeQt/Core/locale", QLocale::system()).toLocale() : QLocale::system();
}

void BApplicationBasePrivate::init(BApplicationBase::Portability portability)
{
    destructorCalled = false;
#if defined(BEQT_BUILTIN_RESOURCES)
    Q_INIT_RESOURCE(beqtcore);
    Q_INIT_RESOURCE(beqt_translations);
#endif
    bRegister();
    appName = QCoreApplication::applicationName();
    if (!bTest(!appName.isEmpty(), "BApplicationBase", "Application name not specified"))
        appName = DefaultAppName;
    orgName = QCoreApplication::organizationName();
    if (!bTest(!orgName.isEmpty(), "BApplicationBase", "Organization name not specified"))
        orgName = DefaultOrgName;
    if (BApplicationBase::AutoDetect == portability) {
        QString path = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../share/" + toLowerNoSpaces(appName)
                                       + "/settings");
        portable = QFileInfo(confFileName(path, appName)).isFile();
    } else {
        portable = (BApplicationBase::Portable == portability);
    }
    internalLocationProvider = new BInternalLocationProvider(appName, orgName, portable);
    locationProviders << internalLocationProvider;
    QString fn = confFileName(location("settings", BApplicationBase::UserResource), appName);
    settings = new QSettings(fn, QSettings::IniFormat);
    settings->setIniCodec("UTF-8");
    QCoreApplication::instance()->installEventFilter(this);
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

void BApplicationBasePrivate::installTranslator(BTranslator *translator)
{
    if (!translator || !translator->isValid() || translators.contains(translator->fileName()))
        return;
    translator->setLocale(getLocale());
    translators.insert(translator->fileName(), translator);
    translator->install();
    languageChangeProxy->trigger();
}

bool BApplicationBasePrivate::isPluginActivated(const QString &pluginId) const
{
    if (pluginId.isEmpty())
        return false;
    if (settings.isNull())
        return true;
    return !settings->value("BeQt/Core/deactivated_plugins").toStringList().contains(pluginId);
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

void BApplicationBasePrivate::removePlugin(QObject *plugin)
{
    if (!plugin)
        return;
    if (!plugins.contains(plugin))
        return;
    plugins.remove(plugin);
}

void BApplicationBasePrivate::removeTranslator(BTranslator *translator)
{
    if (!translator || !translators.contains(translator->fileName()))
        return;
    translators.remove(translator->fileName());
    translator->remove();
    languageChangeProxy->trigger();
}

void BApplicationBasePrivate::setLocale(const QLocale &l)
{
    foreach (BTranslator *t, translators)
        t->setLocale(l);
    if (!settings.isNull())
        settings->setValue("BeQt/Core/locale", l);
    languageChangeProxy->trigger();
}

void BApplicationBasePrivate::setPluginActivated(const QString &pluginId, bool activated)
{
    if (pluginId.isEmpty())
        return;
    BPluginWrapper *pw = BApplicationBase::pluginWrapper(pluginId);
    if (!pw)
        return;
    pw->setActivated(activated);
    if (settings.isNull())
        return;
    QStringList list = settings->value("BeQt/Core/deactivated_plugins").toStringList();
    if (activated)
        list.removeAll(pluginId);
    else if (!list.contains(pluginId))
        list << pluginId;
    settings->setValue("BeQt/Core/deactivated_plugins", list);
}

/*============================== Public slots ==============================*/

void BApplicationBasePrivate::languageChanged()
{
    emitLanguageChanged();
}

void BApplicationBasePrivate::pluginAboutToBeDeactivated()
{
    BPluginWrapper *pw = qobject_cast<BPluginWrapper *>(sender());
    if (!pw)
        return;
    if (!plugins.contains(pw))
        return;
    emitPluginAboutToBeDeactivated(pw);
}

void BApplicationBasePrivate::pluginActivated()
{
    BPluginWrapper *pw = qobject_cast<BPluginWrapper *>(sender());
    if (!pw)
        return;
    if (!plugins.contains(pw))
        return;
    emitPluginActivated(pw);
}

void BApplicationBasePrivate::pluginDestroyed(QObject *obj)
{
    removePlugin(obj);
}

void BApplicationBasePrivate::sendLanguageChangeEvent()
{
    blockLanguageChangeEvent = false;
    QEvent e(QEvent::LanguageChange);
    QCoreApplication::sendEvent(qApp, &e);
    blockLanguageChangeEvent = true;
}

/*============================================================================
================================ BApplicationBase ============================
============================================================================*/

/*!
\macro bSettings()
\relates BApplicationBase
Equivalent of BApplicationBase::settingsInstance().
*/

/*!
\macro bLogger()
\relates BApplicationBase
Equivalent of BApplicationBase::logger().
*/

/*!
\macro bLog()
\relates BApplicationBase
Equivalent of BApplicationBase::log().
*/

/*!
\class BApplicationBase
\brief The BApplicationBase class provides application-wide resources, settings, translations and plugins system.

This class is not instantiated directly, but is inherited by BCoreApplication and BApplication classes, which also
inherit from QCoreApplication and QApplication, respectively.

\section1 Resources system

BApplicationBase uses UNIX-style resource structure. Let's say the main executable file of your application is located
in "/home/user/yourapp/bin", where "yourapp" is your application name. Then, BApplicationBase will search for resources
in "/home/user/yourapp/share/yourapp". Plugins will be searched for in "/home/user/yourapp/lib/yourapp/plugins".

Functions location() and locations() are used to get resources paths. In order to provide custom resources paths, use
installLocationProvider() or installLocationProviders().

\sa BAbstractLocationProvider, BLocationProvider

There are two modes of the application: portable and non-portable. An application is considered portable if
(considering the example above) a settings file "/home/user/yourapp/settings/yourapp.conf" is present. You may also
force an application to be portable by passing an InitialSettings object with InitialSettings::portability member set
to Portable to the BApplicationBase constructor.

When application is portable, the resources are searched for in "/home/user/yourapp/share/yourapp" directory and in the
":/" directory (Qt resources built into the executable). When appliction is non-portable, the resources are also
searched in OS-specific location ("/home/user/.yourapplication" on UNIX-like systems,
"/Library/Application Support/orgname/yourapp" on Mac OS X, "C:/Users/user/Application Data/orgname/yourapp" on Windows
XP and Windows Server 2003, and "C:/Users/User/AppData/Roaming/orgname/yourapp" on later versions of Windows). Here
"orgname" is your organization name.

\note On UNIX-like systems application name is lowercased, and spaces are replaced by hyphens.

\sa isPortable()

The following standard resource locations are available:

\table
\header
    \li Location
    \li Enum member
    \li Example path
\row
    \li BeQt icons
    \li BeqtIconsPath
    \li share/yourapp/beqt/icons
\row
    \li BeQt resources
    \li BeqtPath
    \li share/yourapp/beqt
\row
    \li All resources
    \li DataPath
    \li share/yourapp
\row
    \li Documentation
    \li DocumentationPath
    \li share/yourapp/doc
\row
    \li Icons
    \li IconsPath
    \li share/yourapp/icons
\row
    \li Plugins
    \li PluginsPath
    \li lib/yourapp/plugins
\row
    \li Settings
    \li SettingsPath
    \li share/yourapp/settings
\row
    \li Translations
    \li TranslationsPath
    \li share/yourapp/translations
\endtable

The resources types are:

\list
    \li BuiltinResource
    \li SharedResource
    \li UserResource
\endlist

The prefix for builtin resources paths is ":/".

The prefix for shared resources paths is "../share/yourapp" (relative
to the main application executable file).

The prexixes for user resources paths are:

\table
\header
    \li Operating system
    \li Prefix
\row
    \li UNIX-like
    \li /home/user/.yourapp
\row
    \li Mac OS X
    \li /Library/Application Support/orgname/yourapp
\row
    \li Windows XP / Windows Server 2003
    \li C:/Users/user/Application Data/orgname/yourapp
\row
    \li Later Windows
    \li C:/Users/User/AppData/Roaming/orgname/yourapp
\endtable

\section1 Plugins system

BApplicationBase uses BPluginWrapper class to load and manage plugins. Plugins are loaded from standard locations (for
example, "/usr/share/yourapp/plugins" on UNIX-like systems) and registered in the BApplicationBase instance. Plugins
marked as deactivated on previous application launch are not activated on later launches.

Plugins loaded by loadPlugins() call are automatically unloaded when the application instance is destroyed.

Example:

\snippet src/corelib/bapplicationbase.cpp 0

\sa loadPlugins(), setPluginActivated(), unloadPlugins(), pluginWrapper(), pluginWrappers(), removePlugin(),
setDisabledPlugins(), BPluginsSettingsTab

For more information about plugins see BPluginWrapper.

\section1 Settings system

BApplicationBase instance automatically creates application settings object (QSettings), accessible with
settingsInstance() call or bSettings() macro.

The settings are loaded from a .conf file in the corresponding user directory. If this file does not exist, it is
created. The settings object instance is deleted automatically when the BApplicationBase instance is destroyed.

\note When accessing global settings instance from different threads, a locking mechanism (for example, QMutex) should
be used.

Plugins settings files are stored in the same directory as the application settings file.

\section1 Translations system

BeQt extends Qt's translations system by wrapping several QTranslator instances, grouped by common prefix (identifier),
and loaded from several locations (builtin, shared and user resources).

When BTranslator classes are installed on BApplicationBase instance, several QTranslators are implicitly installed on
the QCoreApplication instance. QCoreApplication QEvent::LanguageChange event is blocked to prevent multiple consequent
events for each QTranslator. Instead, BCoreApplication::languageChanged() or BApplication::languageChanged() signal is
emitted. You may connect to this signal to retranslate user interface when application locale is changed.

The application locale is set by calling setLocale(). Calling this method makes all installed BTranslator objects
uninstall all QTranslator objects they own and install new ones, with the new locale suffix.

\snippet src/corelib/bapplicationbase.cpp 1

\sa installBeqtTranslator(), removeBeqtTranslator(), beqtTranslator(), beqtTranslators(), BTranslator

\section1 Logging

BApplicationBase holds an instance of BLogger, a class used for logging. Some BeQt classes use an instance of BLogger
owned by BApplicationBase by default.

To set custom BLogger instance, use setLogger(). To access global logger instance, use either logger() method or
bLogger() macro.

Convenience method log() and macro bLog() are available. They call global logger's method BLogger::log().

\sa BLogger, BRemoteLogger

\section1 Application information

BApplicationBase contains information about application, such as application name, organization name and domain,
application version, authors, translators, etc.

Application and organization names should be set when creating BCoreApplication or BApplication instance. They can not
be changed later. These names are used to get resources paths.

Available information methods are:

\table
\header
    \li Setters
    \li Getters
\row
    \li setApplicationAuthors()
    \li applicationAuthors()
\row
    \li setApplicationAuthorsFile()
    \li applicationAuthorsFile()
\row
    \li -
    \li applicationAuthorsProvider()
\row
    \li setApplicationChangeLog()
    \li applicationChangeLog()
\row
    \li setApplicationChangeLogFile()
    \li applicationChangeLogFile()
\row
    \li setApplicationCopyrightPeriod()
    \li applicationCopyrightPeriod()
\row
    \li setApplicationDescription()
    \li applicationDescription()
\row
    \li setApplicationDescriptionFile()
    \li applicationDescriptionFile()
\row
    \li setApplicationExtendedCopyrightInfo()
    \li applicationExtendedCopyrightInfo()
\row
    \li setApplicationLicense()
    \li applicationLicense()
\row
    \li setApplicationLicenseFile()
    \li applicationLicenseFile()
\row
    \li setApplicationThanksTo()
    \li applicationThanksTo()
\row
    \li setApplicationThanksToFile()
    \li applicationThanksToFile()
\row
    \li -
    \li applicationThanksToProvider()
\row
    \li setApplicationTranslations()
    \li applicationTranslations()
\row
    \li setApplicationTranslationsFile()
    \li applicationTranslationsFile()
\row
    \li -
    \li applicationTranslationsProvider()
\endtable

There are also method applicationInfo() and beqtInfo(), used to get formatted application information of the specified
type. A locale may also be specified. In that case list of authors, translators, etc. forthat locale will be output.

Example:

\snippet src/corelib/bapplicationbase.cpp 2

\sa BPersonInfoProvider, BPersonInfo, BPersonInfoList
*/

/*!
\enum BApplicationBase::About

This enum type is used to describe types of information about application.

\value Authors
Information about application authors.

\value ChangeLog
Application ChangeLog.

\value Copyringt
Brief application copyright information.

\value Description
Application description.

\value ExtendedCopyright
Detailed application copyright information.

\value License
Application license.

\value ThanksTo
Information about those who contributd to application code or about external libraries/resources used in the
application.

\value Translators
Information about those who translated the application into different languages.
*/

/*!
\enum BApplicationBase::Location

This enum type is used to describe standard location paths.

\value BeqtIconsPath
Path to standard icons shipped with BeQt: "share/appname/beqt/icons".

\value BeqtPath
Basic path to resources shipped with BeQt: "share/appname/beqt".

\value DataPath
Basic path to application resources "share/appname".

\value DocumentationPath
Path to application documentation: "share/appname/doc".

\value IconsPath
Path to application icons: "share/appname/icons".

\value PluginsPath
Path to application plugins: "lib/appname/plguins".

\value SettingsPath
Path to application settings: "share/appname/settings".

\value TranslationsPath
Path to application translations: "share/appname/translations".
*/

/*!
\enum BApplicationBase::Portability

This enum type is used to describe application portability.

\value AutoDetect
BApplicationBase automaticaly determines if application is portable if settings file
"share/appname/settings/appname.conf" exists.

\value NotPortable
The application is not portable.

\value Portable
The application is portable.
*/

/*!
\enum BApplicationBase::ResourceType

This enum type is used to describe application resource type.

\value BuiltinResource
Builtin resource (Qt's resource system). Path starts with ":/".

\value SharedResource
Shared resource. Something like "/usr/share/appname" or "C:/Program Files/appname".

\value UserResource
User resource. Something like "/home/user/.appname" or "C:/Users/user/AppData/Roaming/orgname/appname". In
BApplicationBase::Portable mode the path is the same as for BApplicationBase::SharedResource.
*/

/*!
\class BApplicationBase::CopyrightInfo
\brief The CopyrightInfo struct represents information about copyright.
*/

/*!
\variable BApplicationBase::CopyrightInfo::email
E-mail of the copyright owner.
*/

/*!
\variable BApplicationBase::CopyrightInfo::owner
Name of the copyright owner.
*/

/*!
\variable BApplicationBase::CopyrightInfo::period
Copyright period.

For example: "2012-2014".
*/

/*!
\class BApplicationBase::InitialSettings
\brief The InitialSettings struct represents initial application parameters.
*/

/*!
\variable BApplicationBase::InitialSettings::applicationName
Name of the application.
*/

/*!
\variable BApplicationBase::InitialSettings::organizationName
Name of the application developer/owner.
*/

/*!
\variable BApplicationBase::InitialSettings::portability
Portability mode of the application.
*/

/*!
\class BApplicationBase::LocaleSupportInfo
\brief The LocaleSupportInfo struct represents information about certain locale support.
*/

/*!
\variable BApplicationBase::LocaleSupportInfo::locale
The locale which support is described.
*/

/*!
\variable BApplicationBase::LocaleSupportInfo::supports
Number of translators which support the locale.
*/

/*!
\variable BApplicationBase::LocaleSupportInfo::total
Total number of translators registered in the BApplicationBase instance.
*/

/*============================== Static protected variables ================*/

/*!
\variable BApplicationBase::_m_self
This static variable is used to access BApplicationBase instance. You should not use it directly.
*/

BApplicationBase *BApplicationBase::_m_self = 0;

/*============================== Protected constructors ====================*/

/*!
Constructs a BApplicationBase object and associates the given data object \a d with it. Application and organization
names are set to \a applicationName and \a organizationName, respectively.

\note BApplicationBase itself can not be instantiated because of the pure virtual destructor. Use BCoreApplication or
BApplication instead.
*/

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

/*!
Constructs a BApplicationBase object and associates the given data object \a d with it. Application and organization
names are set according to the corresponding InitialSettings \a s members. You may also specify application portability
mode.

\note BApplicationBase itself can not be instantiated because of the pure virtual destructor. Use BCoreApplication or
BApplication instead.
*/

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

/*============================== Public constructors =======================*/

/*!
Destroys the object. Plugins, translators, settings and logger instances owned by the object are unloaded/deleted.
*/

BApplicationBase::~BApplicationBase()
{
    d_func()->destructorCalled = true;
    foreach (BTranslator *t, d_func()->translators)
        delete t;
    d_func()->translators.clear();
    foreach (BPluginWrapper *pw, d_func()->plugins)
        delete pw;
    d_func()->plugins.clear();
    _m_self = 0;
}

/*============================== Static public methods =====================*/

/*!
Returns the list of application authors.

\sa applicationAuthorsFile(), setApplicationAuthors(), setApplicationAuthorsFile(), BPersonInfoList
*/

BPersonInfoList BApplicationBase::applicationAuthors()
{
    if (!BApplicationBasePrivate::testInit())
        return BPersonInfoList();
    return ds_func()->appAuthorsList;
}

/*!
Returns the path to a file which provides application authors information. If no file path is set, returns an empty
QString.

\sa applicationAuthors(), applicationAuthorsProvider(), setApplicationAuthors(), setApplicationAuthorsFile()
*/

QString BApplicationBase::applicationAuthorsFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appAuthors ? ds_func()->appAuthors->fileName() : QString();;
}

/*!
Returns the BLocationProvider used by BApplicationBase internally to retrieve authors information from the
corresponding file.

\sa applicationAuthors(), applicationAuthorsFile(), setApplicationAuthors(), setApplicationAuthorsFile(),
BPersonInfoProvider
*/

BPersonInfoProvider *BApplicationBase::applicationAuthorsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->appAuthors;
}

/*!
Returns the application ChangeLog text. If no ChangeLog is specified, an empty QString is returned.

\sa applicationChangeLogFile(), setApplicationChangeLog()
*/

QString BApplicationBase::applicationChangeLog()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appChangeLog;
}

/*!
Returns the path to a file which provides application ChangeLog. If no file path is set, returns an empty QString.

\sa applicationChangeLog(), setApplicationChangeLog(), setApplicationChangeLogFile(), BDirTools::localeBasedFileName()
*/

QString BApplicationBase::applicationChangeLogFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appChangeLogFileName;
}

/*!
Returns the application copyright period.

For example: "2012-2014"

\sa setApplicationCopyrightPeriod()
*/

QString BApplicationBase::applicationCopyrightPeriod()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appCopyrightYears;
}

/*!
Returns the application description text. If no description is specified, an empty QString is returned.

\sa applicationDescriptionFile(), setApplicationDescription(), setApplicationDescriptionFile()
*/

QString BApplicationBase::applicationDescription()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appDescription;
}

/*!
Returns the path to a file which provides application description. If no file path is set, returns an empty QString.

\sa applicationDescription(), setApplicationDescription(), setApplicationDescriptionFile(),
BDirTools::localeBasedFileName()
*/

QString BApplicationBase::applicationDescriptionFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appDescriptionFileName;
}

/*!
Returns the list of CopyrightInfo.

\sa setApplicationExtendedCopyrightInfo(), CopyrightInfo
*/

QList<BApplicationBase::CopyrightInfo> BApplicationBase::applicationExtendedCopyrightInfo()
{
    if (!BApplicationBasePrivate::testInit())
        return QList<CopyrightInfo>();
    return ds_func()->copyrightInfos;
}

/*!
Returns formatted application information of the specified \a type. For Authors, ThanksTo and Translators \a loc
determines for which locale the list must be returned.

\sa BPersonInfoProvider
*/

QString BApplicationBase::applicationInfo(About type, const QLocale &loc)
{
    if (!BApplicationBasePrivate::testInit())
        return "";
    QString fn;
    switch (type) {
    case Copyringt: {
        QString crp = (!ds_func()->appCopyrightYears.isEmpty() ? (ds_func()->appCopyrightYears + " ") : QString());
        QString domain = QCoreApplication::organizationDomain();
        if (!domain.isEmpty())
            domain.prepend(" [").append("]");
        return tr("Copyright") + " (C) " + crp + ds_func()->orgName + domain;
    }
    case ExtendedCopyright: {
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
    case Description: {
        if (!ds_func()->appDescription.isEmpty())
            return ds_func()->appDescription;
        else
            fn = ds_func()->appDescriptionFileName;
        break;
    }
    case ChangeLog: {
        if (!ds_func()->appChangeLog.isEmpty())
            return ds_func()->appChangeLog;
        else
            fn = ds_func()->appChangeLogFileName;
        break;
    }
    case License: {
        if (!ds_func()->appLicense.isEmpty())
            return ds_func()->appLicense;
        else
            fn = ds_func()->appLicenseFileName;
        break;
    }
    case Authors: {
        if (!ds_func()->appAuthorsList.isEmpty())
            return BPersonInfoProvider::infoListToString(ds_func()->appAuthorsList);
        else if (ds_func()->appAuthors)
            return ds_func()->appAuthors->infosString(loc);
        else
            return "";
    }
    case Translators: {
        if (!ds_func()->appTranslationsList.isEmpty())
            return BPersonInfoProvider::infoListToString(ds_func()->appTranslationsList);
        else if (ds_func()->appTranslations)
            return ds_func()->appTranslations->infosString(loc, true);
        else
            return "";
    }
    case ThanksTo: {
        if (!ds_func()->appThanksToList.isEmpty())
            return BPersonInfoProvider::infoListToString(ds_func()->appThanksToList);
        else if (ds_func()->appThanksTo)
            return ds_func()->appThanksTo->infosString(loc);
        else
            return "";
    }
    default: {
        return "";
    }
    }
    return BDirTools::readTextFile(BDirTools::localeBasedFileName(fn), "UTF-8");
}

/*!
Returns the application license text. If no description is specified, an empty QString is returned.

\sa applicationLicenseFile(), setApplicationLicense(), setApplicationLicenseFile()
*/

QString BApplicationBase::applicationLicense()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appLicense;
}

/*!
Returns the path to a file which provides application license. If no file path is set, returns an empty QString.

\sa applicationLicense(), setApplicationLicense(), setApplicationLicenseFile(), BDirTools::localeBasedFileName()
*/

QString BApplicationBase::applicationLicenseFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appLicenseFileName;
}

/*!
Returns the list of application "thanks to".

\sa applicationThanksToFile(), setApplicationThanksTo(), setApplicationThanksToFile(), BPersonInfoList
*/

BPersonInfoList BApplicationBase::applicationThanksTo()
{
    if (!BApplicationBasePrivate::testInit())
        return BPersonInfoList();
    return ds_func()->appThanksToList;
}

/*!
Returns the path to a file which provides application "thanks to" information. If no file path is set, returns an empty
QString.

\sa applicationThanksTo(), applicationThanksToProvider(), setApplicationThanksTo(), setApplicationThanksToFile()
*/

QString BApplicationBase::applicationThanksToFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appThanksTo ? ds_func()->appThanksTo->fileName() : QString();
}

/*!
Returns the BLocationProvider used by BApplicationBase internally to retrieve "thanks to" information from the
corresponding file.

\sa applicationThanksTo(), applicationThanksToFile(), setApplicationThanksTo(), setApplicationThanksToFile(),
BPersonInfoProvider
*/

BPersonInfoProvider *BApplicationBase::applicationThanksToProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->appThanksTo;
}

/*!
Returns the list of application translators (people, not QTranslator or BTranslator classes).

\sa applicationTranslationsFile(), setApplicationTranslations(), setApplicationTranslationsFile(), BPersonInfoList
*/

BPersonInfoList BApplicationBase::applicationTranslations()
{
    if (!BApplicationBasePrivate::testInit())
        return BPersonInfoList();
    return ds_func()->appTranslationsList;
}

/*!
Returns the path to a file which provides application translators (people, not QTranslator or BTranslator classes)
information. If no file path is set, returns an empty QString.

\sa applicationTranslations(), applicationTranslationsProvider(), setApplicationTranslations(),
setApplicationTranslationsFile()
*/

QString BApplicationBase::applicationTranslationsFile()
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    return ds_func()->appTranslations ? ds_func()->appTranslations->fileName() : QString();
}

/*!
Returns the BLocationProvider used by BApplicationBase internally to retrieve translators (people, not QTranslator or
BTranslator classes) information from the corresponding file.

\sa applicationTranslations(), applicationTranslationsFile(), setApplicationTranslations(),
setApplicationTranslationsFile(), BPersonInfoProvider
*/

BPersonInfoProvider *BApplicationBase::applicationTranslationsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->appTranslations;
}

/*!
This function performs search of translation files (.qm) for all installed BTranslator objects in all available
locations (builtin, shared and user resources). Returns list of LocaleSupportInfo.

If \a alwaysIncludeEnglish parameter is true, the returned list will always contain LocaleSupportInfo with locale
"en_US". That means, translators for locale "en_US" will be always considered existing, even if they do not actually
exist.

If \a englishAlwaysFull parameter is true, the supports and total members of the returned LocaleSupportInfo
with locale "en_US" will be always equal. That means, support for locale "en_US" will be always considered full, even
if there is no translator for thet locale.

\sa LocaleSupportInfo
*/

QList<BApplicationBase::LocaleSupportInfo> BApplicationBase::availableLocales(bool alwaysIncludeEnglish,
                                                                              bool englishAlwaysFull)
{
    if (!BApplicationBasePrivate::testInit())
        return QList<BApplicationBase::LocaleSupportInfo>();
    B_DS(BApplicationBase);
    QList< QList<QLocale> > lll;
    foreach (BTranslator *t, ds->translators)
        lll << t->availableLocales();
    QList<QLocale> llist;
    QList<BApplicationBase::LocaleSupportInfo> list;
    if (alwaysIncludeEnglish) {
        LocaleSupportInfo en = BApplicationBasePrivate::createLocaleSupportInfo();
        en.total = ds->translators.size();
        if (englishAlwaysFull)
            en.supports = en.total;
        list << en;
    }
    foreach (const QList<QLocale> &ll, lll) {
        foreach (const QLocale &l, ll) {
            if (llist.contains(l))
                continue;
            llist << l;
            LocaleSupportInfo lsi = BApplicationBasePrivate::createLocaleSupportInfo();
            lsi.locale = l;
            lsi.total = ds->translators.size();
            list << lsi;
        }
    }
    for (int i = 0; i < list.size(); ++i) {
        LocaleSupportInfo &lsi = list[i];
        foreach (const QList<QLocale> &ll, lll) {
            if (!ll.contains(lsi.locale))
                continue;
            ++lsi.supports;
        }
    }
    return list;
}

/*!
Returns the BLocationProvider used by BApplicationBase internally to retrieve BeQt authors information.

\sa BPersonInfoProvider
*/

BPersonInfoProvider *BApplicationBase::beqtAuthorsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->beqtAuthors;
}

/*!
Returns formatted BeQt library information of the specified \a type. For Authors, ThanksTo and Translators \a loc
determines for which locale the list must be returned.

\sa BPersonInfoProvider
*/

QString BApplicationBase::beqtInfo(About type, const QLocale &loc)
{
    if (!BApplicationBasePrivate::testInit())
        return "";
    QString bfn;
    switch (type) {
    case Copyringt:
        return tr("Copyright") + " (C) 2012-2014 Andrey Bogdanov [https://github.com/ololoepepe/BeQt]";
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
    if (fn.isEmpty()) {
        dir = location(BeqtPath, BuiltinResource) + "/";
        fn = BDirTools::localeBasedFileName(dir + bfn, loc);
    }
    return BDirTools::readTextFile(fn, "UTF-8");
}

/*!
Returns the BLocationProvider used by BApplicationBase internally to retrieve BeQt "thanks to" information.

\sa BPersonInfoProvider
*/

BPersonInfoProvider *BApplicationBase::beqtThanksToProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->beqtThanksTo;
}

/*!
Returns the BLocationProvider used by BApplicationBase internally to retrieve BeQt translators (people, not QTranslator
or BTranslator classes) information.

\sa BPersonInfoProvider
*/

BPersonInfoProvider *BApplicationBase::beqtTranslationsProvider()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->beqtTranslations;
}

/*!
Returns BTranslator instance for \a fileName identifier, previously registered by calling installBeQtTranslator().

If \a fileName is empty or there is no translator registered with this identifier, 0 is returned.
*/

BTranslator *BApplicationBase::beqtTranslator(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->translators.value(fileName);
}

/*!
Returns a list of BTranslator instances, previously registered by calling installBeQtTranslator().

If there are no translators installed, an empty list is returned.
*/

QList<BTranslator *> BApplicationBase::beqtTranslators()
{
    if ( !BApplicationBasePrivate::testInit() )
        return QList<BTranslator *>();
    return ds_func()->translators.values();
}

/*!
Returns a pointer to the application's BApplicationBase instance.

If no instance has been allocated, null is returned.
*/

BApplicationBase *BApplicationBase::binstance()
{
    return _m_self;
}

/*!
Returns a list of plugin identifiers, which are considered deactivated and will not be automatically activated on next
loadPlugins() call.
*/

QStringList BApplicationBase::disabledPlugins()
{
    if (!BApplicationBasePrivate::testInit())
        return QStringList();
    if (ds_func()->settings.isNull())
        return QStringList();
    return ds_func()->settings->value("BeQt/Core/deactivated_plugins").toStringList();
}

/*!
Registers \a translator. Underlying QTranslators are installed on QCoreApplication instance.

BApplicationBase takes ownership of the translator.

\sa beqtTranslator(), beqtTranslators(), removeBeqtTranslator(), BTranslator
*/

void BApplicationBase::installBeqtTranslator(BTranslator *translator)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->installTranslator(translator);
}

/*!
\overload installBeqtTranslator()

Creates new BTranslator with file name \a id and registers it. Underlying QTranslators are installed on
QCoreApplication instance.

BApplicationBase takes ownership of the translator.

\sa beqtTranslator(), beqtTranslators(), removeBeqtTranslator(), BTranslator
*/

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

/*!
Registers location provider \a p in BApplicationBase location system.

Location providers are used by location() and locations() methods when retrieving paths for given identifiers.

\sa location(), locations(), removeLocationProvider(), removeLocationProviders(), BAbstractLocationProvider,
BLocationProvider
*/

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

/*!
Registers every location provider in \a list in BApplicationBase location system.

Location providers are used by location() and locations() methods when retrieving paths for given identifiers.

\sa installLocationProvider(), location(), locations(), removeLocationProvider(), removeLocationProviders(),
BAbstractLocationProvider, BLocationProvider
*/

void BApplicationBase::installLocationProviders(const QList<BAbstractLocationProvider *> &list)
{
    foreach (BAbstractLocationProvider *p, list)
        installLocationProvider(p);
}

/*!
Register \a plugin in BApplicationBase plugin system.

Registered plugins are automatically deactivated and unloaded when BApplicationBase instance is destroyed.

\sa loadPlugins(), removePlugin(), pluginWrapper(), pluginWrappers(), unloadPlugins()
*/

void BApplicationBase::installPlugin(BPluginWrapper *plugin)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->installPlugin(plugin);
}

/*!
Returns true if the application is portable, otherwise returns false.

For portable applications shared and user resources have same paths.

See \l {Resources system} for details.
*/

bool BApplicationBase::isPortable()
{
    if (!BApplicationBasePrivate::testInit())
        return false;
    return ds_func()->portable;
}

/*!
Loads and registeres all plugins which types match any of BPluginWrapper::acceptableTypes() and for which
BPluginWrapper::interfaceTestFunction() function returns true.

Plugins which were manually disabled on last application launch are not activated. Other plugins are.

\sa BPluginWrapper
*/

void BApplicationBase::loadPlugins()
{
    loadPlugins(BPluginWrapper::acceptableTypes(), BPluginWrapper::interfaceTestFunction());
}

/*!
\overload loadPlugins()

Loads and registeres all plugins which types match any of \a acceptableTypes and for which
BPluginWrapper::interfaceTestFunction() function returns true.

Plugins which were manually disabled on last application launch are not activated. Other plugins are.

\sa BPluginWrapper
*/

void BApplicationBase::loadPlugins(const QStringList &acceptableTypes)
{
    loadPlugins(acceptableTypes, BPluginWrapper::interfaceTestFunction());
}

/*!
\overload loadPlugins()

Loads and registeres all plugins which types match any of \a acceptableTypes and for which \a function returns true.

Plugins which were manually disabled on last application launch are not activated. Other plugins are.

\sa BPluginWrapper
*/

void BApplicationBase::loadPlugins(const QStringList &acceptableTypes, BPluginWrapper::InterfaceTestFunction function)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    QStringList prevAcceptableTypes = BPluginWrapper::acceptableTypes();
    BPluginWrapper::InterfaceTestFunction prevFunction = BPluginWrapper::interfaceTestFunction();
    BPluginWrapper::setAcceptableTypes(acceptableTypes);
    BPluginWrapper::setInterfaceTestFunction(function);
    foreach (QString dir, locations(PluginsPath)) {
        foreach (QString file, BDirTools::entryList(dir, BApplicationBasePrivate::PluginSuffixes, QDir::Files)) {
            BPluginWrapper *pw = new BPluginWrapper(file);
            if (!pw->load()) {
                pw->deleteLater();
                continue;
            }
            installPlugin(pw);
            if (ds_func()->isPluginActivated(pw->id()))
                pw->activate();
            else
                pw->unload();
        }
    }
    BPluginWrapper::setAcceptableTypes(prevAcceptableTypes);
    BPluginWrapper::setInterfaceTestFunction(prevFunction);
}

/*!
Returns the locale used in translations system. By default QLocale::system() is used.

For details, see \l {Translations system}.
*/

QLocale BApplicationBase::locale()
{
    if (!BApplicationBasePrivate::testInit())
        return QLocale::system();
    return ds_func()->getLocale();
}

/*!
Returns path for location type \a loc and resource type \a type.

For details, see \l {Resources system}.

\sa locations()
*/

QString BApplicationBase::location(Location loc, ResourceType type)
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    QString name;
    switch (loc) {
    case DataPath:
        name = "data";
        break;
    case DocumentationPath:
        name = "doc";
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

/*!
\overload location()

Returns path for identifier \a subdir and resource type \a type.

For details, see \l {Resources system}.

\sa locations()
*/

QString BApplicationBase::location(const QString &subdir, ResourceType type)
{
    if (!BApplicationBasePrivate::testInit())
        return QString();
    if (subdir.isEmpty())
        return QString();
    return ds_func()->location(subdir, type);
}

/*!
Returns all paths for location type \a loc (builtin, shared and user resources paths are returned).

For details, see \l {Resources system}.

\sa location()
*/

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

/*!
\overload locations()

Returns all paths for identifier \a subdir (builtin, shared and user resources paths are returned).

For details, see \l {Resources system}.

\sa location()
*/

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

/*!
Convenience method, which calls BLogger::log() on BApplicationBase logger() BLogger instance.

The text logged is \a text, and logging level is \a lvl.

\sa bLog(), BLogger
*/

void BApplicationBase::log(const QString &text, BLogger::Level lvl)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    B_DS(BApplicationBase);
    if (!ds->logger)
        return;
    ds->logger->log(text, lvl);
}

/*!
Returns BLogger instance installed on the BApplicationBase.

For details, see \l {Logging}.

\sa BLogger
*/

BLogger *BApplicationBase::logger()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->logger;
}

/*!
Returns an instance BPluginWrapper with id \a pluginId, registered in BApplicationBase.

\sa installPlugin(), pluginWrappers()
*/

BPluginWrapper *BApplicationBase::pluginWrapper(const QString &pluginId)
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    if (pluginId.isEmpty())
        return 0;
    foreach (BPluginWrapper *pw, ds_func()->plugins) {
        if (pw->id() == pluginId)
            return pw;
    }
    return 0;
}

/*!
Returns a list of BPluginWrapper instances of type \a type, registered in BApplicationBase.

\sa installPlugin(), pluginWrapper()
*/

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

/*!
Removes \a translator. Underlying QTranslators are removed from QCoreApplication instance.

The translator itself is not deleted.

\sa beqtTranslator(), beqtTranslators(), BTranslator
*/

void BApplicationBase::removeBeqtTranslator(BTranslator *translator)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->removeTranslator(translator);
}

/*!
\overload removeBeqtTranslator()

Removes BTranslator with id \a id. Underlying QTranslators are removed from QCoreApplication instance.

The translator itself is not deleted.

\sa beqtTranslator(), beqtTranslators(), BTranslator
*/

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

/*!
Removes location provider \a p from BApplicationBase location system.

Location provider itself is not deleted.

\sa removeLocationProviders(), installLocationProvider(), installLocationProviders()
*/

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

/*!
Removes all location providers in \a list from BApplicationBase location system.

Location providers themselves are not deleted.

\sa removeLocationProvider(), installLocationProvider(), installLocationProviders()
*/

void BApplicationBase::removeLocationProviders(const QList<BAbstractLocationProvider *> &list)
{
    foreach (BAbstractLocationProvider *p, list)
        removeLocationProvider(p);
}

/*!
Removes \a plugin from BApplicationBase plugin system.

Plugin itself is not deleted.

\sa installPlugin(), pluginWrapper(), pluginWrappers()
*/

void BApplicationBase::removePlugin(BPluginWrapper *plugin)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    B_DS(BApplicationBase);
    QObject::disconnect(plugin, SIGNAL(activated()), ds, SLOT(pluginActivated()));
    QObject::disconnect(plugin, SIGNAL(aboutToBeDeactivated()), ds, SLOT(pluginAboutToBeDeactivated()));
    QObject::disconnect(plugin, SIGNAL(destroyed(QObject *)), ds, SLOT(pluginDestroyed(QObject *)));
}

/*!
Sets application authors to \a list.

\sa applicationAuthors(), setApplicationAuthorsFile(), BPersonInfoList
*/

void BApplicationBase::setApplicationAuthors(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appAuthorsList = list;
    ds_func()->appAuthors->setFileName("");
}

/*!
Sets the path to a file which provides application authors information to \a fileName.

\sa applicationAuthorsFile(), setApplicationAuthors()
*/

void BApplicationBase::setApplicationAuthorsFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appAuthors->setFileName(fileName);
    ds_func()->appAuthorsList.clear();
}

/*!
Sets the application ChangeLog text to \a s.

\sa applicationChangeLog(), setApplicationChangeLogFile()
*/

void BApplicationBase::setApplicationChangeLog(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appChangeLog = s;
    ds_func()->appChangeLogFileName.clear();
}

/*!
Sets the path to a file which provides application ChangeLog to \a fileName.

Actual file name is retrieved using BDirTools::localeBasedFileName() based on current application locale.

\sa applicationChangeLog(), setApplicationChangeLog()
*/

void BApplicationBase::setApplicationChangeLogFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appChangeLogFileName = fileName;
    ds_func()->appChangeLog.clear();
}

/*!
Sets the application copyright period to \a s.

For example: "2012-2014"

\sa applicationCopyrightPeriod()
*/

void BApplicationBase::setApplicationCopyrightPeriod(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appCopyrightYears = s;
}

/*!
Sets the application description text to \a s.

\sa applicationDescription(), setApplicationDescriptionFile()
*/

void BApplicationBase::setApplicationDescription(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appDescription = s;
    ds_func()->appDescriptionFileName.clear();
}

/*!
Sets the path to a file which provides application description \a fileName.

Actual file name is retrieved using BDirTools::localeBasedFileName() based on current application locale.

\sa applicationDescription(), setApplicationDescription()
*/

void BApplicationBase::setApplicationDescriptionFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appDescriptionFileName = fileName;
    ds_func()->appDescription.clear();
}

/*!
Sets the list of application CopyrightInfo to \a list.

\sa applicationExtendedCopyrightInfo(), CopyrightInfo
*/

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

/*!
Sets the application license text to \a s.

\sa applicationLicense(), setApplicationLicenseFile()
*/

void BApplicationBase::setApplicationLicense(const QString &s)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appLicense = s;
    ds_func()->appLicenseFileName.clear();
}

/*!
Sets the path to a file which provides application license to \a fileName.

Actual file name is retrieved using BDirTools::localeBasedFileName() based on current application locale.

\sa applicationLicense(), setApplicationLicense()
*/

void BApplicationBase::setApplicationLicenseFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appLicenseFileName = fileName;
    ds_func()->appLicense.clear();
}

/*!
Sets the list of application "thanks to" to \a list.

\sa applicationThanksTo(), setApplicationThanksToFile(), BPersonInfoList
*/

void BApplicationBase::setApplicationThanksTo(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appThanksToList = list;
    ds_func()->appThanksTo->setFileName("");
}

/*!
Sets the path to a file which provides application "thanks to" information to \a fileName.

\sa applicationThanksToFile(), setApplicationThanksTo()
*/

void BApplicationBase::setApplicationThanksToFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appThanksTo->setFileName(fileName);
    ds_func()->appThanksToList.clear();
}

/*!
Sets the list of application translators (people, not QTranslator or BTranslator classes) to \a list.

\sa applicationTranslations(), setApplicationTranslationsFile(), BPersonInfoList
*/

void BApplicationBase::setApplicationTranslations(const BPersonInfoList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appTranslationsList = list;
    ds_func()->appTranslations->setFileName("");
}

/*!
Sets the path to a file which provides application translators (people, not QTranslator or BTranslator classes)
information to \a fileName.

\sa applicationTranslationsFile(), setApplicationTranslations()
*/

void BApplicationBase::setApplicationTranslationsFile(const QString &fileName)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->appTranslations->setFileName(fileName);
    ds_func()->appTranslationsList.clear();
}

/*!
Appends \a list of plugin identifiers to the internal list of deactivated plugins.

Plugins with that identifiers will not be activated automatically on next loadPlugins() call.

For details, see \l {Plugins system}.

\sa loadPlugins(), installPlugin(), removePlugin(), setPluginActivated(), unloadPlugins(), BPluginWrapper
*/

void BApplicationBase::setDisabledPlugins(const QStringList &list)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (ds_func()->settings.isNull())
        return;
    QStringList nlist = bWithoutDuplicates(list);
    nlist.removeAll("");
    ds_func()->settings->setValue("BeQt/Core/deactivated_plugins", nlist);
}

/*!
Sets the application locale used in translations system to \a l. By default QLocale::system() is used.

Calling this function causes all installed BTranslator instances to reload. BCoreApplication::languageChanged() or
BApplication::languageChanged() signal is emitted.

For details, see \l {Translations system}.
*/

void BApplicationBase::setLocale(const QLocale &l)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    if (l == locale())
        return;
    ds_func()->setLocale(l);
}

/*!
Sets \a l to be the BLogger instance installed on the BApplicationBase.

For details, see \l {Logging}.

\sa BLogger
*/

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

/*!
Activates plugin with id \a pluginId if \a activated is true. Otherwise deactivates that plugin.

If plugin is already activated, it will not be activated again. The same applies to deactivation.

When \a activated if false, the plugin id is placed into the list of deactivated plugins. Plugins which were
deactivated on previous application launch will not be activated automatically on next application launch.

For details, see \l {Plugins system}.

\sa loadPlugins(), installPlugin(), removePlugin(), unloadPlugins(), BPluginWrapper
*/

void BApplicationBase::setPluginActivated(const QString &pluginId, bool activated)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    ds_func()->setPluginActivated(pluginId, activated);
}

/*!
Returns global application settings instance.

For details, see \l {Settings system}.
*/

QSettings *BApplicationBase::settingsInstance()
{
    if (!BApplicationBasePrivate::testInit())
        return 0;
    return ds_func()->settings.data();
}

/*!
Unloads all plugins registered in BApplicationBase plugin system.

If \a remove is true, plugins are also removed from BApplicationBase plugin system.

For details, see \l {Plugins system}.

\sa BPluginWrapper
*/

void BApplicationBase::unloadPlugins(bool remove)
{
    if (!BApplicationBasePrivate::testInit())
        return;
    foreach (BPluginWrapper *pw, ds_func()->plugins) {
        if (pw->isLoaded())
            pw->unload();
        if (remove)
            removePlugin(pw);
    }
}
