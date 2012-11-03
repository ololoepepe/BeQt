#include <BApplication>
#include <BTerminalIOHandler>
#include <BAboutDialog>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QEventLoop>
#include <QTimer>
#include <QTextStream>
#include <QMainWindow>
#include <QIcon>
#include <QLibraryInfo>
#include <QPushButton>

#include <QDebug>

int main(int argc, char **argv)
{
    QApplication *app = new QApplication(argc, argv);
    QApplication::setApplicationName("My App");
    QApplication::setOrganizationName("darkangel");
    QApplication::setApplicationVersion("0.1.0pa1");
    BApplication *bapp = new BApplication;
    //test
    BApplication::loadSettings();
    //about
    BApplication::setAboutPixmap( BApplication::location(BApplication::DataPath,
                                                         BApplication::SharedResources) + "/images/myapp.png");
    BApplication::setAbout("This is an example application", "2012 Andrey Bogdanov",
                           "https://github.com/the-dark-angel/test");
    BAboutDialog::PersonInfo pi;
    pi.name = "Andrey Bogdanov";
    pi.role = "Main developer";
    pi.site = "https://github.com/the-dark-angel";
    pi.mail = "the-dark-angel@yandex.ru";
    BApplication::setAboutAuthorsInfos(BAboutDialog::PersonInfoList() << pi);
    pi.role = "Translator";
    BApplication::setAboutTranslationInfos(BAboutDialog::PersonInfoList() << pi);
    pi.name = "Miku";
    pi.role = "Muse";
    pi.mail = "";
    pi.site = "";
    BApplication::setAboutThanksToInfos(BAboutDialog::PersonInfoList() << pi);
    BApplication::setAboutLicense("MIT License");
    //end about
    QApplication::setWindowIcon( BApplication::beqtIcon("apply") );
    QMainWindow *mw = new QMainWindow();
    QPushButton *btn = new QPushButton("About", mw);
    QObject::connect( btn, SIGNAL( clicked() ), bApp, SLOT( showAbout() ) );
    mw->setCentralWidget(btn);
    mw->show();
    int ret = app->exec();
    //end test
    BApplication::saveSettings();
    delete bapp;
    delete app;
    return ret;
}
