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
#include <BDirTools>
#include <BCodeEdit>
#include <BCodeEditor>
#include <BCodeEditorDocument>
#include <BAbstractEditorModule>
#include <BBookmarksEditorModule>
#include <BSearchEditorModule>
#include <BPersonInfoProvider>
#include <BAboutDialog>

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
#include <QMenu>
#include <QList>
#include <QAction>
#include <QMenuBar>

#include <QDir>
#include <QEventLoop>
#include <QTimer>
#include <QSettings>

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
    BAboutDialog *adlg = BApplication::aboutDialogInstance();
    adlg->setOrganization("Andrey Bogdanov", "2012");
    adlg->setWebsite("https://github.com/the-dark-angel/test");
    adlg->setDescription("This is an example application");
    adlg->setPixmap(BApplication::location(BApplication::DataPath,
                                           BApplication::SharedResources) + "/images/myapp.png");
    BPersonInfoProvider::PersonInfo pi;
    pi.name = "Andrey Bogdanov";
    pi.role = "Main developer";
    pi.site = "https://github.com/the-dark-angel";
    pi.mail = "the-dark-angel@yandex.ru";
    adlg->setAuthors(BPersonInfoProvider::PersonInfoList() << pi);
    pi.role = "Translator";
    adlg->setTranslators(BPersonInfoProvider::PersonInfoList() << pi);
    pi.name = "Miku";
    pi.role = "Muse";
    pi.mail = "";
    pi.site = "";
    adlg->setThanksTo(BPersonInfoProvider::PersonInfoList() << pi);
    adlg->setLicense("MIT License");
    //Installing translators
    BTranslator *t = new BTranslator("qt");
    BApplication::installTranslator(t);
    t = new BTranslator("beqt");
    BApplication::installTranslator(t);
    //Creating and initializing GUI
    QApplication::setWindowIcon( QIcon( BDirTools::findResource("images/myapp.png", BDirTools::GlobalOnly) ) );
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
    //Loading settings and plugins
    BApplication::loadSettings();
    BApplication::loadPlugins();
    //Showing widgets
    mw->show();
    //term->show();
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
