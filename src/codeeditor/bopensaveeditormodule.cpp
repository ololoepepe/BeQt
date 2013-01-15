#include "bopensaveeditormodule.h"
#include "bopensaveeditormodule_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"
#include "babstractfiletype.h"
#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QString>
#include <QList>
#include <QAction>
#include <QTextCodec>
#include <QMenu>
#include <QList>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include <QFileInfo>
#include <QKeySequence>

#include <QDebug>

/*============================================================================
================================ BOpenSaveEditorModulePrivate ================
============================================================================*/

/*============================== Public constructors =======================*/

BOpenSaveEditorModulePrivate::BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BOpenSaveEditorModulePrivate::~BOpenSaveEditorModulePrivate()
{
    if ( !mnuFileHistory.isNull() )
        mnuFileHistory->deleteLater();
}

/*============================== Public methods ============================*/

void BOpenSaveEditorModulePrivate::init()
{
    actNewFile = new QAction(this);
      actNewFile->setIcon( BApplication::icon("filenew") );
      actNewFile->setShortcut(QKeySequence::New);
    actOpenFiles = new QAction(this);
      actOpenFiles->setIcon( BApplication::icon("fileopen") );
      actOpenFiles->setShortcut(QKeySequence::Open);
    actReopenFile = new QAction(this);
      actReopenFile->setIcon( BApplication::icon("reload") );
      actReopenFile->setMenu( new QMenu() );
      QList<BCodeEditor::CodecGroup> list;
      list << BCodeEditor::UnicodeGroup;
      list << BCodeEditor::EastEuropeanGroup;
      list << BCodeEditor::WestEuropeanGroup;
      list << BCodeEditor::EastAsianGroup;
      list << BCodeEditor::SouthEastSouthWestAsianGroup;
      list << BCodeEditor::MiddleEastGroup;
      foreach (BCodeEditor::CodecGroup gr, list)
      {
          QMenu *mnu = new QMenu( actReopenFile->menu() );
            mnu->setProperty("beqt/codec_group", gr);
            foreach ( const QString &cn, BCodeEditor::codecNamesForGroup(gr) )
            {
                QAction *act = new QAction(mnu);
                  act->setProperty("beqt/codec_name", cn);
                  connect( act, SIGNAL( triggered() ), this, SLOT( codecTriggered() ) );
                mnu->addAction(act);
                codecs << act;
            }
          actReopenFile->menu()->addMenu(mnu);
          codecGroups << mnu;
      }
    actSaveFile = new QAction(this);
      actSaveFile->setIcon( BApplication::icon("filesave") );
      actSaveFile->setShortcut(QKeySequence::Save);
    actSaveFileAs = new QAction(this);
      actSaveFileAs->setIcon( BApplication::icon("filesaveas") );
    actSaveAllFiles = new QAction(this);
      actSaveAllFiles->setIcon( BApplication::icon("save_all") );
      actSaveAllFiles->setShortcut(QKeySequence::SaveAs); //Yep, this shortcut is really used for a different action
    actCloseFile = new QAction(this);
      actCloseFile->setIcon( BApplication::icon("fileclose") );
      actCloseFile->setShortcut(QKeySequence::Close);
    actCloseAllFiles = new QAction(this);
      actCloseAllFiles->setIcon( BApplication::icon("fileclose") );
    mnuFileHistory = new QMenu;
      mnuFileHistory->setIcon( BApplication::icon("history") );
    //
    checkActions();
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BOpenSaveEditorModulePrivate::checkActions()
{
    BCodeEditorDocument *doc = q_func()->currentDocument();
    if ( !actNewFile.isNull() )
        actNewFile->setEnabled(editor);
    if ( !actOpenFiles.isNull() )
        actOpenFiles->setEnabled(editor);
    if ( !actReopenFile.isNull() )
        actReopenFile->setEnabled( doc && editor->driver()->testFileExistance( doc->fileName() ) && !doc->isBuisy() );
    if ( !actSaveFile.isNull() )
        actSaveFile->setEnabled( doc && doc->isModified() && !doc->isReadOnly() && !doc->isBuisy() );
    if ( !actSaveFileAs.isNull() )
        actSaveFileAs->setEnabled( doc && !doc->isBuisy() );
    if ( !actSaveAllFiles.isNull() )
        actSaveAllFiles->setEnabled(editor && doc);
    if ( !actCloseFile.isNull() )
        actCloseFile->setEnabled( doc && !doc->isBuisy() );
    if ( !actCloseAllFiles.isNull() )
        actCloseAllFiles->setEnabled(editor && doc);
    if ( !mnuFileHistory.isNull() )
        mnuFileHistory->setEnabled( editor && !mnuFileHistory->isEmpty() );
}

void BOpenSaveEditorModulePrivate::resetFileHistory(const QStringList &list)
{
    if ( mnuFileHistory.isNull() )
        return;
    QList<QAction *> acts = mnuFileHistory->actions();
    while ( acts.size() > list.size() )
    {
        QAction *act = acts.takeLast();
        mnuFileHistory->removeAction(act);
        act->deleteLater();
    }
    while ( acts.size() < list.size() )
    {
        QAction *act = new QAction( mnuFileHistory.data() );
        acts << act;
        mnuFileHistory->addAction(act);
    }
    for (int i = 0; i < list.size(); ++i)
    {
        QAction *act = acts.at(i);
        act->setProperty( "beqt/file_name", list.at(i) );
        act->setText( QFileInfo( list.at(i) ).fileName() );
        connect( act, SIGNAL( triggered() ), this, SLOT( fileTriggered() ) );
    }
    mnuFileHistory->setEnabled( !mnuFileHistory->isEmpty() );
}

/*============================== Public slots ==============================*/

void BOpenSaveEditorModulePrivate::retranslateUi()
{
    if ( !actNewFile.isNull() )
    {
        actNewFile->setText( tr("Create", "act text") );
        actNewFile->setToolTip( tr("Create new document", "act toolTip") );
        actNewFile->setWhatsThis( tr("Use this action to create a new document. No file will be created on your drive",
                                     "act whatsThis") );
    }
    if ( !actOpenFiles.isNull() )
    {
        actOpenFiles->setText( tr("Open", "act text") );
        actOpenFiles->setToolTip( tr("Open existing files", "act toolTip") );
        actOpenFiles->setWhatsThis( tr("Use this action to open one or more files existing on your drive",
                                       "act whatsThis") );
    }
    if ( !actReopenFile.isNull() )
    {
        actReopenFile->setText( tr("Reopen", "act text") );
        actReopenFile->setToolTip( tr("Reopen current file", "act toolTip") );
        actReopenFile->setWhatsThis( tr("Use this file to reload current document. "
                                        "Use the down arrow to reopen the document using another encoding",
                                        "act whatsThis") );
        foreach (QMenu *mnu, codecGroups)
            mnu->setTitle( BCodeEditor::codecGroupName( static_cast<BCodeEditor::CodecGroup>(
                                                            mnu->property("beqt/codec_group").toInt() ) ) );
        foreach (QAction *act, codecs)
            act->setText( BCodeEditor::fullCodecName( act->property("beqt/codec_name").toString() ) );
    }
    if ( !actSaveFile.isNull() )
    {
        actSaveFile->setText( tr("Save", "act text") );
        actSaveFile->setToolTip( tr("Save current document", "act toolTip") );
        actSaveFile->setWhatsThis( tr("Use this action to save the current document on your drive. "
                                      "If the corresponding file doesn't exist, "
                                      "you will be proposed to select it's location", "act whatsThis") );
    }
    if ( !actSaveFileAs.isNull() )
    {
        actSaveFileAs->setText( tr("Save as...", "act text") );
        actSaveFileAs->setToolTip( tr("Save current document as...", "act toolTip") );
        actSaveFileAs->setWhatsThis( tr("Use this action to save the current document on your drive. "
                                        "You will be proposed to select file location and encoding", "act whatsThis") );
    }
    if ( !actSaveAllFiles.isNull() )
    {
        actSaveAllFiles->setText( tr("Save all", "act text") );
        actSaveAllFiles->setToolTip( tr("Save all opened documents", "act toolTip") );
        actSaveAllFiles->setWhatsThis( tr("Use this action to save all modified documents. "
                                          "This is the same as using the Save action at every modified document",
                                          "act whatsThis") );
    }
    if ( !actCloseFile.isNull() )
    {
        actCloseFile->setText( tr("Close", "act text") );
        actCloseFile->setToolTip( tr("Close current document", "act toolTip") );
        actCloseFile->setWhatsThis( tr("Use this action to close the current document. "
                                       "If the document is modified, you will be asked to save it before closing",
                                       "act whatsThis") );
    }
    if ( !actCloseAllFiles.isNull() )
    {
        actCloseAllFiles->setText( tr("Close all", "act text") );
        actCloseAllFiles->setToolTip( tr("Close all opened documents", "act toolTip") );
        actCloseAllFiles->setWhatsThis( tr("Use this action to close all opened documents. "
                                           "If any documents are modified, "
                                           "you will be asked to save them before closing", "act whatsThis") );
    }
    if ( !mnuFileHistory.isNull() )
    {
        mnuFileHistory->setTitle( tr("Recent files", "mnu title") );
        mnuFileHistory->setToolTip( tr("Recently opened files", "mnu toolTip") );
        mnuFileHistory->setWhatsThis( tr("Use this action to open one of the recently opened files",
                                         "mnu whatsThis") );
    }
}

void BOpenSaveEditorModulePrivate::codecTriggered()
{
    if (!editor)
        return;
    QString cn = sender()->property("beqt/codec_name").toString();
    if ( cn.isEmpty() )
        return;
    editor->reopenCurrentDocument( QTextCodec::codecForName( cn.toLatin1() ) );
}

void BOpenSaveEditorModulePrivate::fileTriggered()
{
    if (!editor)
        return;
    QString fn = sender()->property("beqt/file_name").toString();
    if ( fn.isEmpty() )
        return;
    editor->openDocument(fn);
}

/*============================================================================
================================ BOpenSaveEditorModule =======================
============================================================================*/

/*============================== Public constructors =======================*/

BOpenSaveEditorModule::BOpenSaveEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BOpenSaveEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BOpenSaveEditorModule::~BOpenSaveEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BOpenSaveEditorModule::BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QString BOpenSaveEditorModule::id() const
{
    return "beqt/open_save";
}

QAction *BOpenSaveEditorModule::action(int type)
{
    switch (type)
    {
    case NewFileAction:
        return d_func()->actNewFile.data();
    case OpenFilesAction:
        return d_func()->actOpenFiles.data();
    case ReopenAction:
        return d_func()->actReopenFile.data();
    case SaveFileAction:
        return d_func()->actSaveFile.data();
    case SaveFileAsAction:
        return d_func()->actSaveFileAs.data();
    case SaveAllFilesAction:
        return d_func()->actSaveAllFiles.data();
    case CloseFileAction:
        return d_func()->actCloseFile.data();
    case CloseAllFilesAction:
        return d_func()->actCloseAllFiles.data();
    default:
        return 0;
    }
}

QList<QAction *> BOpenSaveEditorModule::actions(int group, bool extended)
{
    B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    switch (group)
    {
    case OpenActionGroup:
        if ( !d->actNewFile.isNull() )
            list << d->actNewFile.data();
        if ( !d->actOpenFiles.isNull() )
            list << d->actOpenFiles.data();
        if ( extended && !d->actReopenFile.isNull() )
            list << d->actReopenFile.data();
        break;
    case SaveActionGroup:
        if ( !d->actSaveFile.isNull() )
            list << d->actSaveFile.data();
        if ( !d->actSaveFileAs.isNull() )
            list << d->actSaveFileAs.data();
        if ( extended && !d->actSaveAllFiles.isNull() )
            list << d->actSaveAllFiles.data();
        break;
    case CloseActionGroup:
        if ( !d->actCloseFile.isNull() )
            list << d->actCloseFile.data();
        if ( extended && !d->actCloseAllFiles.isNull() )
            list << d->actCloseAllFiles.data();
        break;
    default:
        break;
    }
    return list;
}

QList<QAction *> BOpenSaveEditorModule::actions(bool extended)
{
    QList<QAction *> list;
    list << actions(OpenActionGroup, extended);
    list << actions(SaveActionGroup, extended);
    list << actions(CloseActionGroup, extended);
    return list;
}

QMenu *BOpenSaveEditorModule::fileHistoryMenu()
{
    return d_func()->mnuFileHistory.data();
}

QList<QAction *> BOpenSaveEditorModule::fileHistoryActions()
{
    const B_D(BOpenSaveEditorModule);
    return !d->mnuFileHistory.isNull() ? d->mnuFileHistory->actions() : QList<QAction *>();
}

/*============================== Protected methods =========================*/

void BOpenSaveEditorModule::editorSet(BCodeEditor *edr)
{
    B_D(BOpenSaveEditorModule);
    if (edr)
    {
        if ( !d->actNewFile.isNull() )
            connect( d->actNewFile.data(), SIGNAL( triggered() ), edr, SLOT( addDocument() ) );
        if ( !d->actOpenFiles.isNull() )
            connect( d->actOpenFiles.data(), SIGNAL( triggered() ), edr, SLOT( openDocuments() ) );
        if ( !d->actReopenFile.isNull() )
            connect( d->actReopenFile.data(), SIGNAL( triggered() ), edr, SLOT( reopenCurrentDocument() ) );
        if ( !d->actSaveFile.isNull() )
            connect( d->actSaveFile.data(), SIGNAL( triggered() ), edr, SLOT( saveCurrentDocument() ) );
        if ( !d->actSaveFileAs.isNull() )
            connect( d->actSaveFileAs.data(), SIGNAL( triggered() ), edr, SLOT( saveCurrentDocumentAs() ) );
        if ( !d->actSaveAllFiles.isNull() )
            connect( d->actSaveAllFiles.data(), SIGNAL( triggered() ), edr, SLOT( saveAllDocuments() ) );
        if ( !d->actCloseFile.isNull() )
            connect( d->actCloseFile.data(), SIGNAL( triggered() ), edr, SLOT( closeCurrentDocument() ) );
        if ( !d->actCloseAllFiles.isNull() )
            connect( d->actCloseAllFiles.data(), SIGNAL( triggered() ), edr, SLOT( closeAllDocuments() ) );
    }
    d->checkActions();
    d->resetFileHistory( edr ? edr->fileHistory() : QStringList() );
}

void BOpenSaveEditorModule::editorUnset(BCodeEditor *edr)
{
    B_D(BOpenSaveEditorModule);
    if (edr)
    {
        if ( !d->actNewFile.isNull() )
            disconnect( d->actNewFile.data(), SIGNAL( triggered() ), edr, SLOT( addDocument() ) );
        if ( !d->actOpenFiles.isNull() )
            disconnect( d->actOpenFiles.data(), SIGNAL( triggered() ), edr, SLOT( openDocuments() ) );
        if ( !d->actReopenFile.isNull() )
            disconnect( d->actReopenFile.data(), SIGNAL( triggered() ), edr, SLOT( reopenCurrentDocument() ) );
        if ( !d->actSaveFile.isNull() )
            disconnect( d->actSaveFile.data(), SIGNAL( triggered() ), edr, SLOT( saveCurrentDocument() ) );
        if ( !d->actSaveFileAs.isNull() )
            disconnect( d->actSaveFileAs.data(), SIGNAL( triggered() ), edr, SLOT( saveCurrentDocumentAs() ) );
        if ( !d->actSaveAllFiles.isNull() )
            disconnect( d->actSaveAllFiles.data(), SIGNAL( triggered() ), edr, SLOT( saveAllDocuments() ) );
        if ( !d->actCloseFile.isNull() )
            disconnect( d->actCloseFile.data(), SIGNAL( triggered() ), edr, SLOT( closeCurrentDocument() ) );
        if ( !d->actCloseAllFiles.isNull() )
            disconnect( d->actCloseAllFiles.data(), SIGNAL( triggered() ), edr, SLOT( closeAllDocuments() ) );
    }
    d->checkActions();
    d->resetFileHistory();
}

void BOpenSaveEditorModule::documentReadOnlyChanged(bool ro)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::documentModificationChanged(bool modified)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::documentBuisyChanged(bool buisy)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::fileHistoryChanged(const QStringList &list)
{
    d_func()->resetFileHistory(list);
}
