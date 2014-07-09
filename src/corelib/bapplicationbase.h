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

#ifndef BAPPLICATIONBASE_H
#define BAPPLICATIONBASE_H

class BApplicationBasePrivate;

class BAbstractLocationProvider;
class BPersonInfoProvider;
class BTranslator;

class QSettings;
class QStringList;

#include "bbaseobject.h"
#include "blogger.h"
#include "bpersoninfolist.h"
#include "bpluginwrapper.h"

#include <QCoreApplication>
#include <QList>
#include <QLocale>
#include <QString>

#if !defined(bSettings)
#   define bSettings BApplicationBase::settingsInstance()
#endif
#if !defined(bLogger)
#   define bLogger BApplicationBase::logger()
#endif
#if !defined(bLog)
#   define bLog BApplicationBase::log
#endif

/*============================================================================
================================ BApplicationBase ============================
============================================================================*/

class B_CORE_EXPORT BApplicationBase : public BBaseObject
{
    B_DECLARE_PRIVATE(BApplicationBase)
    B_DECLARE_PRIVATE_S(BApplicationBase)
    Q_DECLARE_TR_FUNCTIONS(BApplicationBase)
public:
    enum About
    {
        Authors = 1,
        ChangeLog,
        Copyringt,
        Description,
        ExtendedCopyright,
        License,
        ThanksTo,
        Translators
    };
    enum Location
    {
        BeqtIconsPath = 1,
        BeqtPath,
        DataPath,
        DocumentationPath,
        IconsPath,
        PluginsPath,
        SettingsPath,
        TranslationsPath
    };
    enum Portability
    {
        AutoDetect = 0,
        NotPortable,
        Portable
    };
    enum ResourceType
    {
        BuiltinResource = 1,
        SharedResource,
        UserResource
    };
public:
    struct CopyrightInfo
    {
        QString email;
        QString owner;
        QString period;
    };
    struct InitialSettings
    {
        QString applicationName;
        QString organizationName;
        Portability portability;
    };
    struct LocaleSupportInfo
    {
        QLocale locale;
        int supports;
        int total;
    };
protected:
    static BApplicationBase *_m_self;
protected:
    explicit BApplicationBase(BApplicationBasePrivate &d, const QString &applicationName = QString(),
                              const QString &organizationName = QString());
    explicit BApplicationBase(BApplicationBasePrivate &d, const InitialSettings &s);
public:
    ~BApplicationBase() = 0;
public:
    static BPersonInfoList applicationAuthors();
    static QString applicationAuthorsFile();
    static BPersonInfoProvider *applicationAuthorsProvider();
    static QString applicationChangeLog();
    static QString applicationChangeLogFile();
    static QString applicationCopyrightPeriod();
    static QString applicationDescription();
    static QString applicationDescriptionFile();
    static QList<CopyrightInfo> applicationExtendedCopyrightInfo();
    static QString applicationInfo(About type, const QLocale &loc = locale());
    static QString applicationLicense();
    static QString applicationLicenseFile();
    static BPersonInfoList applicationThanksTo();
    static QString applicationThanksToFile();
    static BPersonInfoProvider *applicationThanksToProvider();
    static BPersonInfoList applicationTranslations();
    static QString applicationTranslationsFile();
    static BPersonInfoProvider *applicationTranslationsProvider();
    static QList<LocaleSupportInfo> availableLocales(bool alwaysIncludeEnglish = false);
    static BPersonInfoProvider *beqtAuthorsProvider();
    static QString beqtInfo(About type, const QLocale &loc = locale());
    static BPersonInfoProvider *beqtThanksToProvider();
    static BPersonInfoProvider *beqtTranslationsProvider();
    static BTranslator *beqtTranslator(const QString &fileName);
    static QList<BTranslator *> beqtTranslators();
    static BApplicationBase *binstance();
    static QStringList disabledPlugins();
    static void installBeqtTranslator(BTranslator *translator);
    static void installBeqtTranslator(const QString &id);
    static void installLocationProvider(BAbstractLocationProvider *p);
    static void installLocationProviders(const QList<BAbstractLocationProvider *> &list);
    static void installPlugin(BPluginWrapper *plugin);
    static bool isPortable();
    static void loadPlugins();
    static void loadPlugins(const QStringList &acceptableTypes);
    static void loadPlugins(const QStringList &acceptableTypes, BPluginWrapper::InterfaceTestFunction function);
    static QLocale locale();
    static QString location(Location loc, ResourceType type);
    static QString location(const QString &subdir, ResourceType type);
    static QStringList locations(Location loc);
    static QStringList locations(const QString &subdir);
    static void log(const QString &text, BLogger::Level lvl = BLogger::NoLevel);
    static BLogger *logger();
    static QList<BPluginWrapper *> pluginWrappers(const QString &type = QString());
    static void removeBeqtTranslator(BTranslator *translator);
    static void removeBeqtTranslator(const QString &id);
    static void removeLocationProvider(BAbstractLocationProvider *p);
    static void removeLocationProviders(const QList<BAbstractLocationProvider *> &list);
    static void removePlugin(BPluginWrapper *plugin);
    static void setApplicationAuthors(const BPersonInfoList &list);
    static void setApplicationAuthorsFile(const QString &fileName);
    static void setApplicationChangeLog(const QString &s);
    static void setApplicationChangeLogFile(const QString &fileName);
    static void setApplicationCopyrightPeriod(const QString &s);
    static void setApplicationDescription(const QString &s);
    static void setApplicationDescriptionFile(const QString &fileName);
    static void setApplicationExtendedCopyrightInfo(const QList<CopyrightInfo> &list);
    static void setApplicationLicense(const QString &s);
    static void setApplicationLicenseFile(const QString &fileName);
    static void setApplicationThanksTo(const BPersonInfoList &list);
    static void setApplicationThanksToFile(const QString &fileName);
    static void setApplicationTranslations(const BPersonInfoList &list);
    static void setApplicationTranslationsFile(const QString &fileName);
    static void setDisabledPlugins(const QStringList &list);
    static void setLocale(const QLocale &l);
    static void setLogger(BLogger *l);
    static QSettings *settingsInstance();
    static void unloadPlugins(bool remove = false);
private:
    Q_DISABLE_COPY(BApplicationBase)
};

#endif // BAPPLICATIONBASE_H
