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

#ifndef BAPPLICATIONBASE_P_H
#define BAPPLICATIONBASE_P_H

class BLogger;
class BPersonInfoProvider;
class BPluginWrapper;
class BSignalDelayProxy;
class BTranslator;

class QEvent;
class QLocale;

#include "bapplicationbase.h"

#include "babstractlocationprovider.h"
#include "bbaseobject_p.h"
#include "bpersoninfolist.h"

#include <QList>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QSettings>
#include <QString>
#include <QStringList>

/*============================================================================
================================ BInternalLocationProvider ===================
============================================================================*/

class B_CORE_EXPORT BInternalLocationProvider : public BAbstractLocationProvider
{
private:
    const QString AppName;
    const QStringList Names;
    const QString OrgName;
    const bool Portable;
    const QString SharedPrefix;
    const QString UserPrefix;
public:
    explicit BInternalLocationProvider(const QString &appName, const QString &orgName, bool portable);
    ~BInternalLocationProvider();
private:
    static QStringList createNames();
    static QString createSharedPrefix();
    static QString createUserPrefix(const QString &appName, const QString &orgName);
    static QString sharedPrefix(const QString &sharedPrefix, bool plugins = false);
public:
    bool canCreateLocationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
    bool createLocationPath(const QString &locationName, BApplicationBase::ResourceType type);
    QStringList locationNames() const;
    QString locationPath(const QString &locationName, BApplicationBase::ResourceType type) const;
};

/*============================================================================
================================ BApplicationBasePrivate =====================
============================================================================*/

class B_CORE_EXPORT BApplicationBasePrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplicationBase)
    B_DECLARE_PUBLIC_S(BApplicationBase)
public:
    static const QString DefaultAppName;
    static const QString DefaultOrgName;
    static const QStringList PluginSuffixes;
public:
    BPersonInfoProvider *appAuthors;
    BPersonInfoList appAuthorsList;
    QString appChangeLog;
    QString appChangeLogFileName;
    QString appCopyrightYears;
    QString appDescription;
    QString appDescriptionFileName;
    QString appLicense;
    QString appLicenseFileName;
    QString appName;
    BPersonInfoProvider *appThanksTo;
    BPersonInfoList appThanksToList;
    BPersonInfoList appTranslationsList;
    BPersonInfoProvider *appTranslations;
    BPersonInfoProvider *beqtThanksTo;
    BPersonInfoProvider *beqtAuthors;
    BPersonInfoProvider *beqtTranslations;
    bool blockLanguageChangeEvent;
    QList<BApplicationBase::CopyrightInfo> copyrightInfos;
    bool destructorCalled;
    BInternalLocationProvider *internalLocationProvider;
    BSignalDelayProxy *languageChangeEventProxy;
    BSignalDelayProxy *languageChangeProxy;
    QList<BAbstractLocationProvider *> locationProviders;
    BLogger *logger;
    QString orgName;
    QMap<QObject *, BPluginWrapper *> plugins;
    bool portable;
    QPointer<QSettings> settings;
    QMap<QString, BTranslator *> translators;
public:
    explicit BApplicationBasePrivate(BApplicationBase *q);
    ~BApplicationBasePrivate();
public:
    static QString confFileName(const QString &path, const QString &appName);
    static BApplicationBase::LocaleSupportInfo createLocaleSupportInfo();
    static BApplicationBasePrivate *instance();
    static QString platformSpecificAppName(const QString &appName);
    static bool testInit(const char *where = 0);
    static bool testUnique();
    static QString toLowerNoSpaces(const QString &string);
public:
    virtual void emitLanguageChanged() = 0;
    virtual void emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper) = 0;
    virtual void emitPluginActivated(BPluginWrapper *pluginWrapper) = 0;
    bool eventFilter(QObject *o, QEvent *e);
    QLocale getLocale() const;
    void init(BApplicationBase::Portability portability = BApplicationBase::AutoDetect);
    void installPlugin(BPluginWrapper *plugin);
    void installTranslator(BTranslator *translator);
    bool isPluginActivated(const QString &pluginId) const;
    QString location(const QString &name, BApplicationBase::ResourceType type) const;
    QStringList locations(const QString &name) const;
    void removePlugin(QObject *plugin);
    void removeTranslator(BTranslator *translator);
    void setLocale(const QLocale &l);
    void setPluginActivated(const QString &pluginId, bool activated);
public Q_SLOTS:
    void languageChanged();
    void pluginAboutToBeDeactivated();
    void pluginActivated();
    void pluginDestroyed(QObject *obj);
    void sendLanguageChangeEvent();
private:
    Q_DISABLE_COPY(BApplicationBasePrivate)
};

#endif // BAPPLICATIONBASE_P_H
