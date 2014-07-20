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

class BSpellChecker;

#include "bcodeeditor.h"
#include "bcodeeditor_p.h"

#include "babstractcodeeditordocument.h"
#include "babstractdocumentdriver_p.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "babstractfiletype.h"
#include "bbookmarkseditormodule.h"
#include "bcodeeditordocument.h"
#include "bediteditormodule.h"
#include "bindicatorseditormodule.h"
#include "blocaldocumentdriver.h"
#include "bopensaveeditormodule.h"
#include "bsearcheditormodule.h"
#include "bsimplecodeeditordocument.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtWidgets/BApplication>
#include <BeQtWidgets/BGuiTools>

#include <QAbstractButton>
#include <QAction>
#include <QByteArray>
#include <QCheckBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileInfo>
#include <QFont>
#include <QFontInfo>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMap>
#include <QMessageBox>
#include <QMetaObject>
#include <QMimeData>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QTabWidget>
#include <QTextCodec>
#include <QTimer>
#include <QUrl>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ SelectDocumentsDialogPrivate ================
============================================================================*/

/*============================== Public constructors =======================*/

BSelectDocumentsDialogPrivate::BSelectDocumentsDialogPrivate(BSelectDocumentsDialog *q,
                                                             const QList<BAbstractCodeEditorDocument *> &list) :
    BBaseObjectPrivate(q), Documents(list)
{
    //
}

BSelectDocumentsDialogPrivate::~BSelectDocumentsDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSelectDocumentsDialogPrivate::init()
{
    B_Q(BSelectDocumentsDialog);
    decision = QMessageBox::Cancel;
    //
    QVBoxLayout *vlt = new QVBoxLayout(q);
      lblText = new QLabel(q);
        QFont fnt;
        fnt.setBold(true);
        lblText->setWordWrap(true);
        lblText->setFont(fnt);
        lblText->setText(tr("Some documents are modified. Do you want to save them before closing?", "lbl text"));
      vlt->addWidget(lblText);
      lblInformativeTextUpper = new QLabel(q);
        lblInformativeTextUpper->setWordWrap(true);
        lblInformativeTextUpper->setText(tr("Choose documents that you want to save:", "lbl text"));
      vlt->addWidget(lblInformativeTextUpper);
      lstwgt = new QListWidget(q);
        foreach (BAbstractCodeEditorDocument *doc, Documents) {
            QString fn = doc->fileName();
            QListWidgetItem *lwi = new QListWidgetItem(QFileInfo(fn).fileName());
            lwi->setData(Qt::ToolTipRole, fn);
            lwi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            lwi->setCheckState(Qt::Checked);
            lstwgt->addItem(lwi);
        }
      vlt->addWidget(lstwgt);
      lblInformativeTextLower = new QLabel(q);
        lblInformativeTextLower->setWordWrap(true);
        lblInformativeTextLower->setText(tr("If you don't save the documents, all changes will be lost", "lbl text"));
      vlt->addWidget(lblInformativeTextLower);
      dlgbbox = new QDialogButtonBox(q);
        dlgbbox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Discard | QDialogButtonBox::Cancel);
        dlgbbox->button(QDialogButtonBox::Discard)->setDefault(true);
        connect(dlgbbox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(dlgbboxClicked(QAbstractButton *)));
      vlt->addWidget(dlgbbox);
}

/*============================== Public slots ==============================*/

void BSelectDocumentsDialogPrivate::dlgbboxClicked(QAbstractButton *button)
{
    if (button == (QAbstractButton *) dlgbbox->button(QDialogButtonBox::Save))
    {
        decision = QMessageBox::Save;
        q_func()->accept();
    } else if (button == (QAbstractButton *) dlgbbox->button(QDialogButtonBox::Discard)) {
        decision = QMessageBox::Discard;
        q_func()->accept();
    } else if (button == (QAbstractButton *) dlgbbox->button(QDialogButtonBox::Cancel)) {
        decision = QMessageBox::Cancel;
        q_func()->reject();
    }
}

/*============================================================================
================================ BSelectDocumentsDialog ======================
============================================================================*/

/*============================== Public constructors =======================*/

BSelectDocumentsDialog::BSelectDocumentsDialog(const QList<BAbstractCodeEditorDocument *> &documents,
                                               QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSelectDocumentsDialogPrivate(this, documents))
{
    d_func()->init();
}

BSelectDocumentsDialog::~BSelectDocumentsDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSelectDocumentsDialog::BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBaseObject(d)
{
    //
}

/*============================== Public methods ============================*/

int BSelectDocumentsDialog::decision() const
{
    return d_func()->decision;
}

QList<BAbstractCodeEditorDocument *> BSelectDocumentsDialog::selectedDocuments() const
{
    const B_D(BSelectDocumentsDialog);
    QList<BAbstractCodeEditorDocument *> list;
    for (int i = 0; i < d->lstwgt->count(); ++i) {
        if (d->lstwgt->item(i)->checkState() == Qt::Checked)
            list << d->Documents.at(i);
    }
    return list;
}

/*============================================================================
================================ BSplittedLinesDialogPrivate =================
============================================================================*/

/*============================== Public constructors =======================*/

BSplittedLinesDialogPrivate::BSplittedLinesDialogPrivate(
        BSplittedLinesDialog *q, const QList<BCodeEdit::SplittedLinesRange> &ranges, int lineLength) :
    BBaseObjectPrivate(q), LineLength(lineLength), Ranges(ranges)
{
    //
}

BSplittedLinesDialogPrivate::~BSplittedLinesDialogPrivate()
{
    //
}

/*============================== Static public methods =====================*/

QListWidgetItem *BSplittedLinesDialogPrivate::createListWidgetItem(const BCodeEdit::SplittedLinesRange &range,
                                                                   int lineLength)
{
    QListWidgetItem *lwi = new QListWidgetItem;
    int lb = range.firstLineNumber;
    int ub = range.lastLineNumber;
    lwi->setData(Qt::UserRole, QPoint(0, lb));
    lwi->setData(Qt::UserRole + 1, QPoint(lineLength, ub));
    QString text = tr("Lines:", "lstwgti text") + " " + QString::number(lb + 1) + " - " + QString::number(ub + 1);
    text += " (" + tr("total:", "lstwgti text") + QString::number(ub - lb + 1) + ")";
    lwi->setText(text);
    return lwi;
}

/*============================== Public methods ============================*/

void BSplittedLinesDialogPrivate::init()
{
    B_Q(BSplittedLinesDialog);
    q->setWindowTitle(tr("Splitted lines", "windowTitle"));
    q->resize(400, 300);
    QVBoxLayout *vlt = new QVBoxLayout(q);
      lbl = new QLabel(q);
        lbl->setWordWrap(true);
        lbl->setText(tr("Some lines in this document were too long, "
                        "so they were splitted into several lines", "lbl text"));
      vlt->addWidget(lbl);
      lstwgt = new QListWidget(q);
        foreach (const BCodeEdit::SplittedLinesRange &range, Ranges)
            lstwgt->insertItem(0, createListWidgetItem(range, LineLength));
        lstwgt->setToolTip(tr("Doubleclick an item to go to the corresponding line", "lstwgt toolTip"));
        connect(lstwgt, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
                this, SLOT(lstwgtItemDoubleClicked(QListWidgetItem *)));
      vlt->addWidget(lstwgt);
      cboxSelect = new QCheckBox(q);
        cboxSelect->setChecked(true);
        cboxSelect->setText(tr("Select text when going to line", "cbox text"));
      vlt->addWidget(cboxSelect);
      dlgbbox = new QDialogButtonBox(q);
        dlgbbox->setStandardButtons(QDialogButtonBox::Close);
        connect(dlgbbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), q, SLOT(close()));
        btnGoto = new QPushButton;
          btnGoto->setDefault(true);
          btnGoto->setText(tr("Go to line", "btn text"));
          connect(btnGoto, SIGNAL(clicked()), this, SLOT(btnGotoClicked()));
        dlgbbox->addButton(btnGoto, QDialogButtonBox::ActionRole);
      vlt->addWidget(dlgbbox);
}

