#include "application.h"
#include "mainwindow.h"

#include <BApplication>
#include <BTerminalIOHandler>
#include <BAboutDialog>
#include <BLocaleComboBox>
#include <BTranslator>
#include <BSettingsDialog>
#include <BTerminalWidget>
#include <BLocalTerminalDriver>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QEventLoop>
#include <QTimer>
#include <QTextStream>
#include <QWidget>
#include <QIcon>
#include <QLibraryInfo>
#include <QPushButton>
#include <QVBoxLayout>

#include <QDebug>

int main(int argc, char **argv)
{
    QApplication *app = new QApplication(argc, argv);
    QApplication::setApplicationName("My App");
    QApplication::setOrganizationName("darkangel");
    QApplication::setOrganizationDomain("https://github.com/the-dark-angel/test");
    QApplication::setApplicationVersion("0.1.0pa1");
    /*BCoreApplication *bapp = new BCoreApplication;
    delete bapp;
    delete app;
    return 0;*/
    BApplication *bapp = new Application;
    //test
    BApplication::loadSettings();
    //about
    BApplication::setAboutPixmap(BApplication::location(BApplication::DataPath,
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
    //navigation
    //BApplication::setSettingsTabDefaultNavigation(BApplication::DefaultNavigation);
    //end navigation
    QApplication::setWindowIcon( BApplication::beqtIcon("apply") );
    MainWindow *mw = new MainWindow;
    QWidget *w = new QWidget(mw);
    w->move(800, 400);
    QVBoxLayout *vlt = new QVBoxLayout(w);
    //locale combo box
    BTranslator *t = new BTranslator("qt");
    BApplication::installTranslator(t);
    t = new BTranslator("beqt");
    BApplication::installTranslator(t);
    BLocaleComboBox *lcb = new BLocaleComboBox(false, w);
    vlt->addWidget(lcb);
    //lcb->updateAvailableLocales();
    //
    //loading plugins
    BApplication::loadPlugins();
    //
    //BApplication::setHelpMode(BApplication::ExternalOnlineMode);
    QPushButton *btn = new QPushButton("About", w);
    QObject::connect( btn, SIGNAL( clicked() ), bApp, SLOT( showAboutDialog() ) );
    vlt->addWidget(btn);
    QPushButton *btn2 = new QPushButton("Settings", w);
    QObject::connect( btn2, SIGNAL( clicked() ), bApp, SLOT( showSettingsDialog() ) );
    vlt->addWidget(btn2);
    mw->setCentralWidget(w);
    //mw->show();
    mw->loadSettings();
    //terminal
    BTerminalWidget *term = new BTerminalWidget(BTerminalWidget::NormalMode);
    term->setDriver(new BLocalTerminalDriver);
    term->show();
    //term->emulateCommand("tex-creator.sh");
    //end terminal
    int ret = app->exec();
    mw->saveSettings();
    //end test
    BApplication::saveSettings();
    delete bapp;
    delete app;
    return ret;
}
