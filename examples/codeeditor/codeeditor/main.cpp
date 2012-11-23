#include <BApplication>
#include <BCodeEditor>
#include <BAbstractEditorModule>
#include <BBookmarksEditorModule>
#include <BIndicatorsEditorModule>
#include <BSearchEditorModule>
#include <BFileDialog>

#include <QApplication>
#include <QString>
#include <QMainWindow>
#include <QMenu>
#include <QList>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>

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
    //QMainWindow
    QMainWindow *mw = new QMainWindow;
      BCodeEditor *cedtr = new BCodeEditor;
        cedtr->openDocument("/home/darkangel/tmp/texput.log");
        cedtr->addDocument("Test.txt");
        cedtr->openDocument("/home/darkangel/tmp/texput.log");
      mw->setCentralWidget(cedtr);
      //
      QMenu *mnuBM = mw->menuBar()->addMenu("Find");
        mnuBM->addActions( static_cast<BSearchEditorModule *>( cedtr->module(BCodeEditor::SearchModule) )->actions() );
      //
      QToolBar *tbar = new QToolBar;
        tbar->setWindowTitle("Search");
        tbar->addActions( mnuBM->actions() );
      mw->addToolBar(tbar);
      //
      BIndicatorsEditorModule *imdl = static_cast<BIndicatorsEditorModule *>(
                  cedtr->module(BCodeEditor::IndicatorsModule) );
      mw->statusBar()->addPermanentWidget( imdl->createIndicator(BIndicatorsEditorModule::FileTypeIndicator) );
      mw->statusBar()->addPermanentWidget( imdl->createIndicator(BIndicatorsEditorModule::CursorPositionIndicator) );
      mw->statusBar()->addPermanentWidget( imdl->createIndicator(BIndicatorsEditorModule::EncodingIndicator) );
    mw->resize(1200, 800);
    mw->move(400, 200);
    //BFileDialog
    BFileDialog *fd = new BFileDialog;
    fd->setFileTypes( cedtr->fileTypes() );
    fd->setCodecs( cedtr->supportedCodecs() );
    fd->selectCodec("Windows-1251");
    fd->exec();
    //Showing widgets
    mw->show();
    //Running main event loop
    int ret = app->exec();
    //Deleting objects
    delete fd;
    delete mw;
    delete bapp;
    delete app;
    //Cleaning up BeQt built-in resources
    Q_CLEANUP_RESOURCE(beqt);
    //Returning from main
    return ret;
}