/*============================== Public slots ==============================*/

void BSplittedLinesDialogPrivate::btnGotoClicked()
{
    lstwgtItemDoubleClicked(lstwgt->currentItem());
}

void BSplittedLinesDialogPrivate::lstwgtItemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    if (cboxSelect->isChecked())
        QMetaObject::invokeMethod(q_func(), "selectLines", Q_ARG(QPoint, item->data(Qt::UserRole).toPoint()),
                                  Q_ARG(QPoint, item->data(Qt::UserRole + 1).toPoint()));
    else
        QMetaObject::invokeMethod(q_func(), "gotoLine", Q_ARG(QPoint, item->data(Qt::UserRole).toPoint()));
}

/*============================================================================
================================ BSplittedLinesDialog ========================
============================================================================*/

/*============================== Public constructors =======================*/

BSplittedLinesDialog::BSplittedLinesDialog(const QList<BCodeEdit::SplittedLinesRange> &ranges,
                                           int lineLength, QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSplittedLinesDialogPrivate(this, ranges, lineLength))
{
    d_func()->init();
}

BSplittedLinesDialog::~BSplittedLinesDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSplittedLinesDialog::BSplittedLinesDialog(BSplittedLinesDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================================================================
================================ BDropHandler ================================
============================================================================*/

/*============================== Public constructors =======================*/

BDropHandler::BDropHandler(BCodeEditor *parent) :
    QObject(parent), Editor(parent)
{
    //
}

BDropHandler::~BDropHandler()
{
    //
}

/*============================== Public methods ============================*/

bool BDropHandler::eventFilter(QObject *o, QEvent *e)
{
    if (!Editor)
        return QObject::eventFilter(o, e);
    switch (e->type()) {
    case QEvent::DragEnter: {
        QDragEnterEvent *de = static_cast<QDragEnterEvent *>(e);
        const QMimeData *md = de->mimeData();
        if (!md || !md->hasUrls())
            return QObject::eventFilter(o, e);
        de->acceptProposedAction();
        return true;
    }
    case QEvent::DragMove: {
        QDragMoveEvent *de = static_cast<QDragMoveEvent *>(e);
        const QMimeData *md = de->mimeData();
        if (!md || !md->hasUrls())
            return QObject::eventFilter(o, e);
        de->acceptProposedAction();
        return true;
    }
    case QEvent::DragLeave:
        e->accept();
        return true;
    case QEvent::Drop: {
        const QMimeData *md = static_cast<QDropEvent *>(e)->mimeData();
        if (!md || !md->hasUrls())
            return QObject::eventFilter(o, e);
        QStringList list;
        foreach (const QUrl &url, md->urls())
            list << url.toLocalFile();
        Editor->openDocuments(list);
        return true;
    }
    default:
        return QObject::eventFilter(o, e);
    }
}

/*============================================================================
================================ BCloseHandler ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BCloseHandler::BCloseHandler(BCodeEditorPrivate *parent) :
    QObject(parent), Editor(parent)
{
    lastSender = 0;
}

BCloseHandler::~BCloseHandler()
{
    //
}

/*============================== Public methods ============================*/

bool BCloseHandler::eventFilter(QObject *o, QEvent *e)
{
    if (!Editor || e->type() != QEvent::Close)
        return false;
    if (!Editor->closeAllDocuments()) {
        e->ignore();
        return true;
    }
    if (!Editor->processedDocuments.isEmpty()) {
        if (!lastSender) {
            lastSender = o;
            Editor->showClosingMessage(qobject_cast<QWidget *>(o));
            connect(Editor->q_func(), SIGNAL(allDocumentsProcessed()), this, SLOT(processingFinished()));
        }
        e->ignore();
        return true;
    }
    return false;
}

/*============================== Public slots ==============================*/

void BCloseHandler::processingFinished()
{
    //disconnect(Editor->q_func(), SIGNAL(allDocumentsProcessed()), this, SLOT(processingFinished()));
    QWidget *wgt = qobject_cast<QWidget *>(lastSender);
    if (wgt)
        wgt->close();
    lastSender = 0;
}

/*============================================================================
================================ BCodeEditorPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBaseObjectPrivate(q)
{
    //
}

BCodeEditorPrivate::~BCodeEditorPrivate()
{
    fileTypes.remove(BAbstractFileType::defaultFileTypeId());
    foreach (BAbstractFileType *ft, fileTypes)
        delete ft;
}

/*============================== Static public methods =====================*/

QString BCodeEditorPrivate::createFileName(const QString &fileName, const QString &defaultName,
                                           const QStringList &existingNames)
{
    QFileInfo fi(!defaultName.isEmpty() ? defaultName : defaultFileName());
    QString fnbasens = fi.baseName();
    QString fnbasesuff = fi.suffix();
    QFileInfo fif(fileName);
    if (!fileName.isEmpty() && (fif.baseName().remove(QRegExp(" \\d$")) != fnbasens || fif.suffix() != fnbasesuff))
        return fileName;
    int i = 1;
    QString fn = fnbasens + " " + QString::number(i++) + (!fnbasesuff.isEmpty() ? ("." + fnbasesuff) : "");
    while (existingNames.contains(fn))
        fn = fnbasens + " " + QString::number(i++) + (!fnbasesuff.isEmpty() ? ("." + fnbasesuff) : "");
    return fn;
}

QString BCodeEditorPrivate::defaultFileName()
{
    return tr("New document", "fileName");
}

/*============================== Public methods ============================*/

void BCodeEditorPrivate::addDocument(BAbstractCodeEditorDocument *doc)
{
    if (!doc)
        return;
    emitDocumentAboutToBeAdded(doc);
    twgt->addTab(doc, "");
    twgt->setCurrentWidget(doc);
    updateDocumentTab(doc);
    emitDocumentAdded(doc);
}

void BCodeEditorPrivate::alreadyOpenedMessage(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    QMessageBox msg(q_func());
    msg.setWindowTitle(tr("File already opened", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("The file is already opened:", "msgbox text") + "\n" + fileName);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void BCodeEditorPrivate::appendFileHistory(const QString &fileName, const QString &oldFileName)
{
    if (!maxHistoryCount)
        return;
    if (fileName.isEmpty())
        return;
    if (!oldFileName.isEmpty())
        fileHistory.removeAll(oldFileName);
    fileHistory.removeAll(fileName);
    fileHistory.prepend(fileName);
    if (maxHistoryCount >= 0)
        while (fileHistory.size() > maxHistoryCount)
            fileHistory.removeLast();
    emitFileHistoryChanged(fileHistory);
}

bool BCodeEditorPrivate::closeAllDocuments(bool save)
{
    QList<BAbstractCodeEditorDocument *> list = q_func()->documents();
    if (list.isEmpty())
        return true;
    if (save) {
        QList<BAbstractCodeEditorDocument *> nslist;
        for (int i = list.size() - 1; i >= 0; --i) {
            BAbstractCodeEditorDocument *doc = list.at(i);
            if (openingDocuments.contains(doc) || savingDocuments.contains(doc)) {
                list.removeAt(i);
                continue;
            }
            if (doc->isModified()) {
                list.removeAt(i);
                nslist << doc;
            }
        }
        if (!nslist.isEmpty()) {
            if (nslist.size() == 1)
                return closeDocument(nslist.first());
            BSelectDocumentsDialog sdlg(nslist);
            sdlg.exec();
            switch (sdlg.decision()) {
            case QMessageBox::Save: {
                QList<BAbstractCodeEditorDocument *> selected = sdlg.selectedDocuments();
                for (int i = nslist.size() - 1; i >= 0; --i) {
                    BAbstractCodeEditorDocument *doc = nslist.at(i);
                    if (!selected.contains(doc)) {
                        nslist.removeAt(i);
                        list << doc;
                    }
                }
                foreach (BAbstractCodeEditorDocument *doc, nslist) {
                    if (!tryCloseDocument(doc))
                        return false;
                }
                break;
            }
            case QMessageBox::Discard:
                foreach (BAbstractCodeEditorDocument *doc, nslist)
                    list << doc;
                break;
            case QMessageBox::Cancel:
            default:
                return false;
            }
        }
    }
    foreach (BAbstractCodeEditorDocument *doc, list)
        removeDocument(doc);
    return true;
}

bool BCodeEditorPrivate::closeDocument(BAbstractCodeEditorDocument *doc)
{
    if (!doc || openingDocuments.contains(doc) || savingDocuments.contains(doc))
        return false;
    if ((!isDefaultFileName(doc->fileName()) && !driver->testFileExistance(doc->fileName())) || doc->isModified()) {
        switch (closeModifiedMessage(doc->fileName())) {
        case QMessageBox::Save:
            return tryCloseDocument(doc);
        case QMessageBox::Discard:
            removeDocument(doc);
            return true;
        case QMessageBox::Cancel:
        default:
            return false;
        }
    } else {
        removeDocument(doc);
        return true;
    }
}

int BCodeEditorPrivate::closeModifiedMessage(const QString &fileName)
{
    QMessageBox msg(q_func());
    msg.setWindowTitle(tr("Closing modified document", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Question);
    msg.setText(tr("Document is modified:", "msgbox text") + "\n" + fileName);
    msg.setInformativeText(tr("Do you want to save it before closing?", "msgbox informativeText"));
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Discard);
    return msg.exec();
}

BAbstractCodeEditorDocument *BCodeEditorPrivate::createDocument(const QString &fileName, const QString &text)
{
    BAbstractCodeEditorDocument *doc = q_func()->createDocument(docType, q_func());
    if (!doc)
        return 0;
    doc->init();
    doc->setFileName(createFileName(fileName, defaultFN, q_func()->fileNames()));
    doc->installInnerEventFilter(this);
    doc->installInnerViewportEventFilter(dropHandler);
    doc->setEditFont(editFont);
    BCodeEditorDocument *ddoc  = qobject_cast<BCodeEditorDocument *>(doc);
    if (ddoc) {
        ddoc->setEditMode(editMode);
        ddoc->setEditLineLength(editLineLength);
    }
    doc->setEditTabWidth(editTabWidth);
    doc->setEditAutoIndentationEnabled(editAutoIndentation);
    doc->setLineNumberWidgetVisible(lineNumberVisible);
    doc->setBracketHighlightingEnabled(bracketsHighlighting);
    doc->setSpellChecker(spellChecker);
    doc->setCodec(defaultCodec);
    doc->setAsyncProcessingMinimumLength(asyncMin);
    doc->setFileType(selectDocumentFileType(doc));
    if (ddoc) {
        connect(ddoc, SIGNAL(lineSplitted(BCodeEdit::SplittedLinesRange)),
                this, SLOT(documentLineSplitted(BCodeEdit::SplittedLinesRange)));
        connect(ddoc, SIGNAL(linesSplitted(QList<BCodeEdit::SplittedLinesRange>)),
                this, SLOT(documentLinesSplitted(QList<BCodeEdit::SplittedLinesRange>)));
    }
    connect(doc, SIGNAL(modificationChanged(bool)), this, SLOT(documentModificationChanged(bool)));
    connect(doc, SIGNAL(fileNameChanged(QString)), this, SLOT(documentFileNameChanged(QString)));
    connect(doc, SIGNAL(loadingFinished(bool)), this, SLOT(documentLoadingFinished(bool)));
    connect(doc, SIGNAL(savingFinished(bool)), this, SLOT(documentSavingFinished(bool)));
    connect(doc, SIGNAL(buisyChanged(bool)), this, SLOT(documentBuisyChanged(bool)));
    if (!text.isEmpty())
        doc->setText(text);
    return doc;
}

BSplittedLinesDialog *BCodeEditorPrivate::createSplittedLinesDialog(BCodeEditorDocument *doc,
                                                                    const QList<BCodeEdit::SplittedLinesRange> ranges)
{
    if (!doc || !processedDocuments.contains(doc))
        return 0;
    BSplittedLinesDialog *sld = doc->splittedLinesDialog();
    if (sld) {
        sld->close();
        sld->deleteLater();
    }
    sld = new BSplittedLinesDialog(ranges, doc->editLineLength(), q_func());
    doc->setSplittedLinesDialog(sld);
    connect(sld, SIGNAL(gotoLine(QPoint)), doc, SLOT(moveCursor(QPoint)));
    connect(sld, SIGNAL(selectLines(QPoint, QPoint)), doc, SLOT(selectText(QPoint, QPoint)));
    connect(sld, SIGNAL(gotoLine(QPoint)), doc, SLOT(activateWindow()));
    connect(sld, SIGNAL(selectLines(QPoint, QPoint)), doc, SLOT(activateWindow()));
    sld->show();
    return sld;
}

void BCodeEditorPrivate::emitCurrentDocumentChanged(BAbstractCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->currentDocumentChanged(doc);
    QMetaObject::invokeMethod(q_func(), "currentDocumentChanged", Q_ARG(BAbstractCodeEditorDocument *, doc));
    QMetaObject::invokeMethod(q_func(), "documentAvailableChanged", Q_ARG(bool, doc));
}

void BCodeEditorPrivate::emitCurrentDocumentFileNameChanged(const QString &fileName)
{
    QMetaObject::invokeMethod(q_func(), "currentDocumentFileNameChanged", Q_ARG(QString, fileName));
}

void BCodeEditorPrivate::emitCurrentDocumentModificationChanged(bool modified)
{
    QMetaObject::invokeMethod(q_func(), "currentDocumentModificationChanged", Q_ARG(bool, modified));
}

void BCodeEditorPrivate::emitDefaultCodecChanged(const QString &codecName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->defaultCodecChanged(codecName);
    QMetaObject::invokeMethod(q_func(), "defaultCodecChanged", Q_ARG(QString, codecName));
}

void BCodeEditorPrivate::emitDocumentAboutToBeAdded(BAbstractCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentAboutToBeAdded(doc);
    QMetaObject::invokeMethod(q_func(), "documentAboutToBeAdded", Q_ARG(BAbstractCodeEditorDocument *, doc));
}

void BCodeEditorPrivate::emitDocumentAboutToBeRemoved(BAbstractCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentAboutToBeRemoved(doc);
    QMetaObject::invokeMethod(q_func(), "documentAboutToBeRemoved", Q_ARG(BAbstractCodeEditorDocument *, doc));
}

void BCodeEditorPrivate::emitDocumentAdded(BAbstractCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentAdded(doc);
    QMetaObject::invokeMethod(q_func(), "documentAdded", Q_ARG(BAbstractCodeEditorDocument *, doc));
}

void BCodeEditorPrivate::emitDocumentRemoved(const QString &fileName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentRemoved(fileName);
    QMetaObject::invokeMethod(q_func(), "documentRemoved", Q_ARG(QString, fileName));
}

void BCodeEditorPrivate::emitEditModeChanged(BCodeEdit::EditMode mode)
{
    foreach (BAbstractEditorModule *mdl, modules)
        mdl->editModeChanged(mode);
    QMetaObject::invokeMethod(q_func(), "editModeChanged", Q_ARG(BCodeEdit::EditMode, mode));
}

void BCodeEditorPrivate::emitFileHistoryChanged(const QStringList &list)
{
    foreach (BAbstractEditorModule *module, modules)
        module->fileHistoryChanged(list);
    QMetaObject::invokeMethod(q_func(), "fileHistoryChanged", Q_ARG(QStringList, list));
}

void BCodeEditorPrivate::emitFileTypesChanged()
{
    foreach (BAbstractEditorModule *module, modules)
        module->fileTypesChanged();
    QMetaObject::invokeMethod(q_func(), "fileTypesChanged");
}

bool BCodeEditorPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return BBaseObjectPrivate::eventFilter(o, e);
    QKeyEvent *ke = static_cast<QKeyEvent *>(e);
    if (ke->modifiers() != Qt::ControlModifier || ke->key() != Qt::Key_Tab)
        return BBaseObjectPrivate::eventFilter(o, e);
    int count = twgt->count();
    if (!count)
        return true;
    int ind = twgt->currentIndex();
    if (ind < count - 1)
        ++ind;
    else
        ind = 0;
    twgt->setCurrentIndex(ind);
    return true;
}

void BCodeEditorPrivate::failedToOpenMessage(const QString &fileName, const QString &info)
{
    if (fileName.isEmpty())
        return;
    QMessageBox msg(q_func());
    msg.setWindowTitle(tr("Failed to open file", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Warning);
    msg.setText(tr("Failed to open file:", "msgbox text") + "\n" + fileName);
    if (!info.isEmpty())
        msg.setInformativeText(info);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void BCodeEditorPrivate::failedToSaveMessage(const QString &fileName, const QString &newFileName)
{
    if (fileName.isEmpty())
        return;
    QMessageBox msg(q_func());
    msg.setWindowTitle(tr("Failed to save file", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Warning);
    QString text = tr("Failed to open file:", "msgbox text") + "\n" + fileName;
    if (!newFileName.isEmpty() && fileName != newFileName)
        text += "\n" + tr("as:", "msgbox text") + "\n" + newFileName;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

bool BCodeEditorPrivate::findDocument(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;
    foreach (BAbstractCodeEditorDocument *doc, q_func()->documents()) {
        if (doc->fileName() == fileName) {
            twgt->setCurrentWidget(doc);
            return true;
        }
    }
    foreach (const QString &fn, openingDocuments) {
        if (fn == fileName)
            return true;
    }
    foreach (const QString &fn, savingDocuments) {
        if (fn == fileName)
            return true;
    }
    return false;
}

void BCodeEditorPrivate::init()
{
    B_Q(BCodeEditor);
    docType = BCodeEditor::StandardDocument;
    document = 0;
    editFont = BGuiTools::createMonospaceFont();
    editMode = BCodeEdit::NormalMode;
    editLineLength = 120;
    editTabWidth = BeQt::TabWidth4;
    editAutoIndentation = true;
    lineNumberVisible = true;
    bracketsHighlighting = true;
    spellChecker = 0;
    driver = 0;
    q->setDriver(new BLocalDocumentDriver);
    fileTypes.insert(BAbstractFileType::defaultFileTypeId(), BAbstractFileType::defaultFileType());
    preferredFileType = BAbstractFileType::defaultFileType();
    defaultCodec = QTextCodec::codecForName("UTF-8");
    defaultFN = defaultFileName();
    maximumFileSize = BeQt::Megabyte;
    asyncMin = 100 * 1000;
    autoCodecDetection = true;
    maxHistoryCount = 20;
    //
    QVBoxLayout *vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      twgt = new QTabWidget(q);
        twgt->setMovable(true);
        twgt->setTabsClosable(true);
        connect(twgt, SIGNAL(currentChanged(int)), this, SLOT(twgtCurrentChanged(int)));
        connect(twgt, SIGNAL(tabCloseRequested(int)), this, SLOT(twgtTabCloseRequested(int)));
      vlt->addWidget(twgt);
   //
   createDropHandler();
   createCloseHandler();
}

bool BCodeEditorPrivate::isDefaultFileName(const QString &fileName) const
{
    if (fileName.isEmpty())
        return false;
    QFileInfo fi(fileName);
    QFileInfo fid(!defaultFN.isEmpty() ? defaultFN : defaultFileName());
    if (fi.suffix() != fid.suffix())
        return false;
    QStringList sl = fi.baseName().split(' ');
    if (sl.isEmpty())
        return false;
    bool ok = false;
    if (sl.takeLast().toInt(&ok) < 1 || !ok)
        return false;
    return sl.join(" ") == fid.baseName();
}

BAbstractCodeEditorDocument *BCodeEditorPrivate::openDocument(QString fileName, QTextCodec *codec)
{
    fileName = QDir::fromNativeSeparators(fileName);
    if (fileName.isEmpty() || findDocument(fileName))
        return 0;
    if (maximumFileSize && QFileInfo(fileName).size() > maximumFileSize) {
        failedToOpenMessage(fileName, tr("The file is too large", "msgbox informativeText"));
        return 0;
    }
    BAbstractCodeEditorDocument *doc = createDocument(fileName);
    openingDocuments.insert(doc, fileName);
    if (!doc->load(driver, codec, fileName)) {
        openingDocuments.remove(doc);
        doc->deleteLater();
        doc = 0;
    }
    return doc;
}

void BCodeEditorPrivate::removeDocument(BAbstractCodeEditorDocument *doc)
{
    if (!doc)
        return;
    QString fn = doc->fileName();
    emitDocumentAboutToBeRemoved(doc);
    twgt->removeTab(twgt->indexOf(doc));
    BCodeEditorDocument *ddoc = qobject_cast<BCodeEditorDocument *>(doc);
    BSplittedLinesDialog *sld = ddoc ? ddoc->splittedLinesDialog() : 0;
    if (sld) {
        sld->close();
        sld->deleteLater();
    }
    delete doc;
    emitDocumentRemoved(fn);
}

bool BCodeEditorPrivate::reopenDocument(BAbstractCodeEditorDocument *doc, QTextCodec *codec)
{
    if (!doc || doc->isBuisy())
        return false;
    if (doc->isModified() && reopenModifiedMessage(doc->fileName()) != QMessageBox::Yes)
        return false;
    return doc->load(driver, codec);
}

int BCodeEditorPrivate::reopenModifiedMessage(const QString &fileName)
{
    QMessageBox msg(q_func());
    msg.setWindowTitle(tr("Reopening modified document", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Question);
    msg.setText(tr("Document is modified:", "msgbox text") + "\n" + fileName);
    msg.setInformativeText(tr("Do you want to reopen it anyway?", "msgbox informativeText"));
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec();
}

bool BCodeEditorPrivate::saveDocument(BAbstractCodeEditorDocument *doc, const QString &newFileName, QTextCodec *codec)
{
    if (!doc || savingDocuments.contains(doc))
        return false;
    QString nfn = newFileName;
    if (nfn.isEmpty() && !driver->testFileExistance(doc->fileName())) {
        if (!codec)
            codec = doc->codec();
        if (nfn.isEmpty() && !driver->getSaveAsFileName(q_func(), doc->fileName(), nfn, codec))
            return false;
        if (doc->fileName() != nfn && findDocument(nfn)) {
            alreadyOpenedMessage(nfn);
            return false;
        }
    } else {
        if (doc->isReadOnly()) {
            failedToSaveMessage(nfn);
            return false;
        }
        if (nfn.isEmpty())
            nfn = doc->fileName();
    }
    savingDocuments.insert(doc, nfn);
    bool b = doc->save(driver, codec, nfn);
    if (!b) {
        savingDocuments.remove(doc);
        failedToSaveMessage(doc->fileName(), nfn);
    }
    return b;
}

bool BCodeEditorPrivate::saveDocuments(const QList<BAbstractCodeEditorDocument *> &list)
{
    foreach (BAbstractCodeEditorDocument *doc, list) {
        if (!doc->isModified())
            continue;
        if (!saveDocument(doc))
            return false;
    }
    return true;
}

BAbstractFileType *BCodeEditorPrivate::selectDocumentFileType(BAbstractCodeEditorDocument *doc)
{
    if (!doc)
        return 0;
    QString fn = doc->fileName();
    foreach (BAbstractFileType *ft, fileTypes) {
        if (ft->matchesFileName(fn))
            return ft;
    }
    return preferredFileType;
}

void BCodeEditorPrivate::setDriverEditor(BAbstractDocumentDriver *drv, BCodeEditor *edr)
{
    if (!drv)
        return;
    drv->d_func()->setEditor(edr);
}

void BCodeEditorPrivate::setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr)
{
    if (!mdl)
        return;
    mdl->d_func()->setEditor(edr);
}

void BCodeEditorPrivate::showClosingMessage(QWidget *parent)
{
    q_func()->showClosingMessage(parent);
}

bool BCodeEditorPrivate::tryAddFileType(BAbstractFileType *ft)
{
    if (!ft || fileTypes.contains(ft->id()))
        return false;
    fileTypes.insert(ft->id(), ft);
    return true;
}

bool BCodeEditorPrivate::tryCloseDocument(BAbstractCodeEditorDocument *doc)
{
    if (!doc || openingDocuments.contains(doc) || savingDocuments.contains(doc) || closingDocuments.contains(doc))
        return false;
    closingDocuments << doc;
    if (saveDocument(doc)) {
        return true;
    } else {
        closingDocuments.removeAll(doc);
        return false;
    }
}

bool BCodeEditorPrivate::tryRemoveFileType(const QString &id)
{
    if (id.isEmpty() || id == BAbstractFileType::defaultFileTypeId())
        return false;
    BAbstractFileType *ft = fileTypes.take(id);
    if (!ft)
        return false;
    foreach (BAbstractCodeEditorDocument *doc, q_func()->documents()) {
        if (doc->fileType() == ft)
            doc->setFileType(selectDocumentFileType(doc));
    }
    if (ft == preferredFileType)
        preferredFileType = BAbstractFileType::defaultFileType();
    delete ft;
    return true;
}

void BCodeEditorPrivate::updateDocumentTab(BAbstractCodeEditorDocument *doc)
{
    if (!doc)
        return;
    int ind = twgt->indexOf(doc);
    if (ind < 0)
        return;
    twgt->setTabText(ind, QFileInfo(doc->fileName()).fileName());
    twgt->setTabIcon(ind, BApplication::icon(doc->isModified() ? "filesave" : ""));
    twgt->setTabToolTip(ind, QDir::toNativeSeparators(doc->fileName()));
}

/*============================== Public slots ==============================*/

void BCodeEditorPrivate::createCloseHandler()
{
    closeHandler = new BCloseHandler(this);
    connect(closeHandler, SIGNAL(destroyed()), this, SLOT(createCloseHandler()));
}

void BCodeEditorPrivate::createDropHandler()
{
    dropHandler = new BDropHandler(q_func());
    foreach (BAbstractCodeEditorDocument *doc, q_func()->documents())
        doc->installInnerViewportEventFilter(dropHandler);
    connect(dropHandler, SIGNAL(destroyed()), this, SLOT(createDropHandler()));
}

void BCodeEditorPrivate::documentBuisyChanged(bool buisy)
{
    BAbstractCodeEditorDocument *doc = static_cast<BAbstractCodeEditorDocument *>(sender());
    if (!doc)
        return;
    bool b = !processedDocuments.isEmpty();
    if (buisy) {
        if (!processedDocuments.contains(doc))
            processedDocuments << doc;
    } else {
        processedDocuments.removeAll(doc);
    }
    if (!processedDocuments.isEmpty() != b) {
        QMetaObject::invokeMethod(q_func(), "", Q_ARG(bool, !b));
        if (b)
            QMetaObject::invokeMethod(q_func(), "allDocumentsProcessed");
    }
    if (doc == document) {
        foreach (BAbstractEditorModule *module, modules)
            module->documentBuisyChanged(buisy);
    }
}


void BCodeEditorPrivate::documentCodecChanged(const QString &codecName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCodecChanged(codecName);
}

void BCodeEditorPrivate::documentCopyAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCopyAvailableChanged(available);
}

void BCodeEditorPrivate::documentCursorPositionChanged(const QPoint &pos)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCursorPositionChanged(pos);
}

void BCodeEditorPrivate::documentCutAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCutAvailableChanged(available);
}

void BCodeEditorPrivate::documentEditModeChanged(BCodeEdit::EditMode mode)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentEditModeChanged(mode);
}

void BCodeEditorPrivate::documentFileNameChanged(const QString &fn)
{
    BAbstractCodeEditorDocument *doc = static_cast<BAbstractCodeEditorDocument *>(sender());
    if (!doc)
        return;
    updateDocumentTab(doc);
    if (doc == document) {
        emitCurrentDocumentFileNameChanged(doc->fileName());
        foreach (BAbstractEditorModule *module, modules)
            module->documentFileNameChanged(fn);
    }
}

void BCodeEditorPrivate::documentFileTypeChanged(BAbstractFileType *ft)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentFileTypeChanged(ft);
}

void BCodeEditorPrivate::documentHasSelectionChanged(bool hasSelection)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentHasSelectionChanged(hasSelection);
}

