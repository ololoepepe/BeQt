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

class BPluginWrapper;
class BTranslator;
class BPluginWrapperPrivate;
class BTranslatorPrivate;
class BLogger;
class BSignalDelayProxy;
class BPersonInfoProvider;
class BAbstractLocationProvider;
class BInternalLocationProvider;

class QString;
class QLocale;
class QEvent;

#include "bapplicationbase.h"
#include "bglobal.h"
#include "bbaseobject_p.h"
#include "bpersoninfo.h"

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QVariantMap>
#include <QPointer>
#include <QSettings>

/*============================================================================
================================ BApplicationBasePrivate =====================
============================================================================*/

class B_CORE_EXPORT BApplicationBasePrivate : public BBaseObjectPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BApplicationBase)
    B_DECLARE_PUBLIC_S(BApplicationBase)
public:
    static const QStringList PluginSuffixes;
public:
    explicit BApplicationBasePrivate(BApplicationBase *q);
    ~BApplicationBasePrivate();
public:
    static BApplicationBasePrivate *instance();
    static bool testInit(const char *where = 0);
    static bool testUnique();
    static BApplicationBase::LocaleSupportInfo createLocaleSupportInfo();
public:
    virtual void emitPluginActivated(BPluginWrapper *pluginWrapper) = 0;
    virtual void emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper) = 0;
    virtual void emitLanguageChanged() = 0;
public:
    void init(BApplicationBase::Portability portability = BApplicationBase::AutoDetect);
    bool eventFilter(QObject *o, QEvent *e);
    QString location(const QString &name, BApplicationBase::ResourceType type) const;
    QStringList locations(const QString &name) const;
    QLocale getLocale() const;
    void setLocale(const QLocale &l);
    bool isPluginActivated(const QString &pluginName) const;
    void setPluginActivated(const QString &pluginName, bool activated);
    void installTranslator(BTranslator *translator);
    void removeTranslator(BTranslator *translator);
    void installPlugin(BPluginWrapper *plugin);
    void removePlugin(QObject *plugin);
public Q_SLOTS:
    void sendLanguageChangeEvent();
    void languageChanged();
    void pluginActivated();
    void pluginAboutToBeDeactivated();
    void pluginDestroyed(QObject *obj);
public:
    static const QString DefaultAppName;
    static const QString DefaultOrgName;
public:
    QString appName;
    QString orgName;
    bool portable;
    BInternalLocationProvider *internalLocationProvider;
    QList<BAbstractLocationProvider *> locationProviders;
    QPointer<QSettings> settings;
    QMap<QString, BTranslator *> translators;
    QMap<QObject *, BPluginWrapper *> plugins;
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
    Q_DISABLE_COPY(BApplicationBasePrivate)
};

#endif // BAPPLICATIONBASE_P_H
