#ifndef BAPPLICATION_H
#define BAPPLICATION_H

class BApplicationPrivate;
class BAboutDialogPrivate;
class BPersonInfoProvider;
class BAboutDialog;

class QPixmap;
class QAction;
class QFont;

#include "bsettingsdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QSize>
#include <QString>
#include <QIcon>

#if defined(bApp)
#undef bApp
#endif
#define bApp ( static_cast<BApplication *>( BCoreApplication::instance() ) )

/*============================================================================
================================ BApplication ================================
============================================================================*/

class B_WIDGETS_EXPORT BApplication : public BCoreApplication
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BApplication)
    B_DECLARE_PRIVATE_S(BApplication)
public:
    enum SettingsTabNavigation
    {
        DefaultNavigation,
        ListNavigation,
        TabbedNavigation
    };
    enum StandardAction
    {
        SettingsAction,
        HomepageAction,
        HelpContentsAction,
        ContextualHelpAction,
        WhatsThisAction,
        AboutAction
    };
public:
    explicit BApplication();
    ~BApplication();
protected:
    explicit BApplication(BApplicationPrivate &d);
public:
    static QIcon icon( const QString &name, const QIcon &fallback = QIcon() );
    static QIcon beqtIcon(const QString &name);
    static QPixmap beqtPixmap( const QString &name, const QSize &scale = QSize() );
    static void setIconCachingEnabled(bool enabled);
    static void clearIconCache();
    static void setThemedIconsEnabled(bool enabled);
    static bool themedIconsEnabled();
    static void setPreferredIconFormats(const QStringList &suffixes);
    static QStringList preferredIconFormats();
    static BAboutDialog *aboutDialogInstance();
    static void setSettingsTabDefaultNavigation(SettingsTabNavigation navigation);
    static void setHelpIndex(const QString &index);
    static QAction *createStandardAction(StandardAction type, QObject *parent = 0);
    static QFont createMonospaceFont();
    static void setHelpBrowserDefaultGeometry(const QRect &geometry);
public slots:
    void showAboutDialog();
    void showSettingsDialog();
    void showSettingsDialog(SettingsTabNavigation navigation);
    void showHelpContents();
    void showContextualHelp();
    void openHomepage();
protected:
    virtual BSettingsDialog::SettingsTabMap settingsTabMap() const;
    virtual void handleSettings(const BSettingsDialog::SettingsMap &s);
private:
    Q_DISABLE_COPY(BApplication)
    friend class BAboutDialogPrivate;
};

#endif // BAPPLICATION_H
