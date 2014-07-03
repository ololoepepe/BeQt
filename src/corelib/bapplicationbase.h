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

class QLocale;
class QSettings;
class QTextCodec;

#include "bglobal.h"
#include "bbaseobject.h"
#include "blogger.h"
#include "bpersoninfo.h"
#include "bversion.h"

#include <QLocale>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QCoreApplication>
#include <QUrl>

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
    enum ResourcesType
    {
        UserResources,
        SharedResources,
#if defined(Q_OS_MAC)
        BundleResources,
#endif
        BuiltinResources
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
        QUrl organizationDomain;
        BVersion applicationVersion;
    };
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
public:
    ~BApplicationBase() = 0;
protected:
    explicit BApplicationBase(BApplicationBasePrivate &d, const QString &applicationName);
    explicit BApplicationBase(BApplicationBasePrivate &d, const InitialSettings &s = InitialSettings());
public:
    static BApplicationBase *binstance();
    static QString location(Location loc, ResourcesType type);
    static QString location(const QString &subdir, ResourcesType type);
    static QStringList locations(Location loc);
    static QStringList locations(const QString &subdir);
    static QSettings *settingsInstance();
    static bool isPortable();
    static void registerPluginWrapper(BPluginWrapper *plugin);
    static void unregisterPluginWrapper(BPluginWrapper *plugin);
    static void loadPlugins(const QStringList &acceptableTypes = QStringList(), InterfaceTestFunction function = 0);
    static QList<BTranslator *> translators();
    static BTranslator *translator(const QString &fileName);
    static QList<BPluginWrapper *> pluginWrappers( const QString &type = QString() );
    static void binstallTranslator(BTranslator *translator);
    static void bremoveTranslator(BTranslator *translator);
    static void setLocale(const QLocale &l);
    static QLocale locale();
    static QList<LocaleSupportInfo> availableLocales(bool alwaysIncludeEnglish = false);
    static void loadSettings();
    static void saveSettings();
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
    friend class BTranslatorPrivate;
    friend class BPluginWrapperPrivate;
    friend class BDirToolsPrivate;
};

#endif // BAPPLICATIONBASE_H
