#ifndef BCOREAPPLICATION_P_H
#define BCOREAPPLICATION_P_H

class BPlugin;
class BTranslator;
class BPluginPrivate;
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
    void emitPluginActivated(BPlugin *plugin);
    void emitPluginAboutToBeDeactivated(BPlugin *plugin);
    void installTranslator(BTranslator *translator);
    void removeTranslator(BTranslator *translator);
    //
    BCoreApplication *_m_q;
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
    QList<BTranslator *> internalTranslators;
    QList<BTranslator *> userTranslators;
    QMap<QString, BPlugin *> plugins;
private:
    static const QStringList PluginSuffixes;
    static const QString SettingsGroupBeqt;
    static const QString SettingsKeyDeactivatedPlugins;
    static const QString SettingsKeyLocale;
    //
    void init(const BCoreApplication::AppOptions &options);
    //
    Q_DISABLE_COPY(BCoreApplicationPrivate)
    //
    friend class BPluginPrivate;
    friend class BTranslatorPrivate;
};

#endif // BCOREAPPLICATION_P_H
