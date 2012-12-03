#include <BApplication>
#include <BCodeEditor>
#include <BAbstractEditorModule>
#include <BBookmarksEditorModule>
#include <BIndicatorsEditorModule>
#include <BSearchEditorModule>
#include <BOpenSaveEditorModule>
#include <BAbstractDocumentDriver>
#include <BLocalDocumentDirver>
#include <BEditEditorModule>
#include <BPersonInfoProvider>
#include <BAboutDialog>

#include <QApplication>
#include <QString>
#include <QMainWindow>
#include <QMenu>
#include <QList>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QSettings>
#include <QVariant>
#include <QByteArray>
#include <QToolButton>
#include <QWidget>

#include <QDebug>

int main(int argc, char **argv)
{
    //Creating QApplication instance
    QApplication *app = new QApplication(argc, argv);
    //QApplication initialization
    QApplication::setApplicationName("Simple Editor");
    QApplication::setOrganizationName("Andrey Bogdanov");
    QApplication::setOrganizationDomain("https://github.com/the-dark-angel");
    QApplication::setApplicationVersion("0.1.0pa1");
    //Creating BApplication instance
    BApplication *bapp = new BApplication;
    //Initializing BApplication About
    BApplication::aboutDialogInstance()->setOrganization("Andrey Bogdanov", "2012");
    BApplication::aboutDialogInstance()->setWebsite("https://github.com/the-dark-angel");
    BApplication::aboutDialogInstance()->setDescription("Simple code editor.");
    //QMainWindow
    QMainWindow *mw = new QMainWindow;
      BCodeEditor *cedtr = new BCodeEditor;
        mw->installEventFilter( cedtr->dropHandler() );
        mw->setAcceptDrops(true);
      mw->setCentralWidget(cedtr);
      //
      QMenu *mnuBM = mw->menuBar()->addMenu("Find");
        BSearchEditorModule *smdl = static_cast<BSearchEditorModule *>( cedtr->module(BCodeEditor::SearchModule) );
        mnuBM->addActions( smdl->actions() );
        mnuBM->addAction( BApplication::createStandardAction(BApplication::AboutAction, mnuBM) );
        QObject::connect( smdl, SIGNAL( message(QString) ), mw->statusBar(), SLOT( showMessage(QString) ) );
      //
      QToolBar *tbar = new QToolBar;
        tbar->setWindowTitle("Search");
        tbar->addActions( mnuBM->actions() );
      mw->addToolBar(tbar);
      //
      BIndicatorsEditorModule *imdl = static_cast<BIndicatorsEditorModule *>(
                  cedtr->module(BCodeEditor::IndicatorsModule) );
      mw->statusBar()->addPermanentWidget( imdl->indicator(BIndicatorsEditorModule::FileTypeIndicator) );
      mw->statusBar()->addPermanentWidget( imdl->indicator(BIndicatorsEditorModule::CursorPositionIndicator) );
      mw->statusBar()->addPermanentWidget( imdl->indicator(BIndicatorsEditorModule::EncodingIndicator) );
      //
      BOpenSaveEditorModule *osmdl = static_cast<BOpenSaveEditorModule *>(
                  cedtr->module(BCodeEditor::OpenSaveModule) );
      tbar = new QToolBar;
        tbar->setWindowTitle("Open");
        tbar->addActions( osmdl->openActions() );
        tbar->addAction( osmdl->fileHistoryMenu()->menuAction() );
      mw->addToolBar(tbar);
      tbar = new QToolBar;
        tbar->setWindowTitle("Save");
        tbar->addActions( osmdl->saveActions() );
      mw->addToolBar(tbar);
      tbar = new QToolBar;
        tbar->setWindowTitle("Close");
        tbar->addActions( osmdl->closeActions() );
      mw->addToolBar(tbar);
      //
      BEditEditorModule *emdl = static_cast<BEditEditorModule *>( cedtr->module(BCodeEditor::EditModule) );
      tbar = new QToolBar;
        tbar->setWindowTitle("Clipboard");
        tbar->addActions( emdl->clipboardActions() );
      mw->addToolBar(tbar);
      tbar = new QToolBar;
        tbar->setWindowTitle("UndoRedo");
        tbar->addActions( emdl->undoRedoActions() );
        tbar->addSeparator();
        tbar->addAction( emdl->action(BEditEditorModule::SwitchModeAction) );
      mw->addToolBar(tbar);
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
    //Returning from main
    return ret;
}