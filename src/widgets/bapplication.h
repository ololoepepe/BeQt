#ifndef BAPPLICATION_H
#define BAPPLICATION_H

class BApplicationPrivate;

class QIcon;
class QPixmap;
class QAction;
class QFont;

#include "baboutdialog.h"
#include "bsettingsdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QSize>
#include <QString>

#if defined(bApp)
#   undef bApp
#endif
#define bApp ( static_cast<BApplication *>( BCoreApplication::instance() ) )

/*============================================================================
================================ Application
============================================================================*/

class B_WIDGETS_EXPORT BApplication : public BCoreApplication
{
    B_DECLARE_PRIVATE(BApplication)
    B_DECLARE_PRIVATE_S(BApplication)
    Q_OBJECT
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
    //
    static QIcon icon( const QString &name, const QString &theme = QString() );
    static QIcon beqtIcon(const QString &name);
    static QPixmap beqtPixmap( const QString &name, const QSize &scale = QSize() );
    static void setIconCachingEnabled(bool enabled);
    static void clearIconCache();
    static void setAboutPixmap(const QPixmap &pixmap);
    static void setAboutPixmap(const QString &fileName);
    static void setAbout( const QString &description, const QString &copyright, const QString &website = QString() );
    static void setAboutChangeLog(const QString &fileName, const char *codecName);
    static void setAboutAuthorsInfos(const BPersonInfoProvider::PersonInfoList &infos);
    static void setAboutTranslationInfos(const BPersonInfoProvider::PersonInfoList &infos);
    static void setAboutThanksToInfos(const BPersonInfoProvider::PersonInfoList &infos);
    static void setAboutLicense(const QString &text);
    static void setAboutLicense(const QString &fileName, const char *codecName);
    static void setSettingsTabDefaultNavigation(SettingsTabNavigation navigation);
    static void setHelpIndex(const QString &index);
    static QAction *createStandardAction(StandardAction type, QObject *parent = 0);
    static QFont createMonospaceFont();
    //
    explicit BApplication();
    ~BApplication();
public slots:
    void showAboutDialog();
    void showSettingsDialog();
    void showSettingsDialog(SettingsTabNavigation navigation);
    void showHelpContents();
    void showContextualHelp();
    void openHomepage();
protected:
    BApplication(BApplicationPrivate &d);
    //
    virtual BSettingsDialog::SettingsTabMap settingsTabMap() const;
    virtual void handleSettings(const BSettingsDialog::SettingsMap &s);
private:
    Q_DISABLE_COPY(BApplication)
};

#endif // BAPPLICATION_H