void BCodeEditorPrivate::documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange)
{
    createSplittedLinesDialog(static_cast<BCodeEditorDocument *>(sender()),
                              QList<BCodeEdit::SplittedLinesRange>() << linesRange);
}

void BCodeEditorPrivate::documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges)
{
    createSplittedLinesDialog(static_cast<BCodeEditorDocument *>(sender()), linesRanges);
}

void BCodeEditorPrivate::documentLoadingFinished(bool success)
{
    BAbstractCodeEditorDocument *doc = static_cast<BAbstractCodeEditorDocument *>(sender());
    if (!doc)
        return;
    QString fn = doc->fileName();
    if (openingDocuments.contains(doc)) {
        fn = openingDocuments.take(doc);
        if (success) {
            addDocument(doc);
            appendFileHistory(fn);
        } else {
            doc->deleteLater();
        }
    }
    if (success)
        doc->setReadOnly(driver->testFileReadOnly(fn));
    else
        failedToOpenMessage(fn);
}

void BCodeEditorPrivate::documentModificationChanged(bool modified)
{
    BAbstractCodeEditorDocument *doc = static_cast<BAbstractCodeEditorDocument *>(sender());
    if (!doc)
        return;
    updateDocumentTab(doc);
    if (doc == document) {
        emitCurrentDocumentModificationChanged(modified);
        foreach (BAbstractEditorModule *module, modules)
            module->documentModificationChanged(modified);
    }
}

