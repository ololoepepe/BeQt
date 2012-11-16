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
#include <BApplicationServer>
#include <BPlainTextEdit>

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
#include <QLocalServer>
#include <QFont>

#include <QDir>

#include <QDebug>

const QString AppName = "My App";

int main(int argc, char **argv)
{
    //Initializing BeQt built-in resources
    Q_INIT_RESOURCE(beqt);
    //Creating QApplication instance
    QApplication *app = new QApplication(argc, argv);
    //Checking other application process instances
    BApplicationServer *as = new BApplicationServer;
    if ( !as->tryListen(AppName) )
    {
        bool b = as->sendMessage(AppName, argc, argv);
        delete as;
        return b ? 0 : -1;
    }
    //Continuing QApplication initialization
    QApplication::setApplicationName(AppName);
    QApplication::setOrganizationName("darkangel");
    QApplication::setOrganizationDomain("https://github.com/the-dark-angel/test");
    QApplication::setApplicationVersion("0.1.0pa1");
    //Creating BApplication instance
    BApplication *bapp = new Application;
    //Initializing BApplication About
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
    //Installing translators
    BTranslator *t = new BTranslator("qt");
    BApplication::installTranslator(t);
    t = new BTranslator("beqt");
    BApplication::installTranslator(t);
    //Creating and initializing GUI
    QApplication::setWindowIcon( BApplication::beqtIcon("apply") );
    //MainWindow
    MainWindow *mw = new MainWindow;
      QWidget *w = new QWidget(mw);
        QVBoxLayout *vlt = new QVBoxLayout(w);
          BLocaleComboBox *lcb = new BLocaleComboBox(false, w);
        vlt->addWidget(lcb);
      mw->setCentralWidget(w);
    mw->move(800, 400);
    //BTerminalWidget
    BTerminalWidget *term = new BTerminalWidget(BTerminalWidget::NormalMode);
    term->setDriver(new BLocalTerminalDriver);
    //BPlainTextEdit
    BPlainTextEdit *pte = new BPlainTextEdit;
    pte->setFont( QFont("DejaVu Sans Mono") );
    pte->setSelectionMode(BPlainTextEdit::BlockSelection);
    //Loading settings and plugins
    BApplication::loadSettings();
    //BApplication::loadPlugins();
    //Showing widgets
    //mw->show();
    //term->show();
    pte->show();
    //Running main event loop
    int ret = app->exec();
    //Saving settings
    BApplication::saveSettings();
    //Deleting objects
    delete mw;
    delete term;
    delete bapp;
    delete app;
    delete as;
    //Cleaning up BeQt built-in resources
    Q_CLEANUP_RESOURCE(beqt);
    //Returning from main
    return ret;
}
