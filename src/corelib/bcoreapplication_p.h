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

#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPluginWrapper;
class BTranslator;
class BPluginWrapperPrivate;
class BTranslatorPrivate;
class BCoreApplicationPrivate;
class BLogger;
class BSignalDelayProxy;
class BPersonInfoProvider;

class QString;
class QLocale;
class QEvent;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "bpersoninfo.h"

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QVariantMap>
#include <QPointer>
#include <QSettings>

/*============================================================================
================================ BCoreApplicationPrivate =====================
============================================================================*/

class B_CORE_EXPORT BCoreApplicationPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BCoreApplication)
    B_DECLARE_PUBLIC_S(BCoreApplication)
public:
    static const QStringList PluginSuffixes;
public:
    explicit BCoreApplicationPrivate(BCoreApplication *q);
    virtual ~BCoreApplicationPrivate();
public:
    static BCoreApplicationPrivate *instance();
    static QString toLowerNoSpaces(const QString &string);
    static QString subdir(BCoreApplication::Location loc);
    static bool testCoreInit(const char *where = 0);
    static bool testCoreUnique();
    static BCoreApplication::LocaleSupportInfo createLocaleSupportInfo();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    QString getAppName() const;
    QString getOrgName() const;
    QString getAppPath() const;
    QString getUserPrefix() const;
    QString getSharedPrefix(bool forPlugins = false) const;
#if defined(Q_OS_LINUX)
    QString getSharedPrefixPlugins() const;
#endif
#if defined(Q_OS_MAC)
    QString getBundlePrefix() const;
#endif
    bool getIsPortable() const;
    QLocale getLocale() const;
    void setLocale(const QLocale &l);
    QStringList getDeactivatedPlugins() const;
    void setDeactivatedPlugins(const QStringList &list);
    void addDeactivatedPlugin(const QString &pluginName);
    QSettings *createSettingsInstance(const QString &fileName) const;
    QString confFileName(const QString &path, const QString &name) const;
    QString prefix(BCoreApplication::ResourcesType type, bool forPlugins = false) const;
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void installTranslator(BTranslator *translator);
    void removeTranslator(BTranslator *translator);
    void loadSettings();
    void saveSettings();
public Q_SLOTS:
    void initSettings();
    void sendLanguageChangeEvent();
public:
    mutable QString appName;
    mutable QString orgName;
    mutable QString appPath;
    mutable QString userPrefix;
    mutable QString sharedPrefix;
#if defined(Q_OS_LINUX)
    mutable QString sharedPrefixPlugins;
#endif
#if defined(Q_OS_MAC)
    mutable QString bundlePrefix;
#endif
    QPointer<QSettings> settings;
    QMap<QString, BTranslator *> translators;
    QList<BPluginWrapper *> plugins;
    BPersonInfoProvider *beqtAuthors;
    BPersonInfoProvider *beqtTranslations;
    BPersonInfoProvider *beqtThanksTo;
    QString appCopyrightYears;
    QString appDescription;
    QString appChangeLog;
    QString appLicense;
    QString appDescriptionFileName;
    QString appChangeLogFileName;
    QString appLicenseFileName;
    BPersonInfoList appAuthorsList;
    BPersonInfoList appTranslationsList;
    BPersonInfoList appThanksToList;
    BPersonInfoProvider *appAuthors;
    BPersonInfoProvider *appTranslations;
    BPersonInfoProvider *appThanksTo;
    BLogger *logger;
    BSignalDelayProxy *languageChangeProxy;
    BSignalDelayProxy *languageChangeEventProxy;
    bool blockLanguageChangeEvent;
    bool destructorCalled;
private:
    Q_DISABLE_COPY(BCoreApplicationPrivate)
    friend class BPluginWrapperPrivate;
    friend class BTranslatorPrivate;
    friend class BCoreApplicationEventFilter;
};

#endif // BCOREAPPLICATION_P_H
