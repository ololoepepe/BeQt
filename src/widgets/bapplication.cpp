#include "bapplication.h"
#include "baboutdialog.h"
#include "bsettingsdialog.h"
#include "bhelpbrowser.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/private/bcoreapplication_p.h>
#include <BeQtCore/BDirTools>

#include <QObject>
#include <QIcon>
#include <QString>
#include <QFileInfo>
#include <QApplication>
#include <QPixmap>
#include <QSize>
#include <QScopedPointer>
#include <QStringList>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QTextBrowser>

#include <QDebug>
#include <QPointer>

class BApplicationPrivate : public BCoreApplicationPrivate
{
public:
    BApplicationPrivate(BApplication *q, const BCoreApplication::AppOptions &options);
    ~BApplicationPrivate();
    //
    void initAboutDlg();
    void showAbout();
    QString helpContext(QWidget *widget = 0) const;
    QString getHelpIndex() const;
    QStringList helpSearchPaths() const;
    void openInternalHelp(const QString &file);
    void openExternalOfflineHelp(const QString &file);
    void openExternalOnlineHelp(const QString &file);
    void showHelp( const QString &file = QString() );
    //
    QString homepage;
    BAboutDialog *aboutDlg;
    BApplication::SettingsTabNavigation navigation;
    BApplication::HelpMode helpMode;
    QString helpIndex;
private:
    Q_DISABLE_COPY(BApplicationPrivate)
};

//

BApplicationPrivate::BApplicationPrivate(BApplication *q, const BCoreApplication::AppOptions &options) :
    BCoreApplicationPrivate(q, options)
{
    aboutDlg = 0;
    navigation = BApplication::DefaultNavigation;
    helpMode = BApplication::InternalMode;
}

BApplicationPrivate::~BApplicationPrivate()
{
    if (aboutDlg)
    {
        aboutDlg->close();
        aboutDlg->deleteLater();
    }
}

void BApplicationPrivate::initAboutDlg()
{
    if (aboutDlg)
        return;
    BAboutDialog::AboutOptions opt;
    opt.aboutQtButton = true;
    opt.aboutBeQtButton = true;
    aboutDlg = new BAboutDialog(opt);
    aboutDlg->setWindowModality(Qt::NonModal);
}

void BApplicationPrivate::showAbout()
{
    initAboutDlg();
    if ( aboutDlg->isVisible() )
        aboutDlg->activateWindow();
    else
        aboutDlg->open();
}

QString BApplicationPrivate::helpContext(QWidget *widget) const
{
    if (!widget)
        widget = QApplication::focusWidget();
    if (!widget)
        return "";
    QString context = widget->property("help").toString();
    if ( context.isEmpty() )
    {
        QWidget *pw = widget->parentWidget();
        return pw ? helpContext(pw) : QString();
    }
    else
    {
        return context;
    }
}

QString BApplicationPrivate::getHelpIndex() const
{
    return !helpIndex.isEmpty() ? helpIndex : QString("index.html");
}

QStringList BApplicationPrivate::helpSearchPaths() const
{
    QStringList paths;
    foreach ( const QString &loc, BApplication::locations(BApplication::DocumentationPath) )
        paths << BDirTools::localeBasedDirName(loc);
    return paths;
}

void BApplicationPrivate::openInternalHelp(const QString &file)
{
    BHelpBrowser *hb = new BHelpBrowser(helpSearchPaths(), getHelpIndex(), file);
    hb->setAttribute(Qt::WA_DeleteOnClose, true);
    hb->show();
}

void BApplicationPrivate::openExternalOfflineHelp(const QString &file)
{
    QString fn = file;
    if ( !QFileInfo(file).isAbsolute() )
    {
        foreach ( const QString &path, helpSearchPaths() )
        {
            if ( QFileInfo(path + "/" + file).exists() )
            {
                fn = path + "/" + file;
                break;
            }
        }
    }
    if ( !QFileInfo(fn).isAbsolute() )
        return;
    QUrl url = QUrl::fromLocalFile(fn);
    if ( !url.isValid() )
        return;
    QDesktopServices::openUrl(url);
}

void BApplicationPrivate::openExternalOnlineHelp(const QString &file)
{
    QUrl url = QUrl::fromUserInput(file);
    if ( !url.isValid() )
        return;
    QDesktopServices::openUrl(url);
}

