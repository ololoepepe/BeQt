#include "bapplication.h"
#include "baboutdialog.h"
#include "bsettingsdialog.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/private/bcoreapplication_p.h>

#include <QObject>
#include <QIcon>
#include <QString>
#include <QFileInfo>
#include <QApplication>
#include <QPixmap>
#include <QSize>
#include <QScopedPointer>

#include <QDebug>

class BApplicationPrivate : public BCoreApplicationPrivate
{
public:
    BApplicationPrivate(BApplication *q, const BCoreApplication::AppOptions &options) :
        BCoreApplicationPrivate(q, options)
    {
        aboutDlg = 0;
    }
    ~BApplicationPrivate()
    {
        if (aboutDlg)
        {
            aboutDlg->close();
            aboutDlg->deleteLater();
        }
    }
    //
    void initAboutDlg()
    {
        if (aboutDlg)
            return;
        BAboutDialog::AboutOptions opt;
        opt.aboutQtButton = true;
        opt.aboutBeQtButton = true;
        aboutDlg = new BAboutDialog(opt);
        aboutDlg->setWindowModality(Qt::NonModal);
    }
    void showAbout()
    {
        initAboutDlg();
        if ( aboutDlg->isVisible() )
            aboutDlg->activateWindow();
        else
            aboutDlg->open();
    }
    //
    BAboutDialog *aboutDlg;
};

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
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setPixmap(pixmap);
}

void BApplication::setAboutPixmap(const QString &fileName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setPixmap(fileName);
}

void BApplication::setAbout(const QString &description, const QString &copyright, const QString &website)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setAbout(description, copyright, website);
}

void BApplication::setAboutChangeLog(const QString &fileName, const char *codecName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setChangeLog(fileName, codecName);
}

void BApplication::setAboutAuthorsInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setAuthorsInfos(infos);
}

void BApplication::setAboutTranslationInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setTranslationInfos(infos);
}

void BApplication::setAboutThanksToInfos(const BAboutDialog::PersonInfoList &infos)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setThanksToInfos(infos);
}

void BApplication::setAboutLicense(const QString &text)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setLicense(text);
}

void BApplication::setAboutLicense(const QString &fileName, const char *codecName)
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    BApplicationPrivate *const d = ds_func();
    d->initAboutDlg();
    d->aboutDlg->setLicense(fileName, codecName);
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

void BApplication::showSettingsDialog()
{
    QScopedPointer<BSettingsDialog> sd( new BSettingsDialog( settingsTabMap(), QApplication::activeWindow() ) );
    if (sd->exec() != BSettingsDialog::Accepted)
        return;
    handleSettings( sd->settingsMap() );
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
