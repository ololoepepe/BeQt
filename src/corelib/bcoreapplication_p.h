#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPluginWrapper;
class BTranslator;
class BPluginWrapperPrivate;
class BTranslatorPrivate;

class QString;
class QLocale;

#include "bcoreapplication.h"

#include <BeQt>

#include <QtGlobal>
#include <QList>
#include <QMap>
#include <QStringList>

class B_CORE_EXPORT BCoreApplicationPrivate
{
    B_DECLARE_PUBLIC(BCoreApplication)
public:
    static QString toLowerNoSpaces(const QString &string);
    static QString subdir(BCoreApplication::Location loc);
    static bool testCoreInit(const char *where = 0);
    //
    BCoreApplicationPrivate(BCoreApplication *q, const BCoreApplication::AppOptions &options);
    ~BCoreApplicationPrivate();
    //
    QString confFileName(const QString &path, const QString &name, bool create = false) const;
    QString prefix(BCoreApplication::ResourcesType type) const;
    void emitPluginActivated(BPluginWrapper *pluginWrapper);
    void emitPluginAboutToBeDeactivated(BPluginWrapper *pluginWrapper);
    void emitLanguageChange();
    void installTranslator(BTranslator *translator, bool languageChange);
    void removeTranslator(BTranslator *translator, bool languageChange);
    void loadSettings();
    void saveSettings();
    //
    BCoreApplication *const _m_q;
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
    static const QStringList PluginSuffixes;
    static const QString SettingsGroupBeqt;
      static const QString SettingsGroupCore;
        static const QString SettingsKeyDeactivatedPlugins;
        static const QString SettingsKeyLocale;
    //
    void init(const BCoreApplication::AppOptions &options);
    //
    Q_DISABLE_COPY(BCoreApplicationPrivate)
    //
    friend class BPluginWrapperPrivate;
    friend class BTranslatorPrivate;
};

#endif // BCOREAPPLICATION_P_H