void BApplicationPrivate::showHelp(const QString &file)
{
    QString fn = !file.isEmpty() ? file : getHelpIndex();
    switch (helpMode)
    {
    case BApplication::InternalMode:
        openInternalHelp(fn);
        break;
    case BApplication::ExternalOfflineMode:
        openExternalOfflineHelp(fn);
        break;
    case BApplication::ExternalOnlineMode:
        openExternalOnlineHelp(fn);
        break;
    default:
        break;
    }
}

//

QIcon BApplication::beqtIcon(const QString &fileName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QIcon();
    if ( fileName.isEmpty() )
        return QIcon();
    QString dir = location(BeqtPath, SharedResources) + "/" + "images/icons";
    if ( !QFileInfo(dir).isDir() )
        return QIcon();
    QString fn = fileName;
    if ( QFileInfo(fileName).suffix().isEmpty() )
        fn += ".png";
    return QIcon(dir + "/" + fn);
}

QPixmap BApplication::beqtPixmap(const QString &fileName, const QSize &scale)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return QPixmap();
    if ( fileName.isEmpty() )
        return QPixmap();
    QString dir = location(BeqtPath, SharedResources) + "/" + "images/icons";
    if ( !QFileInfo(dir).isDir() )
        return QPixmap();
    QString fn = fileName;
    if ( QFileInfo(fileName).suffix().isEmpty() )
        fn += ".png";
    QPixmap pm(dir + "/" + fn);
    return scale.isEmpty() ? pm : pm.scaled(scale, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void BApplication::setAboutPixmap(const QPixmap &pixmap)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setPixmap(pixmap);
}

void BApplication::setAboutPixmap(const QString &fileName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setPixmap(fileName);
}

void BApplication::setAbout(const QString &description, const QString &copyright, const QString &website)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->homepage = website;
    ds->initAboutDlg();
    ds->aboutDlg->setAbout(description, copyright, website);
}

void BApplication::setAboutChangeLog(const QString &fileName, const char *codecName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setChangeLog(fileName, codecName);
}

void BApplication::setAboutAuthorsInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setAuthorsInfos(infos);
}

void BApplication::setAboutTranslationInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setTranslationInfos(infos);
}

void BApplication::setAboutThanksToInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setThanksToInfos(infos);
}

void BApplication::setAboutLicense(const QString &text)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setLicense(text);
}

void BApplication::setAboutLicense(const QString &fileName, const char *codecName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    B_DS(BApplication);
    ds->initAboutDlg();
    ds->aboutDlg->setLicense(fileName, codecName);
}

void BApplication::setSettingsTabDefaultNavigation(SettingsTabNavigation navigation)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->navigation = navigation;
}

void BApplication::setHelpMode(HelpMode mode)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->helpMode = mode;
}

void BApplication::setHelpIndex(const QString &index)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    ds_func()->helpIndex = index;
}

//

BApplication::BApplication(const AppOptions &options) :
    BCoreApplication( *new BApplicationPrivate(this, options) )
{
    //
}

BApplication::~BApplication()
{
    //
}

//

void BApplication::showAboutDialog()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    d_func()->showAbout();
}

void BApplication::showSettingsDialog(SettingsTabNavigation navigation)
{
    BSettingsDialog::Navigation nvg;
    switch (navigation)
    {
    case ListNavigation:
        nvg = BSettingsDialog::ListNavigation;
        break;
    case TabbedNavigation:
        nvg = BSettingsDialog::TabbedNavigation;
        break;
    default:
        nvg = (TabbedNavigation == d_func()->navigation) ? BSettingsDialog::TabbedNavigation :
                                                           BSettingsDialog::ListNavigation;
        break;
    }
    QScopedPointer<BSettingsDialog> sd( new BSettingsDialog( settingsTabMap(), nvg, QApplication::activeWindow() ) );
    if (sd->exec() != BSettingsDialog::Accepted)
        return;
    handleSettings( sd->settingsMap() );
}

void BApplication::showHelpContents()
{
    d_func()->showHelp();
}

void BApplication::showContextualHelp()
{
    B_D(BApplication);
    d->showHelp( d->helpContext() );
}

void BApplication::openHomepage()
{
    const B_D(BApplication);
    QString url = !d->homepage.isEmpty() ? d->homepage : QApplication::organizationDomain();
    QDesktopServices::openUrl( QUrl::fromUserInput(url) );
}

//

BApplication::BApplication(BApplicationPrivate &d) :
    BCoreApplication(d)
{
    //
}

BSettingsDialog::SettingsTabMap BApplication::settingsTabMap() const
{
    return BSettingsDialog::SettingsTabMap();
}

void BApplication::handleSettings(const BSettingsDialog::SettingsMap &s)
{
    //
}
