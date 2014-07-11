/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the examples of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <BAboutDialog>
#include <BAbstractDocumentDriver>
#include <BAbstractEditorModule>
#include <BApplication>
#include <BCodeEditor>
#include <BEditEditorModule>
#include <BGuiTools>
#include <BIndicatorsEditorModule>
#include <BOpenSaveEditorModule>
#include <BSearchEditorModule>
#include <BTerminal>

#include <QAction>
#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QString>
#include <QToolBar>
#include <QVariant>
#include <QWidget>

int main(int argc, char **argv)
{
    //Creating BApplication instance
    BApplication::InitialSettings settings;
    settings.applicationName = "Simple Editor";
    settings.organizationName = "Andrey Bogdanov";
    BApplication app(argc, argv, settings);
    BApplication::setOrganizationDomain("https://github.com/the-dark-angel");
    BApplication::setApplicationVersion("0.1.0");
    BTerminal::setMode(BTerminal::StandardMode);
    bLogger->setIncludeDateTime(false);
    bLog("Application started", BLogger::InfoLevel);
    //BApplication::setThemedIconsEnabled(false);
    BApplication::installBeqtTranslator("beqt");
    BAboutDialog::setDefaultMinimumSize(800, 400);
    //Initializing BApplication About
    BApplication::aboutDialogInstance()->setOrganization("Andrey Bogdanov", "2012-2014");
    BApplication::aboutDialogInstance()->setWebsite(BApplication::organizationDomain());
    BApplication::aboutDialogInstance()->setDescription("<center>Simple code editor.<br><a href="
                                                        + BApplication::organizationDomain()
                                                        + ">homepage</a></center>");
    //Creating code editor and loading related settings
    BCodeEditor *cedtr = new BCodeEditor;
    QSettings *s = BApplication::settingsInstance();
    cedtr->driver()->restoreState(s ? s->value("editor/driver_dialog_state").toByteArray() : QByteArray());
    BAbstractEditorModule *osmdl = cedtr->module(BCodeEditor::OpenSaveModule);
    BAbstractEditorModule *emdl = cedtr->module(BCodeEditor::EditModule);
    BSearchEditorModule *smdl = static_cast<BSearchEditorModule *>(cedtr->module(BCodeEditor::SearchModule));
    smdl->restoreState(s ? s->value("editor/search_dialog_state").toByteArray() : QByteArray());
    BAbstractEditorModule *imdl = cedtr->module(BCodeEditor::IndicatorsModule);
    //Creating Main window
    QMainWindow *mw = new QMainWindow;
    mw->resize(1200, 800);
    mw->move(400, 200);
    mw->installEventFilter(cedtr->dropHandler());
    mw->installEventFilter(cedtr->closeHandler());
    mw->setAcceptDrops(true);
    mw->setCentralWidget(cedtr);
    mw->statusBar()->addPermanentWidget(imdl->widget(BIndicatorsEditorModule::FileTypeIndicator));
    mw->statusBar()->addPermanentWidget(imdl->widget(BIndicatorsEditorModule::CursorPositionIndicator));
    mw->statusBar()->addPermanentWidget(imdl->widget(BIndicatorsEditorModule::EncodingIndicator));
    QObject::connect(smdl, SIGNAL(message(QString)), mw->statusBar(), SLOT(showMessage(QString)));
    //Creating menus
    QMenu *mnu = mw->menuBar()->addMenu("File");
    mnu->addActions(osmdl->actions(BOpenSaveEditorModule::OpenActionGroup, true));
    mnu->addMenu(static_cast<BOpenSaveEditorModule *>(osmdl)->fileHistoryMenu());
    mnu->addSeparator();
    mnu->addActions(osmdl->actions(BOpenSaveEditorModule::SaveActionGroup, true));
    mnu->addSeparator();
    mnu->addActions(osmdl->actions(BOpenSaveEditorModule::CloseActionGroup, true));
    mnu->addSeparator();
    QAction *act = new QAction("Quit", mnu);
    QObject::connect(act, SIGNAL(triggered()), mw, SLOT(close()));
    mnu->addAction(act);
    mnu = mw->menuBar()->addMenu("Edit");
    mnu->addActions(emdl->actions(BEditEditorModule::UndoRedoActionGroup));
    mnu->addSeparator();
    mnu->addActions(emdl->actions(BEditEditorModule::ClipboardActionGroup));
    mnu->addSeparator();
    mnu->addActions(smdl->actions());
    mnu->addSeparator();
    mnu->addAction(emdl->action(BEditEditorModule::SwitchModeAction));
    mnu = mw->menuBar()->addMenu("Help");
    mnu->addAction(BGuiTools::createStandardAction(BGuiTools::HomepageAction));
    mnu->addSeparator();
    mnu->addAction(BGuiTools::createStandardAction(BGuiTools::WhatsThisAction));
    mnu->addSeparator();
    mnu->addAction(BGuiTools::createStandardAction(BGuiTools::AboutAction));
    //Creating toolbars
    QToolBar *tbar = mw->addToolBar("Open");
    tbar->setObjectName("ToolBarOpen");
    tbar->addActions(osmdl->actions(BOpenSaveEditorModule::OpenActionGroup, true));
    tbar = mw->addToolBar("Save");
    tbar->setObjectName("ToolBarSave");
    tbar->addActions(osmdl->actions(BOpenSaveEditorModule::SaveActionGroup, true));
    tbar = mw->addToolBar("Undo/Redo");
    tbar->setObjectName("ToolBarUndoRedo");
    tbar->addActions(emdl->actions(BEditEditorModule::UndoRedoActionGroup));
    tbar->addSeparator();
    tbar->addAction(emdl->action(BEditEditorModule::SwitchModeAction));
    tbar = mw->addToolBar("Clipboard");
    tbar->setObjectName("ToolBarClipboard");
    tbar->addActions(emdl->actions(BEditEditorModule::ClipboardActionGroup));
    tbar = mw->addToolBar("Search");
    tbar->setObjectName("ToolBarSearch");
    tbar->addActions(smdl->actions());
    //Restoring main window settings and showing it
    mw->restoreGeometry(s ? s->value("main_window/geometry").toByteArray() : mw->saveGeometry());
    mw->restoreState(s ? s->value("main_window/state").toByteArray() : mw->saveState()) ;
    mw->show();
    //Running main event loop
    int ret = app.exec();
    //Saving settings
    if (s) {
        s->setValue("editor/driver_dialog_state", cedtr->driver()->saveState());
        s->setValue("editor/search_dialog_state", smdl->saveState());
        s->setValue("main_window/geometry", mw->saveGeometry());
        s->setValue("main_window/state", mw->saveState());
        s->sync();
    }
    //Deleting objects
    delete mw;
    //Returning from main
    BTerminal::destroy();
    return ret;
}
