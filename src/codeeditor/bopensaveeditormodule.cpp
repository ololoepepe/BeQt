/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtCodeEditor module of the BeQt library.
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

#include "bopensaveeditormodule.h"
#include "bopensaveeditormodule_p.h"

#include "babstractcodeeditordocument.h"
#include "babstractdocumentdriver.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>
#include <BeQtWidgets/BTextCodecMenu>

#include <QAction>
#include <QDebug>
#include <QFileInfo>
#include <QKeySequence>
#include <QList>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QVariant>

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
    if (!mnuFileHistory.isNull() && !mnuFileHistory->parent())
        delete mnuFileHistory;
}

/*============================== Public methods ============================*/

void BOpenSaveEditorModulePrivate::checkActions()
{
    BAbstractCodeEditorDocument *doc = q_func()->currentDocument();
    if (!actNewFile.isNull())
        actNewFile->setEnabled(editor);
    if (!actOpenFiles.isNull())
        actOpenFiles->setEnabled(editor);
    if (!actReopenFile.isNull())
        actReopenFile->setEnabled(doc && editor->driver()->testFileExistance(doc->fileName()) && !doc->isBuisy());
    if (!actSaveFile.isNull())
        actSaveFile->setEnabled(doc && doc->isModified() && !doc->isReadOnly() && !doc->isBuisy());
    if (!actSaveFileAs.isNull())
        actSaveFileAs->setEnabled(doc && !doc->isBuisy());
    if (!actSaveAllFiles.isNull())
        actSaveAllFiles->setEnabled(editor && doc);
    if (!actCloseFile.isNull())
        actCloseFile->setEnabled(doc && !doc->isBuisy());
    if (!actCloseAllFiles.isNull())
        actCloseAllFiles->setEnabled(editor && doc);
    if (!mnuFileHistory.isNull())
        mnuFileHistory->setEnabled(editor && !mnuFileHistory->isEmpty());
}

