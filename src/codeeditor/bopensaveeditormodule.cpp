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

#include <QDebug>

/*============================================================================
================================ Open Save Editor Module Private
============================================================================*/

BOpenSaveEditorModulePrivate::BOpenSaveEditorModulePrivate(BOpenSaveEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BOpenSaveEditorModulePrivate::~BOpenSaveEditorModulePrivate()
{
    //
}

//

void BOpenSaveEditorModulePrivate::init()
{
    actNewFile = new QAction(this);
      actNewFile->setIcon( BApplication::icon("filenew") );
    actOpenFiles = new QAction(this);
      actOpenFiles->setIcon( BApplication::icon("fileopen") );
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
    actSaveFileAs = new QAction(this);
      actSaveFileAs->setIcon( BApplication::icon("filesaveas") );
    actSaveAllFiles = new QAction(this);
      actSaveAllFiles->setIcon( BApplication::icon("save_all") );
    actCloseFile = new QAction(this);
      actCloseFile->setIcon( BApplication::icon("fileclose") );
    actCloseAllFiles = new QAction(this);
      actCloseAllFiles->setIcon( BApplication::icon("fileclose") );
    //
    checkActions();
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BOpenSaveEditorModulePrivate::checkActions()
{
    BCodeEditorDocument *doc = editor ? editor->currentDocument() : 0;
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
}

//

void BOpenSaveEditorModulePrivate::retranslateUi()
{
    if ( !actNewFile.isNull() )
    {
        actNewFile->setText( trq("New file", "act text") );
        actNewFile->setToolTip( trq("New file", "act toolTip") );
        actNewFile->setWhatsThis( trq("New file", "act whatsThis") );
    }
    if ( !actOpenFiles.isNull() )
    {
        actOpenFiles->setText( trq("Open files", "act text") );
        actOpenFiles->setToolTip( trq("Open files", "act toolTip") );
        actOpenFiles->setWhatsThis( trq("Open files", "act whatsThis") );
    }
    if ( !actReopenFile.isNull() )
    {
        actReopenFile->setText( trq("Reopen file", "act text") );
        actReopenFile->setToolTip( trq("Reopen file", "act toolTip") );
        actReopenFile->setWhatsThis( trq("Reopen file", "act whatsThis") );
        foreach (QMenu *mnu, codecGroups)
            mnu->setTitle( BCodeEditor::codecGroupName( static_cast<BCodeEditor::CodecGroup>(
                                                            mnu->property("beqt/codec_group").toInt() ) ) );
        foreach (QAction *act, codecs)
            act->setText( BCodeEditor::fullCodecName( act->property("beqt/codec_name").toString() ) );
    }
    if ( !actSaveFile.isNull() )
    {
        actSaveFile->setText( trq("Save file", "act text") );
        actSaveFile->setToolTip( trq("Save file", "act toolTip") );
        actSaveFile->setWhatsThis( trq("Save file", "act whatsThis") );
    }
    if ( !actSaveFileAs.isNull() )
    {
        actSaveFileAs->setText( trq("Save file as", "act text") );
        actSaveFileAs->setToolTip( trq("Save file as", "act toolTip") );
        actSaveFileAs->setWhatsThis( trq("Save file as", "act whatsThis") );
    }
    if ( !actSaveAllFiles.isNull() )
    {
        actSaveAllFiles->setText( trq("Save all files", "act text") );
        actSaveAllFiles->setToolTip( trq("Save all files", "act toolTip") );
        actSaveAllFiles->setWhatsThis( trq("Save all files", "act whatsThis") );
    }
    if ( !actCloseFile.isNull() )
    {
        actCloseFile->setText( trq("Close file", "act text") );
        actCloseFile->setToolTip( trq("Close file", "act toolTip") );
        actCloseFile->setWhatsThis( trq("Close file", "act whatsThis") );
    }
    if ( !actCloseAllFiles.isNull() )
    {
        actCloseAllFiles->setText( trq("Close all files", "act text") );
        actCloseAllFiles->setToolTip( trq("Close all files", "act toolTip") );
        actCloseAllFiles->setWhatsThis( trq("Close all files", "act whatsThis") );
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

/*============================================================================
================================ Open Save Editor Module
============================================================================*/

BOpenSaveEditorModule::BOpenSaveEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BOpenSaveEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BOpenSaveEditorModule::~BOpenSaveEditorModule()
{
    //
}

//

BOpenSaveEditorModule::BOpenSaveEditorModule(BOpenSaveEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

//

QString BOpenSaveEditorModule::id() const
{
    return "beqt/open_save";
}

QAction *BOpenSaveEditorModule::action(Action type) const
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

QList<QAction *> BOpenSaveEditorModule::openActions(bool extended) const
{
    const B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    if ( !d->actNewFile.isNull() )
        list << d->actNewFile.data();
    if ( !d->actOpenFiles.isNull() )
        list << d->actOpenFiles.data();
    if ( extended && !d->actReopenFile.isNull() )
        list << d->actReopenFile.data();
    return list;
}

QList<QAction *> BOpenSaveEditorModule::saveActions(bool extended) const
{
    const B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    if ( !d->actSaveFile.isNull() )
        list << d->actSaveFile.data();
    if ( !d->actSaveFileAs.isNull() )
        list << d->actSaveFileAs.data();
    if ( extended && !d->actSaveAllFiles.isNull() )
        list << d->actSaveAllFiles.data();
    return list;
}

QList<QAction *> BOpenSaveEditorModule::closeActions(bool extended) const
{
    const B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    if ( !d->actCloseFile.isNull() )
        list << d->actCloseFile.data();
    if ( extended && !d->actCloseAllFiles.isNull() )
        list << d->actCloseAllFiles.data();
    return list;
}

QList<QAction *> BOpenSaveEditorModule::actions(bool extended) const
{
    QList<QAction *> list;
    list << openActions(extended);
    list << saveActions(extended);
    list << closeActions(extended);
    return list;
}

//

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