void BCodeEditorPrivate::documentPasteAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentPasteAvailableChanged(available);
}

void BCodeEditorPrivate::documentReadOnlyChanged(bool ro)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentReadOnlyChanged(ro);
}

void BCodeEditorPrivate::documentRedoAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentRedoAvailableChanged(available);
}

void BCodeEditorPrivate::documentSavingFinished(bool success)
{
    BAbstractCodeEditorDocument *doc = static_cast<BAbstractCodeEditorDocument *>(sender());
    if (!doc || !savingDocuments.contains(doc))
        return;
    QString fn = savingDocuments.take(doc);
    if (success) {
        doc->setReadOnly(driver->testFileReadOnly(!fn.isEmpty() ? fn : doc->fileName()));
        appendFileHistory(!fn.isEmpty() ? fn : doc->fileName(), doc->fileName());
    } else {
        failedToSaveMessage(doc->fileName(), fn);
    }
    if (closingDocuments.contains(doc)) {
        closingDocuments.removeAll(doc);
        removeDocument(doc);
    }
}

void BCodeEditorPrivate::documentSelectionChanged()
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentSelectionChanged();
}

void BCodeEditorPrivate::documentUndoAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentUndoAvailableChanged(available);
}

void BCodeEditorPrivate::twgtCurrentChanged(int index)
{
    BAbstractCodeEditorDocument *doc = (index >= 0) ? static_cast<BAbstractCodeEditorDocument *>(twgt->widget(index)) :
                                                      0;
    if (doc == document)
        return;
    if (document) {
        disconnect(document, SIGNAL(readOnlyChanged(bool)), this, SLOT(documentReadOnlyChanged(bool)));
        disconnect(document, SIGNAL(selectionChanged()), this, SLOT(documentSelectionChanged()));
        disconnect(document, SIGNAL(hasSelectionChanged(bool)), this, SLOT(documentHasSelectionChanged(bool)));
        disconnect(document, SIGNAL(cutAvailableChanged(bool)), this, SLOT(documentCutAvailableChanged(bool)));
        disconnect(document, SIGNAL(copyAvailableChanged(bool)), this, SLOT(documentCopyAvailableChanged(bool)));
        disconnect(document, SIGNAL(pasteAvailableChanged(bool)), this, SLOT(documentPasteAvailableChanged(bool)));
        disconnect(document, SIGNAL(undoAvailableChanged(bool)), this, SLOT(documentUndoAvailableChanged(bool)));
        disconnect(document, SIGNAL(redoAvailableChanged(bool)), this, SLOT(documentRedoAvailableChanged(bool)));
        BCodeEditorDocument *ddoc = qobject_cast<BCodeEditorDocument *>(document);
        if (ddoc) {
            disconnect(document, SIGNAL(editModeChanged(BCodeEdit::EditMode)),
                       this, SLOT(documentEditModeChanged(BCodeEdit::EditMode)));
        }
        disconnect(document, SIGNAL(cursorPositionChanged(QPoint)), this, SLOT(documentCursorPositionChanged(QPoint)));
        disconnect(document, SIGNAL(codecChanged(QString)), this, SLOT(documentCodecChanged(QString)));
        disconnect(document, SIGNAL(fileTypeChanged(BAbstractFileType *)),
                   this, SLOT(documentFileTypeChanged(BAbstractFileType *)));
    }
    document = doc;
    if (document) {
        connect(document, SIGNAL(readOnlyChanged(bool)), this, SLOT(documentReadOnlyChanged(bool)));
        connect(document, SIGNAL(selectionChanged()), this, SLOT(documentSelectionChanged()));
        connect(document, SIGNAL(hasSelectionChanged(bool)), this, SLOT(documentHasSelectionChanged(bool)));
        connect(document, SIGNAL(cutAvailableChanged(bool)), this, SLOT(documentCutAvailableChanged(bool)));
        connect(document, SIGNAL(copyAvailableChanged(bool)), this, SLOT(documentCopyAvailableChanged(bool)));
        connect(document, SIGNAL(pasteAvailableChanged(bool)), this, SLOT(documentPasteAvailableChanged(bool)));
        connect(document, SIGNAL(undoAvailableChanged(bool)), this, SLOT(documentUndoAvailableChanged(bool)));
        connect(document, SIGNAL(redoAvailableChanged(bool)), this, SLOT(documentRedoAvailableChanged(bool)));
        BCodeEditorDocument *ddoc = qobject_cast<BCodeEditorDocument *>(document);
        if (ddoc) {
            connect(document, SIGNAL(editModeChanged(BCodeEdit::EditMode)),
                    this, SLOT(documentEditModeChanged(BCodeEdit::EditMode)));
        }
        connect(document, SIGNAL(cursorPositionChanged(QPoint)), this, SLOT(documentCursorPositionChanged(QPoint)));
        connect(document, SIGNAL(codecChanged(QString)), this, SLOT(documentCodecChanged(QString)));
        connect(document, SIGNAL(fileTypeChanged(BAbstractFileType *)),
                this, SLOT(documentFileTypeChanged(BAbstractFileType *)));
        document->setFocus();
    }
    emitCurrentDocumentChanged(document);
    emitCurrentDocumentFileNameChanged(document ? document->fileName() : QString());
}

