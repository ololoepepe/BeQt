#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPluginWrapper;
class BTranslator;
class BPluginWrapperPrivate;
class BTranslatorPrivate;
class BDirTools;

class QString;
class QLocale;
class QSettings;

#include "bcoreapplication.h"
#include "bglobal.h"
#include "bbase_p.h"

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QStringList>

/*============================================================================
================================ Core Application Private
============================================================================*/

class B_CORE_EXPORT BCoreApplicationPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCoreApplication)
    B_DECLARE_PUBLIC_S(BCoreApplication)
public:
    static QString toLowerNoSpaces(const QString &string);
    static QString subdir(BCoreApplication::Location loc);
    static bool testCoreInit(const char *where = 0);
    static bool testCoreUnique();
    static QSettings *createSettingsInstance(const QString &fileName);
    //
    static const QStringList PluginSuffixes;
    static const QString SettingsGroupBeqt;
      static const QString SettingsGroupCore;
        static const QString SettingsKeyDeactivatedPlugins;
        static const QString SettingsKeyLocale;
    //
    BCoreApplicationPrivate(BCoreApplication *q);
    virtual ~BCoreApplicationPrivate();
    //
    void init();
    QString confFileName(const QString &path, const QString &name) const;
    QString prefix(BCoreApplication::ResourcesType type) const;
    void pluginActivated(BPluginWrapper *pluginWrapper);
    void pluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitLanguageChange();
    void installTranslator(BTranslator *translator, bool languageChange);
    void removeTranslator(BTranslator *translator, bool languageChange);
    void loadSettings();
    void saveSettings();
    //
    bool initialized;
    QString appName;
    QString orgName;
    QString appPath;
    QString userPrefix;
    QString sharedPrefix;
#if defined(B_OS_MAC)
    QString bundlePrefix;
#endif
    bool portable;
    QLocale locale;
    QStringList deactivatedPlugins;
    QMap<QString, BTranslator *> translators;
    QList<BPluginWrapper *> plugins;
private:
    Q_DISABLE_COPY(BCoreApplicationPrivate)
    //
    friend class BPluginWrapperPrivate;
    friend class BTranslatorPrivate;
    friend class BDirTools;
};

#endif // BCOREAPPLICATION_P_H
