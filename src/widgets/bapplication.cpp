#include "bapplication.h"
#include "baboutdialog.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/private/bcoreapplication_p.h>

#include <QObject>
#include <QIcon>
#include <QString>
#include <QFileInfo>
#include <QApplication>

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
    void showAbout()
    {
        if (!aboutDlg)
        {
            BAboutDialog::AboutOptions opt;
            opt.aboutQtButton = true;
            opt.aboutBeQtButton = true;
            aboutDlg = new BAboutDialog(opt);
            aboutDlg->setIcon(BApplication::location(BApplication::DataPath,
                                                     BApplication::SharedResources) + "/images/myapp.png");
        }
        //test
        aboutDlg->setAbout("This is an example application", "2012 Andrey Bogdanov",
                           "https://github.com/the-dark-angel/test");
        aboutDlg->setChangeLog("=== My App 0.1.0pa1 (04 November 2012) ===\n    * First pre-alpha release");
        BAboutDialog::PersonInfo pi;
        pi.name = "Andrey Bogdanov";
        pi.role = "Main developer";
        pi.mail = "the-dark-angel@yandex.ru";
        pi.site = "https://github.com/the-dark-angel";
        aboutDlg->setAuthorsInfos(BAboutDialog::PersonInfoList() << pi);
        pi.role = "Translator";
        aboutDlg->setTranslationInfos(BAboutDialog::PersonInfoList() << pi);
        pi.name = "Miku";
        pi.role = "Muse";
        pi.mail = "";
        pi.site = "";
        aboutDlg->setThanksToInfos(BAboutDialog::PersonInfoList() << pi);
        aboutDlg->setLicense("MIT license");
        //end test
        if ( aboutDlg->isVisible() )
        {
            aboutDlg->activateWindow();
        }
        else
        {
            aboutDlg->setParent( QApplication::activeWindow() );
            aboutDlg->exec();
        }
        aboutDlg->setParent(0);
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

void BApplication::showAbout()
{
    if ( !BCoreApplicationPrivate::testCoreInit("BApplication") )
        return;
    d_func()->showAbout();
}

//

BApplication::BApplication(BApplicationPrivate &d) :
    BCoreApplication(d)
{
    //
}