void BCodeEditorPrivate::twgtTabCloseRequested(int index)
{
    BAbstractCodeEditorDocument *doc = (index >= 0) ? static_cast<BAbstractCodeEditorDocument *>(twgt->widget(index)) :
                                                      0;
    if (doc)
        closeDocument(doc);
}

/*============================================================================
================================ BCodeEditor =================================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditor::BCodeEditor(QWidget *parent) :
    QWidget(parent), BBaseObject(*new BCodeEditorPrivate(this))
{
    d_func()->init();
    addModule(EditModule);
    addModule(IndicatorsModule);
    addModule(OpenSaveModule);
    addModule(SearchModule);
}

BCodeEditor::BCodeEditor(StandardDocumentType t, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BCodeEditorPrivate(this))
{
    d_func()->init();
    d_func()->docType = t;
    addModule(EditModule);
    addModule(IndicatorsModule);
    addModule(OpenSaveModule);
    addModule(SearchModule);
}

BCodeEditor::~BCodeEditor()
{
    //
}

/*============================== Protected constructors ====================*/

BCodeEditor::BCodeEditor(BCodeEditorPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

BAbstractEditorModule *BCodeEditor::createStandardModule(StandardModule type, BCodeEditor *parent)
{
    BAbstractEditorModule *mdl = 0;
    switch (type) {
    case BookmarksModule:
        mdl = new BBookmarksEditorModule(parent);
        break;
    case EditModule:
        mdl = new BEditEditorModule(parent);
        break;
    case IndicatorsModule:
        mdl = new BIndicatorsEditorModule(parent);
        break;
    case OpenSaveModule:
        mdl = new BOpenSaveEditorModule(parent);
        break;
    case SearchModule:
        mdl = new BSearchEditorModule(parent);
        break;
    default:
        return 0;
    }
    mdl->setProperty("beqt/standard_module_type", type);
    if (parent)
        parent->addModule(mdl);
    return mdl;
}

BCodeEditor::StandardDocumentType BCodeEditor::standardDocumentTypeFromInt(int t)
{
    static QList<int> Types = bRangeD(StandardDocument, SimpleDocument);
    return Types.contains(t) ? static_cast<StandardDocumentType>(t) : StandardDocument;
}

/*============================== Public methods ============================*/

void BCodeEditor::addFileType(BAbstractFileType *ft)
{
    if (d_func()->tryAddFileType(ft))
        d_func()->emitFileTypesChanged();
}

void BCodeEditor::addModule(BAbstractEditorModule *mdl)
{
    if (!mdl || mdl->isBuisy())
        return;
    B_D(BCodeEditor);
    if (d->modules.contains(mdl->id()))
        return;
    d->modules.insert(mdl->id(), mdl);
    d->setModuleEditor(mdl, this);
    if (!mdl->parent())
        mdl->setParent(this);
}

void BCodeEditor::addModule(StandardModule type)
{
    BAbstractEditorModule *mdl = createStandardModule(type, this);
    if (mdl->editor() != this)
        mdl->deleteLater();
}

int BCodeEditor::asyncProcessingMinimumLength() const
{
    return d_func()->asyncMin;
}

QObject *BCodeEditor::closeHandler() const
{
    return d_func()->closeHandler;
}

BAbstractCodeEditorDocument *BCodeEditor::currentDocument() const
{
    return d_func()->document;
}

QString BCodeEditor::currentDocumentFileName() const
{
    BAbstractCodeEditorDocument *doc = currentDocument();
    return doc ? doc->fileName() : QString();
}

QTextCodec *BCodeEditor::defaultCodec() const
{
    return d_func()->defaultCodec;
}

QString BCodeEditor::defaultCodecName() const
{
    return QString::fromLatin1(d_func()->defaultCodec->name().data());
}

QString BCodeEditor::defaultFileName() const
{
    return d_func()->defaultFN;
}

BAbstractCodeEditorDocument *BCodeEditor::document(const QString &fileName) const
{
    if (fileName.isEmpty())
        return 0;
    foreach (BAbstractCodeEditorDocument *doc, documents()) {
        if (doc->fileName() == fileName)
            return doc;
    }
    return 0;
}

bool BCodeEditor::documentAvailable() const
{
    return currentDocument();
}

QList<BAbstractCodeEditorDocument *> BCodeEditor::documents() const
{
    const B_D(BCodeEditor);
    QList<BAbstractCodeEditorDocument *> list;
    for (int i = 0; i < d->twgt->count(); ++i)
        list << static_cast<BAbstractCodeEditorDocument *>(d->twgt->widget(i));
    return list;
}

int BCodeEditor::documentType() const
{
    return d_func()->docType;
}

BAbstractDocumentDriver * BCodeEditor::driver() const
{
    return d_func()->driver;
}

QObject *BCodeEditor::dropHandler() const
{
    return d_func()->dropHandler;
}

QFont BCodeEditor::editFont() const
{
    return d_func()->editFont;
}

QString BCodeEditor::editFontFamily() const
{
    return editFont().family();
}

int BCodeEditor::editFontPointSize() const
{
    return editFont().pointSize();
}

int BCodeEditor::editLineLength() const
{
    return d_func()->editLineLength;
}

BCodeEdit::EditMode BCodeEditor::editMode() const
{
    return d_func()->editMode;
}

BeQt::TabWidth BCodeEditor::editTabWidth() const
{
    return d_func()->editTabWidth;
}

QStringList BCodeEditor::fileHistory() const
{
    return d_func()->fileHistory;
}

QStringList BCodeEditor::fileNames() const
{
    QStringList list;
    foreach (BAbstractCodeEditorDocument *doc, documents()) {
        if (doc)
            list << doc->fileName();
    }
    list.removeDuplicates();
    return list;
}

BAbstractFileType *BCodeEditor::fileType(const QString &id) const
{
    return d_func()->fileTypes.value(id);
}

QList<BAbstractFileType *> BCodeEditor::fileTypes() const
{
    QList<BAbstractFileType *> list = d_func()->fileTypes.values();
    return list;
}

bool BCodeEditor::isAutoCodecDetectionEnabled() const
{
    return d_func()->autoCodecDetection;
}

bool BCodeEditor::isBracketHighlightingEnabled() const
{
    return d_func()->bracketsHighlighting;
}

bool BCodeEditor::isBuisy() const
{
    return !d_func()->processedDocuments.isEmpty();
}

bool BCodeEditor::isCurrentDocumentModified() const
{
    BAbstractCodeEditorDocument *doc = currentDocument();
    return doc && doc->isModified();
}

bool BCodeEditor::isEditAutoIndentationEnabled() const
{
    return d_func()->editAutoIndentation;
}

bool BCodeEditor::isLineNumberWidgetVisible() const
{
    return d_func()->lineNumberVisible;
}

int BCodeEditor::maximumFileSize() const
{
    return d_func()->maximumFileSize;
}

int BCodeEditor::maximimHistorySize() const
{
    return d_func()->maxHistoryCount;
}

void BCodeEditor::mergeWith(BCodeEditor *other)
{
    if (!other)
        return;
    BAbstractCodeEditorDocument *cdoc = other->currentDocument();
    foreach (BAbstractCodeEditorDocument *doc, other->documents()) {
        BAbstractCodeEditorDocument *ndoc = d_func()->createDocument(doc->fileName(), doc->text());
        if (!ndoc)
            continue;
        if (cdoc == doc)
            cdoc = ndoc;
        ndoc->setModification(doc->isModified());
        ndoc->moveCursor(doc->cursorPosition());
        ndoc->selectText(doc->selectionStart(), doc->selectionEnd());
        other->d_func()->removeDocument(doc);
        d_func()->addDocument(ndoc);
    }
    setCurrentDocument(cdoc);
}

BAbstractEditorModule *BCodeEditor::module(const QString &name) const
{
    return d_func()->modules.value(name);
}

BAbstractEditorModule *BCodeEditor::module(StandardModule type) const
{
    foreach (BAbstractEditorModule *mdl, d_func()->modules) {
        QVariant v = mdl->property("beqt/standard_module_type");
        if (v.isNull())
            continue;
        if (static_cast<StandardModule>(v.toInt()) == type)
            return mdl;
    }
    return 0;
}

QList<BAbstractEditorModule *> BCodeEditor::modules() const
{
    return d_func()->modules.values();
}

BAbstractFileType *BCodeEditor::preferredFileType() const
{
    return d_func()->preferredFileType;
}

QString BCodeEditor::preferredFileTypeId() const
{
    return preferredFileType()->id();
}

void BCodeEditor::removeFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    removeFileType(ft->id());
}

