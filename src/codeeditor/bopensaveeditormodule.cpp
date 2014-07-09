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
    if (!mnuFileHistory->parent())
        delete mnuFileHistory;
}

/*============================== Public methods ============================*/

void BOpenSaveEditorModulePrivate::checkActions()
{
    BAbstractCodeEditorDocument *doc = q_func()->currentDocument();
    actNewFile->setEnabled(editor);
    actOpenFiles->setEnabled(editor);
    actReopenFile->setEnabled(doc && editor->driver()->testFileExistance(doc->fileName()) && !doc->isBuisy());
    actSaveFile->setEnabled(doc && doc->isModified() && !doc->isReadOnly() && !doc->isBuisy());
    actSaveFileAs->setEnabled(doc && !doc->isBuisy());
    actSaveAllFiles->setEnabled(editor && doc);
    actCloseFile->setEnabled(doc && !doc->isBuisy());
    actCloseAllFiles->setEnabled(editor && doc);
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
    QObject *s = sender();
    mnuFileHistory->setToolTip(s ? s->property("beqt/file_name").toString() : QString());
}

void BOpenSaveEditorModulePrivate::retranslateUi()
{
    actNewFile->setText(tr("Create", "act text"));
    actNewFile->setToolTip(tr("Create new document", "act toolTip"));
    actNewFile->setWhatsThis(tr("Use this action to create a new document. No file will be created on your drive",
                                "act whatsThis"));
    actOpenFiles->setText(tr("Open...", "act text"));
    actOpenFiles->setToolTip(tr("Open existing files", "act toolTip"));
    actOpenFiles->setWhatsThis(tr("Use this action to open one or more files existing on your drive",
                                  "act whatsThis"));
    actReopenFile->setText(tr("Reopen", "act text"));
    actReopenFile->setToolTip(tr("Reopen current file", "act toolTip"));
    actReopenFile->setWhatsThis(tr("Use this file to reload current document. "
                                   "Use the down arrow to reopen the document using another encoding",
                                   "act whatsThis"));
    actSaveFile->setText(tr("Save", "act text"));
    actSaveFile->setToolTip(tr("Save current document", "act toolTip"));
    actSaveFile->setWhatsThis(tr("Use this action to save the current document on your drive. "
                                 "If the corresponding file doesn't exist, "
                                 "you will be proposed to select it's location", "act whatsThis"));
    actSaveFileAs->setText(tr("Save as...", "act text"));
    actSaveFileAs->setToolTip(tr("Save current document as...", "act toolTip"));
    actSaveFileAs->setWhatsThis(tr("Use this action to save the current document on your drive. "
                                   "You will be proposed to select file location and encoding", "act whatsThis"));
    actSaveAllFiles->setText(tr("Save all", "act text"));
    actSaveAllFiles->setToolTip(tr("Save all opened documents", "act toolTip"));
    actSaveAllFiles->setWhatsThis(tr("Use this action to save all modified documents. "
                                     "This is the same as using the Save action at every modified document",
                                     "act whatsThis"));
    actCloseFile->setText(tr("Close", "act text"));
    actCloseFile->setToolTip(tr("Close current document", "act toolTip"));
    actCloseFile->setWhatsThis(tr("Use this action to close the current document. "
                                  "If the document is modified, you will be asked to save it before closing",
                                  "act whatsThis"));
    actCloseAllFiles->setText(tr("Close all", "act text"));
    actCloseAllFiles->setToolTip(tr("Close all opened documents", "act toolTip"));
    actCloseAllFiles->setWhatsThis(tr("Use this action to close all opened documents. "
                                      "If any documents are modified, "
                                      "you will be asked to save them before closing", "act whatsThis"));
    mnuFileHistory->setTitle(tr("Recent files", "mnu title"));
    mnuFileHistory->setWhatsThis(tr("Use this action to open one of the recently opened files", "mnu whatsThis"));
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
        list << d->actNewFile;
        list << d->actOpenFiles;
        list << d->actReopenFile;
        break;
    case SaveActionGroup:
        list << d->actSaveFile;
        list << d->actSaveFileAs;
        list << d->actSaveAllFiles;
        break;
    case CloseActionGroup:
        list << d->actCloseFile;
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

QList<QAction *> BOpenSaveEditorModule::fileHistoryActions()
{
    return d_func()->mnuFileHistory->actions();
}

QMenu *BOpenSaveEditorModule::fileHistoryMenu()
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
        connect(d->actNewFile, SIGNAL(triggered()), edr, SLOT(addDocument()));
        connect(d->actOpenFiles, SIGNAL(triggered()), edr, SLOT(openDocuments()));
        connect(d->actReopenFile, SIGNAL(triggered()), edr, SLOT(reopenCurrentDocument()));
        connect(d->actSaveFile, SIGNAL(triggered()), edr, SLOT(saveCurrentDocument()));
        connect(d->actSaveFileAs, SIGNAL(triggered()), edr, SLOT(saveCurrentDocumentAs()));
        connect(d->actSaveAllFiles, SIGNAL(triggered()), edr, SLOT(saveAllDocuments()));
        connect(d->actCloseFile, SIGNAL(triggered()), edr, SLOT(closeCurrentDocument()));
        connect(d->actCloseAllFiles, SIGNAL(triggered()), edr, SLOT(closeAllDocuments()));
    }
    d->checkActions();
    d->resetFileHistory(edr ? edr->fileHistory() : QStringList());
}

void BOpenSaveEditorModule::editorUnset(BCodeEditor *edr)
{
    B_D(BOpenSaveEditorModule);
    if (edr) {
        disconnect(d->actNewFile, SIGNAL(triggered()), edr, SLOT(addDocument()));
        disconnect(d->actOpenFiles, SIGNAL(triggered()), edr, SLOT(openDocuments()));
        disconnect(d->actReopenFile, SIGNAL(triggered()), edr, SLOT(reopenCurrentDocument()));
        disconnect(d->actSaveFile, SIGNAL(triggered()), edr, SLOT(saveCurrentDocument()));
        disconnect(d->actSaveFileAs, SIGNAL(triggered()), edr, SLOT(saveCurrentDocumentAs()));
        disconnect(d->actSaveAllFiles, SIGNAL(triggered()), edr, SLOT(saveAllDocuments()));
        disconnect(d->actCloseFile, SIGNAL(triggered()), edr, SLOT(closeCurrentDocument()));
        disconnect(d->actCloseAllFiles, SIGNAL(triggered()), edr, SLOT(closeAllDocuments()));
    }
    d->checkActions();
    d->resetFileHistory();
}

void BOpenSaveEditorModule::fileHistoryChanged(const QStringList &list)
{
    d_func()->resetFileHistory(list);
}
