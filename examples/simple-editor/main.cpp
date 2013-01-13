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
#include <BTranslator>

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
    bLogger->setIncludeDateTime(false);
    bLog("Application started", BLogger::InfoLevel);
    //BApplication::setThemedIconsEnabled(false);
    BApplication::installTranslator( new BTranslator("beqt") );
    BAboutDialog::setDefaultMinimumSize(800, 400);
    //Initializing BApplication About
    BApplication::aboutDialogInstance()->setOrganization("Andrey Bogdanov", "2012");
    BApplication::aboutDialogInstance()->setWebsite("https://github.com/the-dark-angel");
    BApplication::aboutDialogInstance()->setDescription("Simple code editor.");
    //Creating code editor and loading related settings
    BCodeEditor *cedtr = new BCodeEditor;
    QSettings *s = BApplication::createAppSettingsInstance();
    BLocalDocumentDriver *drv = static_cast<BLocalDocumentDriver *>( cedtr->driver() );
    drv->restoreDialogState( s ? s->value("editor/driver_dialog_state").toByteArray() : QByteArray() );
    BOpenSaveEditorModule *osmdl = static_cast<BOpenSaveEditorModule *>( cedtr->module(BCodeEditor::OpenSaveModule) );
    BEditEditorModule *emdl = static_cast<BEditEditorModule *>( cedtr->module(BCodeEditor::EditModule) );
    BSearchEditorModule *smdl = static_cast<BSearchEditorModule *>( cedtr->module(BCodeEditor::SearchModule) );
    smdl->restoreDialogState( s ? s->value("editor/search_dialog_state").toByteArray() : QByteArray() );
    BIndicatorsEditorModule *imdl =
            static_cast<BIndicatorsEditorModule *>( cedtr->module(BCodeEditor::IndicatorsModule) );
    //Creating Main window
    QMainWindow *mw = new QMainWindow;
    mw->resize(1200, 800);
    mw->move(400, 200);
    mw->installEventFilter( cedtr->dropHandler() );
    mw->setAcceptDrops(true);
    mw->setCentralWidget(cedtr);
    mw->statusBar()->addPermanentWidget( imdl->indicator(BIndicatorsEditorModule::FileTypeIndicator) );
    mw->statusBar()->addPermanentWidget( imdl->indicator(BIndicatorsEditorModule::CursorPositionIndicator) );
    mw->statusBar()->addPermanentWidget( imdl->indicator(BIndicatorsEditorModule::EncodingIndicator) );
    QObject::connect( smdl, SIGNAL( message(QString) ), mw->statusBar(), SLOT( showMessage(QString) ) );
    //Creating menus
    QMenu *mnu = mw->menuBar()->addMenu("File");
    mnu->addActions( osmdl->openActions(true) );
    mnu->addMenu( osmdl->fileHistoryMenu() );
    mnu->addSeparator();
    mnu->addActions( osmdl->saveActions(true) );
    mnu->addSeparator();
    mnu->addActions( osmdl->closeActions(true) );
    mnu->addSeparator();
    QAction *act = new QAction("Quit", mnu);
    QObject::connect( act, SIGNAL( triggered() ), mw, SLOT( close() ) );
    mnu->addAction(act);
    mnu = mw->menuBar()->addMenu("Edit");
    mnu->addActions( emdl->undoRedoActions() );
    mnu->addSeparator();
    mnu->addActions( emdl->clipboardActions() );
    mnu->addSeparator();
    mnu->addActions( smdl->actions() );
    mnu->addSeparator();
    mnu->addAction( emdl->action(BEditEditorModule::SwitchModeAction) );
    mnu = mw->menuBar()->addMenu("Help");
    mnu->addAction( BApplication::createStandardAction(BApplication::HomepageAction) );
    mnu->addSeparator();
    mnu->addAction( BApplication::createStandardAction(BApplication::WhatsThisAction) );
    mnu->addSeparator();
    mnu->addAction( BApplication::createStandardAction(BApplication::AboutAction) );
    //Creating toolbars
    QToolBar *tbar = mw->addToolBar("Open");
    tbar->setObjectName("ToolBarOpen");
    tbar->addActions( osmdl->openActions() );
    tbar = mw->addToolBar("Save");
    tbar->setObjectName("ToolBarSave");
    tbar->addActions( osmdl->saveActions() );
    tbar = mw->addToolBar("Undo/Redo");
    tbar->setObjectName("ToolBarUndoRedo");
    tbar->addActions( emdl->undoRedoActions() );
    tbar->addSeparator();
    tbar->addAction( emdl->action(BEditEditorModule::SwitchModeAction) );
    tbar = mw->addToolBar("Clipboard");
    tbar->setObjectName("ToolBarClipboard");
    tbar->addActions( emdl->clipboardActions() );
    tbar = mw->addToolBar("Search");
    tbar->setObjectName("ToolBarSearch");
    tbar->addActions( smdl->actions() );
    //Restoring main window settings and showing it
    mw->restoreGeometry( s ? s->value("main_window/geometry").toByteArray() : mw->saveGeometry() );
    mw->restoreState( s ? s->value("main_window/state").toByteArray() : mw->saveState() ) ;
    mw->show();
    //Running main event loop
    int ret = app->exec();
    //Saving settings
    if (s)
    {
        s->setValue( "editor/driver_dialog_state", drv->saveDialogState() );
        s->setValue( "editor/search_dialog_state", smdl->saveDialogState() );
        s->setValue( "main_window/geometry", mw->saveGeometry() );
        s->setValue( "main_window/state", mw->saveState() );
        s->sync();
    }
    //Deleting objects
    delete mw;
    delete bapp;
    delete app;
    //Returning from main
    return ret;
}