void BCodeEditor::removeFileType(const QString &id)
{
    if (d_func()->tryRemoveFileType(id))
        d_func()->emitFileTypesChanged();
}

void BCodeEditor::removeModule(BAbstractEditorModule *mdl)
{
    if (!mdl)
        return;
    removeModule(mdl->id());
}

void BCodeEditor::removeModule(const QString &name)
{
    if (name.isEmpty())
        return;
    B_D(BCodeEditor);
    BAbstractEditorModule *mdl = d->modules.value(name);
    if (!mdl || mdl->isBuisy())
        return;
    d->modules.remove(name);
    d->setModuleEditor(mdl, 0);
}

void BCodeEditor::setAsyncProcessingMinimumLength(int length)
{
    if (length < 0)
        length = 0;
    B_D(BCodeEditor);
    if (d->asyncMin == length)
        return;
    d->asyncMin = length;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setAsyncProcessingMinimumLength(length);
}

void BCodeEditor::setAutoCodecDetectionEnabled(bool b)
{
    d_func()->autoCodecDetection = b;
}

void BCodeEditor::setBracketHighlightingEnabled(bool b)
{
    B_D(BCodeEditor);
    if (b == d->bracketsHighlighting)
        return;
    d->bracketsHighlighting = b;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setBracketHighlightingEnabled(b);
}

