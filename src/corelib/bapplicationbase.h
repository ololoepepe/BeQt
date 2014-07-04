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
class BTranslator;
class BTranslatorPrivate;
class BPluginWrapperPrivate;
class BDirToolsPrivate;
class BPluginWrapper;
class BAbstractLocationProvider;

class QLocale;
class QSettings;

#include "bglobal.h"
#include "bbaseobject.h"
#include "blogger.h"
#include "bpersoninfo.h"

#include <QLocale>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QCoreApplication>

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
    enum Location
    {
        DataPath,
        DocumentationPath,
        PluginsPath,
        SettingsPath,
        TranslationsPath,
        BeqtPath
    };
    enum ResourceType
    {
        UserResource,
        SharedResource,
        BuiltinResource
    };
    enum About
    {
        Copyringt,
        Description,
        ChangeLog,
        License,
        Authors,
        Translators,
        ThanksTo
    };
    enum Portability
    {
        AutoDetect,
        Portable,
        NotPortable
    };
public:
    struct LocaleSupportInfo
    {
        QLocale locale;
        int supports;
        int total;
    };
    struct InitialSettings
    {
        QString applicationName;
        QString organizationName;
        Portability portability;
    };
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
public:
    ~BApplicationBase() = 0;
protected:
    explicit BApplicationBase(BApplicationBasePrivate &d, const QString &applicationName = QString(),
                              const QString &organizationName = QString());
    explicit BApplicationBase(BApplicationBasePrivate &d, const InitialSettings &s);
public:
    static BApplicationBase *binstance();
    static void installLocationProvider(BAbstractLocationProvider *p);
    static void installLocationProviders(const QList<BAbstractLocationProvider *> &list);
    static void removeLocationProvider(BAbstractLocationProvider *p);
    static void removeLocationProviders(const QList<BAbstractLocationProvider *> &list);
    static QString location(Location loc, ResourceType type);
    static QString location(const QString &subdir, ResourceType type);
    static QStringList locations(Location loc);
    static QStringList locations(const QString &subdir);
    static QSettings *settingsInstance();
    static bool isPortable();
    static void installPlugin(BPluginWrapper *plugin);
    static void removePlugin(BPluginWrapper *plugin);
    static void loadPlugins();
    static void loadPlugins(const QStringList &acceptableTypes);
    static void loadPlugins(const QStringList &acceptableTypes, InterfaceTestFunction function);
    static void unloadPlugins(bool remove = false);
    static QList<BTranslator *> beqtTranslators();
    static BTranslator *beqtTranslator(const QString &fileName);
    static QList<BPluginWrapper *> pluginWrappers( const QString &type = QString() );
    static void installBeqtTranslator(BTranslator *translator);
    static void installBeqtTranslator(const QString &id);
    static void removeBeqtTranslator(BTranslator *translator);
    static void removeBeqtTranslator(const QString &id);
    static void setLocale(const QLocale &l);
    static QLocale locale();
    static QList<LocaleSupportInfo> availableLocales(bool alwaysIncludeEnglish = false);
    static void setApplicationCopyrightPeriod(const QString &s);
    static void setApplicationDescription(const QString &s);
    static void setApplicationDescriptionFile(const QString &fileName);
    static void setApplicationChangeLog(const QString &s);
    static void setApplicationChangeLogFile(const QString &fileName);
    static void setApplicationLicense(const QString &s);
    static void setApplicationLicenseFile(const QString &fileName);
    static void setApplicationAuthors(const BPersonInfoList &list);
    static void setApplicationAuthorsFile(const QString &fileName);
    static void setApplicationTranslations(const BPersonInfoList &list);
    static void setApplicationTranslationsFile(const QString &fileName);
    static void setApplicationThanksTo(const BPersonInfoList &list);
    static void setApplicationThanksToFile(const QString &fileName);
    static QString applicationInfo(About type, const QLocale &loc = locale());
    static QString beqtInfo(About type, const QLocale &loc = locale());
    static void setLogger(BLogger *l);
    static BLogger *logger();
    static void log(const QString &text, BLogger::Level lvl = BLogger::NoLevel);
protected:
    static BApplicationBase *_m_self;
private:
    Q_DISABLE_COPY(BApplicationBase)
};

#endif // BAPPLICATIONBASE_H
