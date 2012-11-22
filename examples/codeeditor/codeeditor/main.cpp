#include <BApplication>
#include <BCodeEditor>
#include <BAbstractEditorModule>
#include <BBookmarksEditorModule>
#include <BSearchEditorModule>

#include <QApplication>
#include <QString>
#include <QMainWindow>
#include <QMenu>
#include <QList>
#include <QAction>
#include <QMenuBar>

#include <QDebug>

int main(int argc, char **argv)
{
    //Initializing BeQt built-in resources
    Q_INIT_RESOURCE(beqt);
    //Creating QApplication instance
    QApplication *app = new QApplication(argc, argv);
    //QApplication initialization
    QApplication::setApplicationName("Code Editor");
    QApplication::setOrganizationName("darkangel");
    QApplication::setOrganizationDomain("https://github.com/the-dark-angel/test");
    QApplication::setApplicationVersion("0.1.0pa1");
    //Creating BApplication instance
    BApplication *bapp = new BApplication;
    //Initializing BApplication About
    BApplication::setAbout("This is an example application", "2012 Andrey Bogdanov",
                           "https://github.com/the-dark-angel/test");
    //MainWindow
    QMainWindow *mw = new QMainWindow;
      BCodeEditor *cedtr = new BCodeEditor;
        cedtr->openDocument("/home/darkangel/tmp/texput.log");
        cedtr->addDocument("Test.txt");
        cedtr->openDocument("/home/darkangel/tmp/texput.log");
      mw->setCentralWidget(cedtr);
      //
      QMenu *mnuBM = mw->menuBar()->addMenu("Find");
        mnuBM->addActions( static_cast<BSearchEditorModule *>( cedtr->modules().first() )->actions() );
    mw->resize(1200, 800);
    mw->move(400, 200);
    //Showing widgets
    mw->show();
    //Running main event loop
    int ret = app->exec();
    //Deleting objects
    delete mw;
    delete bapp;
    delete app;
    //Cleaning up BeQt built-in resources
    Q_CLEANUP_RESOURCE(beqt);
    //Returning from main
    return ret;
}