void BCodeEditor::setCurrentDocument(BAbstractCodeEditorDocument *doc)
{
    if (!doc || !documents().contains(doc))
        return;
    d_func()->twgt->setCurrentWidget(doc);
}

void BCodeEditor::setDefaultCodec(QTextCodec *codec)
{
    if (!codec || !BeQt::isCodecSupported(codec))
        return;
    d_func()->defaultCodec = codec;
    d_func()->emitDefaultCodecChanged(BeQt::codecName(codec));
}

void BCodeEditor::setDefaultCodec(const QString &codecName)
{
    setDefaultCodec(BeQt::codec(codecName));
}

void BCodeEditor::setDefaultFileName(const QString &fileName)
{
    d_func()->defaultFN = !fileName.isEmpty() ? fileName : BCodeEditorPrivate::defaultFileName();
}

void BCodeEditor::setDocumentType(int type)
{
    if (type == d_func()->docType)
        return;
    d_func()->docType = type;
    BAbstractCodeEditorDocument *cdoc = currentDocument();
    foreach (BAbstractCodeEditorDocument *doc, documents()) {
        BAbstractCodeEditorDocument *ndoc = d_func()->createDocument(doc->fileName(), doc->text());
        if (!ndoc)
            continue;
        if (cdoc == doc)
            cdoc = ndoc;
        ndoc->setModification(doc->isModified());
        ndoc->moveCursor(doc->cursorPosition());
        ndoc->selectText(doc->selectionStart(), doc->selectionEnd());
        d_func()->removeDocument(doc);
        d_func()->addDocument(ndoc);
    }
    setCurrentDocument(cdoc);
}

void BCodeEditor::setDriver(BAbstractDocumentDriver *drv)
{
    if (!drv)
        return;
    B_D(BCodeEditor);
    if (d->driver)
    {
        if (d->driver->isBuisy())
            return;
        if (d->driver->parent() == (QObject *) this) {
            d->setDriverEditor(d->driver, 0);
            d->driver->deleteLater();
        }
    }
    d->driver = drv;
    if (drv) {
        d->setDriverEditor(drv, this);
        if (!drv->parent())
            drv->setParent(this);
    }
}

void BCodeEditor::setEditAutoIndentationEnabled(bool enabled)
{
    B_D(BCodeEditor);
    if (d->editAutoIndentation == enabled)
        return;
    d->editAutoIndentation = enabled;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setEditAutoIndentationEnabled(enabled);
}

void BCodeEditor::setEditFont(const QFont &fnt)
{
    if (fnt.pointSize() < 1 && fnt.pixelSize() < 1)
        return;
    B_D(BCodeEditor);
    if (fnt == d->editFont)
        return;
    d->editFont = fnt;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setEditFont(fnt);
}

void BCodeEditor::setEditFontFamily(const QString &family)
{
    if (family.isEmpty())
        return;
    B_D(BCodeEditor);
    QFont fnt = d->editFont;
    fnt.setFamily(family);
    setEditFont(fnt);
}

void BCodeEditor::setEditFontPointSize(int pointSize)
{
    if (pointSize < 1)
        return;
    B_D(BCodeEditor);
    QFont fnt = d->editFont;
    fnt.setPointSize(pointSize);
    setEditFont(fnt);
}

void BCodeEditor::setEditLineLength(int ll)
{
    if (ll < 1)
        return;
    B_D(BCodeEditor);
    if (ll == d->editLineLength)
        return;
    d->editLineLength = ll;
    foreach (BAbstractCodeEditorDocument *doc, documents()) {
        BCodeEditorDocument *ddoc = qobject_cast<BCodeEditorDocument *>(doc);
        if (ddoc)
            ddoc->setEditLineLength(ll);
    }
}

