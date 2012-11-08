#ifndef BAPPLICATION_H
#define BAPPLICATION_H

class BApplicationPrivate;

class QIcon;
class QString;
class QPixmap;

#include "baboutdialog.h"
#include "bsettingsdialog.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>

#include <QObject>
#include <QSize>

#if defined(bApp)
#   undef bApp
#endif
#define bApp ( static_cast<BApplication *>( BCoreApplication::instance() ) )

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
    //
    static QIcon beqtIcon(const QString &fileName);
    static QPixmap beqtPixmap( const QString &fileName, const QSize &scale = QSize() );
    static void setAboutPixmap(const QPixmap &pixmap);
    static void setAboutPixmap(const QString &fileName);
    static void setAbout( const QString &description, const QString &copyright, const QString &website = QString() );
    static void setAboutChangeLog(const QString &fileName, const char *codecName);
    static void setAboutAuthorsInfos(const BAboutDialog::PersonInfoList &infos);
    static void setAboutTranslationInfos(const BAboutDialog::PersonInfoList &infos);
    static void setAboutThanksToInfos(const BAboutDialog::PersonInfoList &infos);
    static void setAboutLicense(const QString &text);
    static void setAboutLicense(const QString &fileName, const char *codecName);
    static void setSettingsTabDefaultNavigation(SettingsTabNavigation navigation);
    //
    explicit BApplication( const AppOptions &options = AppOptions() );
    ~BApplication();
public slots:
    void showAboutDialog();
    void showSettingsDialog(SettingsTabNavigation navigation = DefaultNavigation);
protected:
    BApplication(BApplicationPrivate &d);
    //
    virtual BSettingsDialog::SettingsTabMap settingsTabMap() const;
    virtual void handleSettings(const BSettingsDialog::SettingsMap &s);
private:
    Q_DISABLE_COPY(BApplication)
};

#endif // BAPPLICATION_H