void BOpenSaveEditorModulePrivate::init()
{
    actNewFile = new QAction(this);
      actNewFile->setIcon(BApplication::icon("filenew"));
      actNewFile->setShortcut(QKeySequence::New);
    actOpenFiles = new QAction(this);
      actOpenFiles->setIcon(BApplication::icon("fileopen"));
      actOpenFiles->setShortcut(QKeySequence::Open);
    actReopenFile = new QAction(this);
      actReopenFile->setIcon(BApplication::icon("reload"));
      BTextCodecMenu *mnu = new BTextCodecMenu(BTextCodecMenu::StructuredStyle);
      mnu->setMapping(this, SLOT(codecTriggered(QString)));
      actReopenFile->setMenu(mnu);
    actSaveFile = new QAction(this);
      actSaveFile->setIcon(BApplication::icon("filesave"));
      actSaveFile->setShortcut(QKeySequence::Save);
    actSaveFileAs = new QAction(this);
      actSaveFileAs->setIcon(BApplication::icon("filesaveas"));
    actSaveAllFiles = new QAction(this);
      actSaveAllFiles->setIcon(BApplication::icon("save_all"));
      actSaveAllFiles->setShortcut(QKeySequence("Ctrl+Shift+S"));
    actCloseFile = new QAction(this);
      actCloseFile->setIcon(BApplication::icon("fileclose"));
      actCloseFile->setShortcut(QKeySequence::Close);
    actCloseAllFiles = new QAction(this);
      actCloseAllFiles->setIcon(BApplication::icon("fileclose"));
    mnuFileHistory = new QMenu;
      mnuFileHistory->setIcon(BApplication::icon("history"));
    //
    checkActions();
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BOpenSaveEditorModulePrivate::resetFileHistory(const QStringList &list)
{
    if (mnuFileHistory.isNull())
        return;
    QList<QAction *> acts = mnuFileHistory->actions();
    while (acts.size() > list.size()) {
        QAction *act = acts.takeLast();
        mnuFileHistory->removeAction(act);
        act->deleteLater();
    }
    while (acts.size() < list.size()) {
        QAction *act = new QAction(mnuFileHistory);
        acts << act;
        mnuFileHistory->addAction(act);
    }
    for (int i = 0; i < list.size(); ++i) {
        QAction *act = acts.at(i);
        act->setProperty("beqt/file_name", list.at(i));
        act->setText(QFileInfo( list.at(i) ).fileName());
        connect(act, SIGNAL(triggered()), this, SLOT(fileTriggered()), Qt::UniqueConnection);
        connect(act, SIGNAL(hovered()), this, SLOT(resetFileHistoryMenuToolTip()));
    }
    mnuFileHistory->setEnabled(!mnuFileHistory->isEmpty());
}

/*============================== Public slots ==============================*/

void BOpenSaveEditorModulePrivate::codecTriggered(const QString &codecName)
{
    if (!editor)
        return;
    editor->reopenCurrentDocument(codecName);
}

void BOpenSaveEditorModulePrivate::fileTriggered()
{
    if (!editor)
        return;
    QString fn = sender()->property("beqt/file_name").toString();
    if (fn.isEmpty())
        return;
    editor->openDocument(fn);
}

void BOpenSaveEditorModulePrivate::resetFileHistoryMenuToolTip()
{
    if (mnuFileHistory.isNull())
        return;
    QObject *s = sender();
    mnuFileHistory->setToolTip(s ? s->property("beqt/file_name").toString() : QString());
}

void BOpenSaveEditorModulePrivate::retranslateUi()
{
    if (!actNewFile.isNull()) {
        actNewFile->setText(tr("Create", "act text"));
        actNewFile->setToolTip(tr("Create new document", "act toolTip"));
        actNewFile->setWhatsThis(tr("Use this action to create a new document. No file will be created on your drive",
                                    "act whatsThis"));
    }
    if (!actOpenFiles.isNull()) {
        actOpenFiles->setText(tr("Open...", "act text"));
        actOpenFiles->setToolTip(tr("Open existing files", "act toolTip"));
        actOpenFiles->setWhatsThis(tr("Use this action to open one or more files existing on your drive",
                                      "act whatsThis"));
    }
    if (!actReopenFile.isNull()) {
        actReopenFile->setText(tr("Reopen", "act text"));
        actReopenFile->setToolTip(tr("Reopen current file", "act toolTip"));
        actReopenFile->setWhatsThis(tr("Use this file to reload current document. "
                                       "Use the down arrow to reopen the document using another encoding",
                                       "act whatsThis"));
    }
    if (!actSaveFile.isNull()) {
        actSaveFile->setText(tr("Save", "act text"));
        actSaveFile->setToolTip(tr("Save current document", "act toolTip"));
        actSaveFile->setWhatsThis(tr("Use this action to save the current document on your drive. "
                                     "If the corresponding file doesn't exist, "
                                     "you will be proposed to select it's location", "act whatsThis"));
    }
    if (!actSaveFileAs.isNull()) {
        actSaveFileAs->setText(tr("Save as...", "act text"));
        actSaveFileAs->setToolTip(tr("Save current document as...", "act toolTip"));
        actSaveFileAs->setWhatsThis(tr("Use this action to save the current document on your drive. "
                                       "You will be proposed to select file location and encoding", "act whatsThis"));
    }
    if (!actSaveAllFiles.isNull()) {
        actSaveAllFiles->setText(tr("Save all", "act text"));
        actSaveAllFiles->setToolTip(tr("Save all opened documents", "act toolTip"));
        actSaveAllFiles->setWhatsThis(tr("Use this action to save all modified documents. "
                                         "This is the same as using the Save action at every modified document",
                                         "act whatsThis"));
    }
    if (!actCloseFile.isNull()) {
        actCloseFile->setText(tr("Close", "act text"));
        actCloseFile->setToolTip(tr("Close current document", "act toolTip"));
        actCloseFile->setWhatsThis(tr("Use this action to close the current document. "
                                      "If the document is modified, you will be asked to save it before closing",
                                      "act whatsThis"));
    }
    if (!actCloseAllFiles.isNull()) {
        actCloseAllFiles->setText(tr("Close all", "act text"));
        actCloseAllFiles->setToolTip(tr("Close all opened documents", "act toolTip"));
        actCloseAllFiles->setWhatsThis(tr("Use this action to close all opened documents. "
                                          "If any documents are modified, "
                                          "you will be asked to save them before closing", "act whatsThis"));
    }
    if (!mnuFileHistory.isNull()) {
        mnuFileHistory->setTitle(tr("Recent files", "mnu title"));
        mnuFileHistory->setWhatsThis(tr("Use this action to open one of the recently opened files", "mnu whatsThis"));
    }
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

QAction *BOpenSaveEditorModule::action(int type)
{
    switch (type) {
    case NewFileAction:
        return d_func()->actNewFile;
    case OpenFilesAction:
        return d_func()->actOpenFiles;
    case ReopenAction:
        return d_func()->actReopenFile;
    case SaveFileAction:
        return d_func()->actSaveFile;
    case SaveFileAsAction:
        return d_func()->actSaveFileAs;
    case SaveAllFilesAction:
        return d_func()->actSaveAllFiles;
    case CloseFileAction:
        return d_func()->actCloseFile;
    case CloseAllFilesAction:
        return d_func()->actCloseAllFiles;
    default:
        return 0;
    }
}

QList<QAction *> BOpenSaveEditorModule::actions(int group, bool)
{
    B_D(BOpenSaveEditorModule);
    QList<QAction *> list;
    switch (group) {
    case OpenActionGroup:
        if (!d->actNewFile.isNull())
            list << d->actNewFile;
        if (!d->actOpenFiles.isNull())
            list << d->actOpenFiles;
        if (!d->actReopenFile.isNull())
            list << d->actReopenFile;
        break;
    case SaveActionGroup:
        if (!d->actSaveFile.isNull())
            list << d->actSaveFile;
        if (!d->actSaveFileAs.isNull())
            list << d->actSaveFileAs;
        if (!d->actSaveAllFiles.isNull())
            list << d->actSaveAllFiles;
        break;
    case CloseActionGroup:
        if (!d->actCloseFile.isNull())
            list << d->actCloseFile;
        if (!d->actCloseAllFiles.isNull())
            list << d->actCloseAllFiles;
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

QList<QAction *> BOpenSaveEditorModule::fileHistoryActions() const
{
    return !d_func()->mnuFileHistory.isNull() ? d_func()->mnuFileHistory->actions() : QList<QAction *>();
}

QMenu *BOpenSaveEditorModule::fileHistoryMenu() const
{
    return d_func()->mnuFileHistory;
}

QString BOpenSaveEditorModule::id() const
{
    return "beqt/open_save";
}

/*============================== Protected methods =========================*/

void BOpenSaveEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::documentBuisyChanged(bool)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::documentModificationChanged(bool)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::documentReadOnlyChanged(bool)
{
    d_func()->checkActions();
}

void BOpenSaveEditorModule::editorSet(BCodeEditor *edr)
{
    B_D(BOpenSaveEditorModule);
    if (edr) {
        if (!d->actNewFile.isNull())
            connect(d->actNewFile, SIGNAL(triggered()), edr, SLOT(addDocument()));
        if (!d->actOpenFiles.isNull())
            connect(d->actOpenFiles, SIGNAL(triggered()), edr, SLOT(openDocuments()));
        if (!d->actReopenFile.isNull())
            connect(d->actReopenFile, SIGNAL(triggered()), edr, SLOT(reopenCurrentDocument()));
        if (!d->actSaveFile.isNull())
            connect(d->actSaveFile, SIGNAL(triggered()), edr, SLOT(saveCurrentDocument()));
        if (!d->actSaveFileAs.isNull())
            connect(d->actSaveFileAs, SIGNAL(triggered()), edr, SLOT(saveCurrentDocumentAs()));
        if (!d->actSaveAllFiles.isNull())
            connect(d->actSaveAllFiles, SIGNAL(triggered()), edr, SLOT(saveAllDocuments()));
        if (!d->actCloseFile.isNull())
            connect(d->actCloseFile, SIGNAL(triggered()), edr, SLOT(closeCurrentDocument()));
        if (!d->actCloseAllFiles.isNull())
            connect(d->actCloseAllFiles, SIGNAL(triggered()), edr, SLOT(closeAllDocuments()));
    }
    d->checkActions();
    d->resetFileHistory(edr ? edr->fileHistory() : QStringList());
}

void BOpenSaveEditorModule::editorUnset(BCodeEditor *edr)
{
    B_D(BOpenSaveEditorModule);
    if (edr) {
        if (!d->actNewFile.isNull())
            disconnect(d->actNewFile, SIGNAL(triggered()), edr, SLOT(addDocument()));
        if (!d->actOpenFiles.isNull())
            disconnect(d->actOpenFiles, SIGNAL(triggered()), edr, SLOT(openDocuments()));
        if (!d->actReopenFile.isNull())
            disconnect(d->actReopenFile, SIGNAL(triggered()), edr, SLOT(reopenCurrentDocument()));
        if (!d->actSaveFile.isNull())
            disconnect(d->actSaveFile, SIGNAL(triggered()), edr, SLOT(saveCurrentDocument()));
        if (!d->actSaveFileAs.isNull())
            disconnect(d->actSaveFileAs, SIGNAL(triggered()), edr, SLOT(saveCurrentDocumentAs()));
        if (!d->actSaveAllFiles.isNull())
            disconnect(d->actSaveAllFiles, SIGNAL(triggered()), edr, SLOT(saveAllDocuments()));
        if (!d->actCloseFile.isNull())
            disconnect(d->actCloseFile, SIGNAL(triggered()), edr, SLOT(closeCurrentDocument()));
        if (!d->actCloseAllFiles.isNull())
            disconnect(d->actCloseAllFiles, SIGNAL(triggered()), edr, SLOT(closeAllDocuments()));
    }
    d->checkActions();
    d->resetFileHistory();
}

void BOpenSaveEditorModule::fileHistoryChanged(const QStringList &list)
{
    d_func()->resetFileHistory(list);
}