void BCodeEditor::setEditMode(BCodeEdit::EditMode mode)
{
    B_D(BCodeEditor);
    if (d->editMode == mode)
        return;
    d->editMode = mode;
    foreach (BAbstractCodeEditorDocument *doc, documents()) {
        BCodeEditorDocument *ddoc = qobject_cast<BCodeEditorDocument *>(doc);
        if (ddoc)
            ddoc->setEditMode(mode);
    }
    d->emitEditModeChanged(mode);
}

void BCodeEditor::setEditTabWidth(BeQt::TabWidth tw)
{
    B_D(BCodeEditor);
    if (d->editTabWidth == tw)
        return;
    d->editTabWidth = tw;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setEditTabWidth(tw);
}

void BCodeEditor::setFileHistory(const QStringList &list)
{
    B_D(BCodeEditor);
    if (!d->maxHistoryCount || list == d->fileHistory)
        return;
    d->fileHistory = list;
    if (d->maxHistoryCount >= 0 && d->fileHistory.size() > d->maxHistoryCount) {
        while (d->fileHistory.size() > d->maxHistoryCount)
            d->fileHistory.removeLast();
    }
    d->emitFileHistoryChanged(d->fileHistory);
}

void BCodeEditor::setFileTypes(const QList<BAbstractFileType *> &list)
{
    bool b = false;
    foreach (BAbstractFileType *ft, fileTypes())
        b = (b || d_func()->tryRemoveFileType(ft->id()));
    foreach (BAbstractFileType *ft, list)
        b = (b || d_func()->tryAddFileType(ft));
    if (b)
        d_func()->emitFileTypesChanged();
}

void BCodeEditor::setLineNumberWidgetVisible(bool b)
{
    B_D(BCodeEditor);
    if (d->lineNumberVisible == b)
        return;
    d->lineNumberVisible = b;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setLineNumberWidgetVisible(b);
}

void BCodeEditor::setMaximumFileSize(int sz)
{
    if (sz < 1)
        sz = 0;
    d_func()->maximumFileSize = sz;
}

void BCodeEditor::setMaximumHistorySize(int count)
{
    B_D(BCodeEditor);
    if (count == d->maxHistoryCount)
        return;
    if (count < 0)
        count = -1;
    d->maxHistoryCount = count;
    if (d->maxHistoryCount >= 0 && d->fileHistory.size() > count) {
        while (d->fileHistory.size() > count)
            d->fileHistory.removeLast();
        d->emitFileHistoryChanged(d->fileHistory);
    }
}

void BCodeEditor::setModules(const QList<BAbstractEditorModule *> &list)
{
    B_D(BCodeEditor);
    foreach (const QString &name, d->modules.keys())
        removeModule(name);
    foreach (BAbstractEditorModule *mdl, list)
        addModule(mdl);
}

void BCodeEditor::setPreferredFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    setPreferredFileType(ft->id());
}

void BCodeEditor::setPreferredFileType(const QString &id)
{
    if (id.isEmpty())
        return;
    B_D(BCodeEditor);
    if (!d->fileTypes.contains(id))
        return;
    d->preferredFileType = d->fileTypes.value(id);
}

void BCodeEditor::setSpellChecker(BSpellChecker *sc)
{
    B_D(BCodeEditor);
    if (sc == d->spellChecker)
        return;
    d->spellChecker = sc;
    foreach (BAbstractCodeEditorDocument *doc, documents())
        doc->setSpellChecker(sc);
}

BSpellChecker *BCodeEditor::spellChecker() const
{
    return d_func()->spellChecker;
}

bool BCodeEditor::waitForAllDocumentsProcessed(int msecs) const
{
    if (!isBuisy())
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(allDocumentsProcessed()), msecs);
    return !isBuisy();
}

/*============================== Public slots ==============================*/

BAbstractCodeEditorDocument *BCodeEditor::addDocument(const QString &fileName)
{
    return addDocument(fileName, "");
}

BAbstractCodeEditorDocument *BCodeEditor::addDocument(const QString &fileName, const QString &text)
{
    B_D(BCodeEditor);
    if (d->findDocument(BCodeEditorPrivate::createFileName(fileName, d->defaultFN, fileNames())))
        return 0;
    BAbstractCodeEditorDocument *doc = d->createDocument(fileName, text);
    d->addDocument(doc);
    return doc;
}

bool BCodeEditor::closeAllDocuments(bool save)
{
    return d_func()->closeAllDocuments(save);
}

bool BCodeEditor::closeCurrentDocument()
{
    return d_func()->closeDocument(currentDocument());
}

void BCodeEditor::insertTextIntoCurrentDocument(const QString &text)
{
    if (!currentDocument())
        return;
    currentDocument()->insertText(text);
}

BAbstractCodeEditorDocument *BCodeEditor::openDocument(const QString &fileName, QTextCodec *codec)
{
    return d_func()->openDocument(fileName, codec);
}

QList<BAbstractCodeEditorDocument *> BCodeEditor::openDocuments()
{
    QStringList list;
    QTextCodec *codec = d_func()->defaultCodec;
    return d_func()->driver->getOpenFileNames(this, list, codec) ? openDocuments(list, codec) :
                                                                   QList<BAbstractCodeEditorDocument *>();
}

QList<BAbstractCodeEditorDocument *> BCodeEditor::openDocuments(const QStringList &fileNames, QTextCodec *codec)
{
    QList<BAbstractCodeEditorDocument *> list;
    foreach (const QString &fn, fileNames) {
        BAbstractCodeEditorDocument *doc = openDocument(fn, codec);
        if (!doc)
            continue;
        list << doc;
    }
    return list;
}

bool BCodeEditor::reopenCurrentDocument(QTextCodec *codec)
{
    return currentDocument() && d_func()->reopenDocument(currentDocument(), codec);
}

bool BCodeEditor::reopenCurrentDocument(const QString &codecName)
{
    return reopenCurrentDocument(QTextCodec::codecForName(codecName.toLatin1()));
}

bool BCodeEditor::saveAllDocuments()
{
    return d_func()->saveDocuments(documents());
}

bool BCodeEditor::saveCurrentDocument()
{
    BAbstractCodeEditorDocument *doc = currentDocument();
    return doc && (!doc->isModified() || d_func()->saveDocument(doc));
}

bool BCodeEditor::saveCurrentDocumentAs()
{
    if (!currentDocument())
        return false;
    QString nfn;
    QTextCodec *codec = currentDocument()->codec();
    return d_func()->driver->getSaveAsFileName(this, currentDocument()->fileName(), nfn, codec) &&
            d_func()->saveDocument(currentDocument(), nfn, codec);
}

bool BCodeEditor::saveCurrentDocumentAs(const QString &newFileName, QTextCodec *codec)
{
    if (newFileName.isEmpty() || !currentDocument())
        return false;
    return d_func()->saveDocument(currentDocument(), newFileName, codec);
}

void BCodeEditor::setCurrentDocumentText(const QString &text)
{
    if (!currentDocument())
        return;
    currentDocument()->setText(text);
}

/*============================== Protected methods =========================*/

BAbstractCodeEditorDocument *BCodeEditor::createDocument(int type, BCodeEditor *editor) const
{
    switch (type) {
    case SimpleDocument:
        return new BSimpleCodeEditorDocument(editor);
    case StandardDocument:
    default:
        return new BCodeEditorDocument(editor);
    }
}

void BCodeEditor::showClosingMessage(QWidget *parent)
{
    QMessageBox *msg = new QMessageBox(parent);
    msg->setWindowTitle(tr("Saving documents...", "msgbox windowTitle"));
    msg->setIcon(QMessageBox::Information);
    msg->setText(tr("The files are being saved. Please, wait...", "msgbox text"));
    msg->setStandardButtons(0);
    connect(this, SIGNAL(allDocumentsProcessed()), msg, SLOT(close()));
    msg->open();
}
