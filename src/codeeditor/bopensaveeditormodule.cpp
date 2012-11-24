#include "bopensaveeditormodule.h"
#include "bopensaveeditormodule_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"
#include "babstractfiletype.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QString>
#include <QList>
#include <QAction>

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
    B_Q(BOpenSaveEditorModule);
    actNewFile = new QAction(q);
      actNewFile->setIcon( BApplication::icon("filenew") );
    actOpenFiles = new QAction(q);
      actOpenFiles->setIcon( BApplication::icon("fileopen") );
    actSaveFile = new QAction(q);
      actSaveFile->setIcon( BApplication::icon("filesave") );
    actSaveFileAs = new QAction(q);
      actSaveFileAs->setIcon( BApplication::icon("filesaveas") );
    actSaveAllFiles = new QAction(q);
      actSaveAllFiles->setIcon( BApplication::icon("save_all") );
    actCloseFile = new QAction(q);
      actCloseFile->setIcon( BApplication::icon("fileclose") );
    actCloseAllFiles = new QAction(q);
      actCloseAllFiles->setIcon( BApplication::icon("fileclose") );
    checkActions();
    //
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

QList<QAction *> BOpenSaveEditorModule::openActions() const
{
    const B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    if ( !d->actNewFile.isNull() )
        list << d->actNewFile.data();
    if ( !d->actOpenFiles.isNull() )
        list << d->actOpenFiles.data();
    return list;
}

QList<QAction *> BOpenSaveEditorModule::saveActions() const
{
    const B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    if ( !d->actSaveFile.isNull() )
        list << d->actSaveFile.data();
    if ( !d->actSaveFileAs.isNull() )
        list << d->actSaveFileAs.data();
    if ( !d->actSaveAllFiles.isNull() )
        list << d->actSaveAllFiles.data();
    return list;
}

QList<QAction *> BOpenSaveEditorModule::closeActions() const
{
    const B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    if ( !d->actCloseFile.isNull() )
        list << d->actCloseFile.data();
    if ( !d->actCloseAllFiles.isNull() )
        list << d->actCloseAllFiles.data();
    return list;
}

QList<QAction *> BOpenSaveEditorModule::actions() const
{
    QList<QAction *> list;
    list << openActions();
    list << saveActions();
    list << closeActions();
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