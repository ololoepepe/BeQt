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

#ifndef BCOREAPPLICATION_H
#define BCOREAPPLICATION_H

class BCoreApplicationPrivate;
class BTranslator;
class BTranslatorPrivate;
class BPluginWrapperPrivate;
class BDirToolsPrivate;
class BPluginWrapper;

class QLocale;
class QSettings;
class QTextCodec;

#include "bglobal.h"
#include "bbase.h"
#include "blogger.h"
#include "bpersoninfo.h"

#include <QObject>
#include <QLocale>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#define bApp BCoreApplication::instance()
#define bSettings BCoreApplication::settingsInstance()
#define bLogger BCoreApplication::logger()
#define bLog BCoreApplication::log

/*============================================================================
================================ BCoreApplication ============================
============================================================================*/

class B_CORE_EXPORT BCoreApplication : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BCoreApplication)
    B_DECLARE_PRIVATE_S(BCoreApplication)
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
public:
    typedef bool (*InterfaceTestFunction)(const QObject *);
public:
    explicit BCoreApplication();
    ~BCoreApplication();
protected:
    explicit BCoreApplication(BCoreApplicationPrivate &d);
public:
    static BCoreApplication *instance();
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
    static void installTranslator(BTranslator *translator);
    static void removeTranslator(BTranslator *translator);
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
Q_SIGNALS:
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void languageChanged();
    void settingsLoaded(QSettings *s);
    void settingsSaved(QSettings *s);
protected:
    static BCoreApplication *_m_self;
private:
    Q_DISABLE_COPY(BCoreApplication)
    friend class BTranslatorPrivate;
    friend class BPluginWrapperPrivate;
    friend class BDirToolsPrivate;
};

#endif // BCOREAPPLICATION_H
