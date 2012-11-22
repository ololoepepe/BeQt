#include "bcodeeditor.h"
#include "bcodeeditor_p.h"
#include "bcodeeditordocument.h"
#include "babstracteditormodule.h"
#include "blocaldocumentdriver.h"
#include "babstracteditormodule_p.h"
#include "bbookmarkseditormodule.h"
#include "bsearcheditormodule.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QList>
#include <QMap>
#include <QString>
#include <QMetaObject>
#include <QAction>
#include <QFont>
#include <QFontInfo>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QLabel>
#include <QListWidgetItem>
#include <QEventLoop>
#include <QTimer>

#include <QDebug>

/*============================================================================
================================ Select Documents Dialog Private
============================================================================*/

BSelectDocumentsDialogPrivate::BSelectDocumentsDialogPrivate(BSelectDocumentsDialog *q,
                                                             const QList<BCodeEditorDocument *> &list) :
    BBasePrivate(q), Documents(list)
{
    //
}

BSelectDocumentsDialogPrivate::~BSelectDocumentsDialogPrivate()
{
    //
}

//

void BSelectDocumentsDialogPrivate::init()
{
    B_Q(BSelectDocumentsDialog);
    decision = QMessageBox::Cancel;
    //
    vlt = new QVBoxLayout(q);
      lblText = new QLabel(q);
        QFont fnt;
        //fnt.setPointSize(fnt.pointSize() + 4);
        fnt.setBold(true);
        lblText->setWordWrap(true);
        lblText->setFont(fnt);
        lblText->setText( trq("Some documents are modified. Do you want to save them before closing?", "lbl text") );
      vlt->addWidget(lblText);
      lblInformativeTextUpper = new QLabel(q);
        lblInformativeTextUpper->setWordWrap(true);
        lblInformativeTextUpper->setText( trq("Choose documents that you want to save:", "lbl text") );
      vlt->addWidget(lblInformativeTextUpper);
      lstwgt = new QListWidget(q);
        foreach (BCodeEditorDocument *doc, Documents)
        {
            QString fn = doc->fileName();
            QListWidgetItem *lwi = new QListWidgetItem( QFileInfo(fn).fileName() );
            lwi->setData(Qt::ToolTipRole, fn);
            lwi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            lwi->setCheckState(Qt::Checked);
            lstwgt->addItem(lwi);
        }
      vlt->addWidget(lstwgt);
      lblInformativeTextLower = new QLabel(q);
        lblInformativeTextLower->setWordWrap(true);
        lblInformativeTextLower->setText( trq("If you don't save the documents, all changes will be lost",
                                              "lbl text") );
      vlt->addWidget(lblInformativeTextLower);
      dlgbbox = new QDialogButtonBox(q);
        dlgbbox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Discard | QDialogButtonBox::Cancel);
        dlgbbox->button(QDialogButtonBox::Discard)->setDefault(true);
        connect( dlgbbox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( dlgbboxClicked(QAbstractButton *) ) );
      vlt->addWidget(dlgbbox);
}

//

void BSelectDocumentsDialogPrivate::dlgbboxClicked(QAbstractButton *button)
{
    if ( button == (QAbstractButton *) dlgbbox->button(QDialogButtonBox::Save) )
    {
        decision = QMessageBox::Save;
        q_func()->accept();
    }
    else if ( button == (QAbstractButton *) dlgbbox->button(QDialogButtonBox::Discard) )
    {
        decision = QMessageBox::Discard;
        q_func()->accept();
    }
    else if ( button == (QAbstractButton *) dlgbbox->button(QDialogButtonBox::Cancel) )
    {
        decision = QMessageBox::Cancel;
        q_func()->reject();
    }
}

/*============================================================================
================================ Select Documents Dialog
============================================================================*/

BSelectDocumentsDialog::BSelectDocumentsDialog(const QList<BCodeEditorDocument *> &documents, QWidget *parent) :
    QDialog(parent), BBase( *new BSelectDocumentsDialogPrivate(this, documents) )
{
    d_func()->init();
}

BSelectDocumentsDialog::~BSelectDocumentsDialog()
{
    //
}

//

int BSelectDocumentsDialog::decision() const
{
    return d_func()->decision;
}

QList<BCodeEditorDocument *> BSelectDocumentsDialog::selectedDocuments() const
{
    const B_D(BSelectDocumentsDialog);
    QList<BCodeEditorDocument *> list;
    for (int i = 0; i < d->lstwgt->count(); ++i)
        if (d->lstwgt->item(i)->checkState() == Qt::Checked)
            list << d->Documents.at(i);
    return list;
}

//

BSelectDocumentsDialog::BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    //
}

/*============================================================================
================================ Code Editor Private
============================================================================*/

QString BCodeEditorPrivate::defaultFileName()
{
    return trq("New document", "fileName");
}

QString BCodeEditorPrivate::createFileName(const QString &fileName)
{
    return !fileName.isEmpty() ? fileName : defaultFileName();
}

//

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBasePrivate(q)
{
    //
}

BCodeEditorPrivate::~BCodeEditorPrivate()
{
    //
}

//

void BCodeEditorPrivate::init()
{
    B_Q(BCodeEditor);
    document = 0;
    editFont = QFont( QFontInfo( QFont("monospace") ).family() );
    editMode = BCodeEdit::NormalMode;
    editLineLength = 120;
    editTabWidth = BCodeEdit::TabWidth4;
    bracketsHighlighting = true;
    closingAll = false;
    savingAll = false;
    driver = new BLocalDocumentDriver(q);
    //
    vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      twgt = new QTabWidget(q);
        twgt->setMovable(true);
        twgt->setTabsClosable(true);
        connect( twgt, SIGNAL( currentChanged(int) ), this, SLOT( twgtCurrentChanged(int) ) );
        connect( twgt, SIGNAL( tabCloseRequested(int) ), this, SLOT( twgtTabCloseRequested(int) ) );
      vlt->addWidget(twgt);
}

bool BCodeEditorPrivate::findDocument(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return false;
    foreach ( BCodeEditorDocument *doc, q_func()->documents() )
    {
        if (doc->fileName() == fileName)
        {
            twgt->setCurrentWidget(doc);
            return true;
        }
    }
    foreach (const QString &fn, openingDocuments)
        if (fn == fileName)
            return true; //TODO: Show message
    foreach (const QString &fn, savingDocuments)
        if (fn == fileName)
            return true; //TODO: Show message
    return false;
}

BCodeEditorDocument *BCodeEditorPrivate::createDocument(const QString &fileName, const QString &text)
{
    BCodeEditorDocument *doc = new BCodeEditorDocument;
    doc->setFileName( createFileName(fileName) );
    if ( !text.isEmpty() )
        doc->setText(text);
    doc->setEditFont(editFont);
    doc->setEditMode(editMode);
    doc->setEditLineLength(editLineLength);
    doc->setEditTabWidth(editTabWidth);
    doc->setBracketHighlightingEnabled(bracketsHighlighting);
    connect( doc, SIGNAL( loadingFinished(bool) ), this, SLOT( documentLoadingFinished(bool) ) );
    connect( doc, SIGNAL( savingFinished(bool) ), this, SLOT( documentSavingFinished(bool) ) );
    return doc;
}

void BCodeEditorPrivate::addDocument(BCodeEditorDocument *doc)
{
    if (!doc)
        return;
    emitDocumentAboutToBeAdded(doc);
    twgt->addTab(doc, "");
    twgt->setCurrentWidget(doc);
    updateDocumentTab(doc);
    emitDocumentAdded(doc);
}

void BCodeEditorPrivate::removeDocument(BCodeEditorDocument *doc)
{
    if (!doc)
        return;
    emitDocumentAboutToBeRemoved(doc);
    twgt->removeTab( twgt->indexOf(doc) );
    doc->deleteLater();
}

bool BCodeEditorPrivate::saveDocument(BCodeEditorDocument *doc, const QString &newFileName)
{
    if (!doc)
        return false;
    if ( newFileName.isEmpty() )
    {
        if ( !doc->isModified() || doc->isReadOnly() )
            return false;
    }
    else
    {
        if ( findDocument(newFileName) )
            return false;
    }
    savingDocuments.insert(doc, newFileName);
    bool b = doc->save(driver, newFileName);
    if (!b)
    {
        savingDocuments.remove(doc);
        failedToSaveMessage(doc->fileName(), newFileName);
    }
    return b;
}

bool BCodeEditorPrivate::saveDocuments(QList<BCodeEditorDocument *> &list)
{
    if ( list.isEmpty() )
        return true;
    foreach (BCodeEditorDocument *doc, list)
    {
        if ( !doc->isModified() )
            continue;
        closingDocuments.insert(doc, "");
        QString nfn;
        bool sa = q_func()->shouldSaveAs(doc->fileName(), nfn);
        if ( !saveDocument( doc, sa ? nfn : QString() ) )
        {
            closingDocuments.remove(doc);
            if ( closingDocuments.isEmpty() )
                closingAll = false;
            return false;
        }
    }
    return true;
}

bool BCodeEditorPrivate::closeDocument(BCodeEditorDocument *doc)
{
    if (!doc)
        return false;
    if ( doc->isModified() )
    {
        switch ( closeModifiedMessage( doc->fileName() ) )
        {
        case QMessageBox::Save:
        {
            closingDocuments.insert(doc, "");
            QString nfn;
            bool sa = q_func()->shouldSaveAs(doc->fileName(), nfn);
            bool b = saveDocument( doc, sa ? nfn : QString() );
            if (!b)
                closingDocuments.remove(doc);
            return b;
        }
        case QMessageBox::Discard:
            removeDocument(doc);
            return true;
        case QMessageBox::Cancel:
        default:
            return false;
        }
    }
    else
    {
        removeDocument(doc);
        return true;
    }
}

bool BCodeEditorPrivate::closeAllDocuments()
{
    if (closingAll)
        return false;
    QList<BCodeEditorDocument *> list = q_func()->documents();
    if ( list.isEmpty() )
        return true;
    closingAll = true;
    QList<BCodeEditorDocument *> nslist;
    for (int i = list.size() - 1; i >= 0; --i)
        if ( list.at(i)->isModified() )
            nslist << list.takeAt(i);
    if ( !nslist.isEmpty() )
    {
        if (nslist.size() == 1)
        {
            BCodeEditorDocument *doc = nslist.first();
            closingDocuments.insert(doc, "");
            QString nfn;
            bool sa = q_func()->shouldSaveAs(doc->fileName(), nfn);
            bool b = saveDocument( doc, sa ? nfn : QString() );
            if (!b)
                closingDocuments.remove(doc);
            closingAll = false;
            emitAllDocumentsClosed(b);
            return b;
        }
        BSelectDocumentsDialog sdlg(nslist);
        sdlg.exec();
        switch ( sdlg.decision() )
        {
        case QMessageBox::Save:
        {
            QList<BCodeEditorDocument *> selected = sdlg.selectedDocuments();
            foreach (BCodeEditorDocument *doc, nslist)
                if ( !selected.contains(doc) )
                    list << doc;
            foreach (BCodeEditorDocument *doc, selected)
            {
                closingDocuments.insert(doc, "");
                QString nfn;
                bool sa = q_func()->shouldSaveAs(doc->fileName(), nfn);
                bool b = saveDocument( doc, sa ? nfn : QString() );
                if (!b)
                {
                    closingDocuments.remove(doc);
                    closingAll = !closingDocuments.isEmpty();
                    if (!closingAll)
                        emitAllDocumentsClosed(false);
                    return false;
                }
            }
            break;
        }
        case QMessageBox::Discard:
            foreach (BCodeEditorDocument *doc, nslist)
                removeDocument(doc);
            break;
        case QMessageBox::Cancel:
        default:
            return false;
        }
    }
    foreach (BCodeEditorDocument *doc, list)
        removeDocument(doc);
    closingAll = !closingDocuments.isEmpty();
    if (!closingAll)
        emitAllDocumentsClosed(true);
    return true;
}

void BCodeEditorPrivate::updateDocumentTab(BCodeEditorDocument *doc)
{
    if (!doc)
        return;
    int ind = twgt->indexOf(doc);
    if (ind < 0)
        return;
    twgt->setTabText( ind, QFileInfo( doc->fileName() ).fileName() );
    twgt->setTabIcon( ind, BApplication::icon(doc->isModified() ? "filesave" : "") );
    twgt->setTabToolTip( ind, doc->fileName() );
}

//Messages

void BCodeEditorPrivate::failedToOpenMessage(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return;
    QMessageBox msg( q_func() );
    msg.setWindowTitle( trq("Failed to open file", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Warning);
    msg.setText(trq("Failed to open file:", "msgbox text") + "\n" + fileName);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

void BCodeEditorPrivate::failedToSaveMessage(const QString &fileName, const QString &newFileName)
{
    if ( fileName.isEmpty() )
        return;
    QMessageBox msg( q_func() );
    msg.setWindowTitle( trq("Failed to save file", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Warning);
    QString text = trq("Failed to open file:", "msgbox text") + "\n" + fileName;
    if (!newFileName.isEmpty() && fileName != newFileName)
        text += "\n" + trq("as:", "msgbox text") + "\n" + newFileName;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

int BCodeEditorPrivate::closeModifiedMessage(const QString &fileName)
{
    QMessageBox msg( q_func() );
    msg.setWindowTitle( trq("Closing modified document", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Question);
    msg.setText(trq("Document is modified:", "msgbox text") + "\n" + fileName);
    msg.setInformativeText( trq("Do you want to save it before closing?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Discard);
    return msg.exec();
}

//Signal emitting

void BCodeEditorPrivate::emitDocumentAboutToBeAdded(BCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentAboutToBeAdded(doc);
    QMetaObject::invokeMethod( q_func(), "documentAboutToBeAdded", Q_ARG(BCodeEditorDocument *, doc) );
}

void BCodeEditorPrivate::emitDocumentAdded(BCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentAdded(doc);
    QMetaObject::invokeMethod( q_func(), "documentAdded", Q_ARG(BCodeEditorDocument *, doc) );
}

void BCodeEditorPrivate::emitDocumentAboutToBeRemoved(BCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentAboutToBeRemoved(doc);
    QMetaObject::invokeMethod( q_func(), "documentAboutToBeRemoved", Q_ARG(BCodeEditorDocument *, doc) );
}

void BCodeEditorPrivate::emitCurrentDocumentChanged(BCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->currentDocumentChanged(doc);
    QMetaObject::invokeMethod( q_func(), "currentDocumentChanged", Q_ARG(BCodeEditorDocument *, doc) );
    QMetaObject::invokeMethod( q_func(), "documentAvailableChanged", Q_ARG(bool, doc) );
}

void BCodeEditorPrivate::emitAllDocumentsClosed(bool success)
{
    QMetaObject::invokeMethod( q_func(), "allDocumentsClosed", Q_ARG(bool, success) );
}

//External private class call

void BCodeEditorPrivate::setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr)
{
    if (!mdl)
        return;
    mdl->d_func()->setEditor(edr);
}

//

void BCodeEditorPrivate::twgtCurrentChanged(int index)
{
    BCodeEditorDocument *doc = (index >= 0) ? static_cast<BCodeEditorDocument *>( twgt->widget(index) ) : 0;
    if (doc == document)
        return;
    if (document)
    {
        disconnect( document, SIGNAL( readOnlyChanged(bool) ), this, SLOT( documentReadOnlyChanged(bool) ) );
        disconnect( document, SIGNAL( modificationChanged(bool) ), this, SLOT( documentModificationChanged(bool) ) );
        disconnect( document, SIGNAL( selectionChanged() ), this, SLOT( documentSelectionChanged() ) );
        disconnect( document, SIGNAL( hasSelectionChanged(bool) ), this, SLOT( documentHasSelectionChanged(bool) ) );
        disconnect( document, SIGNAL( cutAvailableChanged(bool) ), this, SLOT( documentCutAvailableChanged(bool) ) );
        disconnect( document, SIGNAL( copyAvailableChanged(bool) ), this, SLOT( documentCopyAvailableChanged(bool) ) );
        disconnect( document, SIGNAL( pasteAvailableChanged(bool) ),
                    this, SLOT( documentPasteAvailableChanged(bool) ) );
        disconnect( document, SIGNAL( undoAvailableChanged(bool) ), this, SLOT( documentUndoAvailableChanged(bool) ) );
        disconnect( document, SIGNAL( redoAvailableChanged(bool) ), this, SLOT( documentRedoAvailableChanged(bool) ) );
        disconnect( document, SIGNAL( editModeChanged(BCodeEdit::EditMode) ),
                    this, SLOT( documentEditModeChanged(BCodeEdit::EditMode) ) );
        disconnect( document, SIGNAL( cursorPositionChanged(QPoint) ),
                    this, SLOT( documentCursorPositionChanged(QPoint) ) );
        disconnect( document, SIGNAL( buisyChanged(bool) ), this, SLOT( documentBuisyChanged(bool) ) );
        disconnect( document, SIGNAL( lineSplitted(BCodeEdit::SplittedLinesRange) ),
                    this, SLOT( documentLineSplitted(BCodeEdit::SplittedLinesRange) ) );
        disconnect( document, SIGNAL( linesSplitted(QList<BCodeEdit::SplittedLinesRange>) ),
                    this, SLOT( documentLinesSplitted(QList<BCodeEdit::SplittedLinesRange>) ) );
        disconnect( document, SIGNAL( fileNameChanged(QString) ), this, SLOT( documentFileNameChanged(QString) ) );
        disconnect( document, SIGNAL( codecChanged(QString) ), this, SLOT( documentCodecChanged(QString) ) );
    }
    document = doc;
    if (document)
    {
        connect( document, SIGNAL( readOnlyChanged(bool) ), this, SLOT( documentReadOnlyChanged(bool) ) );
        connect( document, SIGNAL( modificationChanged(bool) ), this, SLOT( documentModificationChanged(bool) ) );
        connect( document, SIGNAL( selectionChanged() ), this, SLOT( documentSelectionChanged() ) );
        connect( document, SIGNAL( hasSelectionChanged(bool) ), this, SLOT( documentHasSelectionChanged(bool) ) );
        connect( document, SIGNAL( cutAvailableChanged(bool) ), this, SLOT( documentCutAvailableChanged(bool) ) );
        connect( document, SIGNAL( copyAvailableChanged(bool) ), this, SLOT( documentCopyAvailableChanged(bool) ) );
        connect( document, SIGNAL( pasteAvailableChanged(bool) ), this, SLOT( documentPasteAvailableChanged(bool) ) );
        connect( document, SIGNAL( undoAvailableChanged(bool) ), this, SLOT( documentUndoAvailableChanged(bool) ) );
        connect( document, SIGNAL( redoAvailableChanged(bool) ), this, SLOT( documentRedoAvailableChanged(bool) ) );
        connect( document, SIGNAL( editModeChanged(BCodeEdit::EditMode) ),
                 this, SLOT( documentEditModeChanged(BCodeEdit::EditMode) ) );
        connect( document, SIGNAL( cursorPositionChanged(QPoint) ),
                 this, SLOT( documentCursorPositionChanged(QPoint) ) );
        connect( document, SIGNAL( buisyChanged(bool) ), this, SLOT( documentBuisyChanged(bool) ) );
        connect( document, SIGNAL( lineSplitted(BCodeEdit::SplittedLinesRange) ),
                 this, SLOT( documentLineSplitted(BCodeEdit::SplittedLinesRange) ) );
        connect( document, SIGNAL( linesSplitted(QList<BCodeEdit::SplittedLinesRange>) ),
                 this, SLOT( documentLinesSplitted(QList<BCodeEdit::SplittedLinesRange>) ) );
        connect( document, SIGNAL( fileNameChanged(QString) ), this, SLOT( documentFileNameChanged(QString) ) );
        connect( document, SIGNAL( codecChanged(QString) ), this, SLOT( documentCodecChanged(QString) ) );
    }
    emitCurrentDocumentChanged(document);
}

void BCodeEditorPrivate::twgtTabCloseRequested(int index)
{
    BCodeEditorDocument *doc = (index >= 0) ? static_cast<BCodeEditorDocument *>( twgt->widget(index) ) : 0;
    if (doc)
        closeDocument(doc);
}

//BCodeEdit events

void BCodeEditorPrivate::documentReadOnlyChanged(bool ro)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentReadOnlyChanged(ro);
}

void BCodeEditorPrivate::documentModificationChanged(bool modified)
{
    updateDocumentTab(document);
    foreach (BAbstractEditorModule *module, modules)
        module->documentModificationChanged(modified);
}

void BCodeEditorPrivate::documentSelectionChanged()
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentSelectionChanged();
}

void BCodeEditorPrivate::documentHasSelectionChanged(bool hasSelection)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentHasSelectionChanged(hasSelection);
}

void BCodeEditorPrivate::documentCutAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCutAvailableChanged(available);
}

void BCodeEditorPrivate::documentCopyAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCopyAvailableChanged(available);
}

void BCodeEditorPrivate::documentPasteAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentPasteAvailableChanged(available);
}

void BCodeEditorPrivate::documentUndoAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentUndoAvailableChanged(available);
}

void BCodeEditorPrivate::documentRedoAvailableChanged(bool available)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentRedoAvailableChanged(available);
}

void BCodeEditorPrivate::documentEditModeChanged(BCodeEdit::EditMode mode)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentEditModeChanged(mode);
}

void BCodeEditorPrivate::documentCursorPositionChanged(const QPoint &pos)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCursorPositionChanged(pos);
}

void BCodeEditorPrivate::documentBuisyChanged(bool buisy)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentBuisyChanged(buisy);
}

void BCodeEditorPrivate::documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentLineSplitted(linesRange);
}

void BCodeEditorPrivate::documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentLinesSplitted(linesRanges);
}

//BCodeEditorDocument events

void BCodeEditorPrivate::documentFileNameChanged(const QString &fn)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentFileNameChanged(fn);
}

void BCodeEditorPrivate::documentCodecChanged(const QString &codecName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCodecChanged(codecName);
}

void BCodeEditorPrivate::documentLoadingFinished(bool success)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if ( !doc || !openingDocuments.contains(doc) )
        return;
    QString fn = openingDocuments.take(doc);
    if (!success)
    {
        doc->deleteLater();
        failedToOpenMessage(fn);
        return;
    }
    addDocument(doc);
}

void BCodeEditorPrivate::documentSavingFinished(bool success)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if ( !doc || !savingDocuments.contains(doc) )
        return;
    QString fn = savingDocuments.take(doc);
    if (!success)
        failedToSaveMessage( !fn.isEmpty() ? fn : doc->fileName() );
    if ( closingDocuments.contains(doc) )
    {
        closingDocuments.remove(doc);
        removeDocument(doc);
        if ( closingAll && closingDocuments.isEmpty() )
        {
            closingAll = false;
            emitAllDocumentsClosed(success);
        }
    }
}

/*============================================================================
================================ Code Editor
============================================================================*/

BAbstractEditorModule *BCodeEditor::createStandardModule(StandardModule type, BCodeEditor *parent)
{
    BAbstractEditorModule *mdl = 0;
    switch (type)
    {
    case BookmarksModule:
        mdl = new BBookmarksEditorModule(parent);
    case SearchModule:
        mdl = new BSearchEditorModule(parent);
        break;
    default:
        break;
    }
    if (parent && mdl)
        parent->addModule(mdl);
    return mdl;
}

//

BCodeEditor::BCodeEditor(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
    createStandardModule(SearchModule, this);
    createStandardModule(BookmarksModule, this); //Just for test, remove, but add another modules in the future
}

BCodeEditor::BCodeEditor(const QList<BAbstractEditorModule *> &moduleList, QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
    setModules(moduleList);
}

BCodeEditor::~BCodeEditor()
{
    //
}

//

void BCodeEditor::setEditFont(const QFont &fnt)
{
    if ( !QFontInfo(fnt).fixedPitch() )
        return;
    if (fnt.pointSize() < 1 && fnt.pixelSize() < 1)
        return;
    B_D(BCodeEditor);
    if (fnt == d->editFont)
        return;
    d->editFont = fnt;
    foreach ( BCodeEditorDocument *doc, documents() )
        doc->setEditFont(fnt);
}

void BCodeEditor::setEditMode(BCodeEdit::EditMode mode)
{
    B_D(BCodeEditor);
    if (d->editMode == mode)
        return;
    d->editMode = mode;
    foreach ( BCodeEditorDocument *doc, documents() )
        doc->setEditMode(mode);
}

void BCodeEditor::setEditLineLength(int ll)
{
    if (ll < 1)
        return;
    B_D(BCodeEditor);
    if (d->editLineLength == ll)
        return;
    foreach ( BCodeEditorDocument *doc, documents() )
        doc->setEditLineLength(ll);
}

void BCodeEditor::setEditTabWidth(BCodeEdit::TabWidth tw)
{
    B_D(BCodeEditor);
    if (d->editTabWidth == tw)
        return;
    foreach ( BCodeEditorDocument *doc, documents() )
        doc->setEditTabWidth(tw);
}

void BCodeEditor::setBracketHighlightingEnabled(bool enabled)
{
    B_D(BCodeEditor);
    if (enabled == d->bracketsHighlighting)
        return;
    d->bracketsHighlighting = enabled;
    foreach ( BCodeEditorDocument *doc, documents() )
        doc->setBracketHighlightingEnabled(enabled);
}

void BCodeEditor::addModule(BAbstractEditorModule *mdl)
{
    if ( !mdl || mdl->isBuisy() )
        return;
    B_D(BCodeEditor);
    if ( d->modules.contains( mdl->name() ) )
        return;
    d->modules.insert(mdl->name(), mdl);
    d->setModuleEditor(mdl, this);
}

void BCodeEditor::addModule(StandardModule type)
{
    BAbstractEditorModule *mdl = createStandardModule(type, this);
    if (mdl->editor() != this)
        mdl->deleteLater();
}

void BCodeEditor::removeModule(BAbstractEditorModule *mdl)
{
    if (!mdl)
        return;
    removeModule( mdl->name() );
}

void BCodeEditor::removeModule(const QString &name)
{
    if ( name.isEmpty() )
        return;
    B_D(BCodeEditor);
    BAbstractEditorModule *mdl = d->modules.value(name);
    if ( !mdl || mdl->isBuisy() )
        return;
    d->modules.remove(name);
    d->setModuleEditor(mdl, 0);
}

void BCodeEditor::setModules(const QList<BAbstractEditorModule *> &list)
{
    B_D(BCodeEditor);
    foreach ( const QString &name, d->modules.keys() )
        removeModule(name);
    foreach (BAbstractEditorModule *mdl, list)
        addModule(mdl);
}

void BCodeEditor::setDriver(BAbstractDocumentDriver *drv)
{
    if (!drv)
        return;
    B_D(BCodeEditor);
    if ( d->driver && ( d->driver->hasPendingLoadOperations() || d->driver->hasPendingSaveOperations() ) )
        return;
    if (d->driver && d->driver->parent() == (QObject *) this)
        d->driver->deleteLater();
    d->driver = drv;
    if ( drv && !drv->parent() )
        drv->setParent(this);
}

bool BCodeEditor::waitForAllDocumentsClosed(int msecs)
{
    B_D(BCodeEditor);
    if (!d->closingAll)
        return true;
    if (msecs <= 0)
        return false;
    QEventLoop el;
    QTimer::singleShot( msecs, &el, SLOT( quit() ) );
    connect( this, SIGNAL( allDocumentsClosed() ), &el, SLOT( quit() ) );
    el.exec();
    return !d->closingAll;
}

QFont BCodeEditor::editFont() const
{
    return d_func()->editFont;
}

BCodeEdit::EditMode BCodeEditor::editMode() const
{
    return d_func()->editMode;
}

int BCodeEditor::editLineLength() const
{
    return d_func()->editLineLength;
}

BCodeEdit::TabWidth BCodeEditor::editTabWidth() const
{
    return d_func()->editTabWidth;
}

bool BCodeEditor::isBracketHighlightingEnabled() const
{
    return d_func()->bracketsHighlighting;
}

BAbstractEditorModule *BCodeEditor::module(const QString &name) const
{
    return d_func()->modules.value(name);
}

QList<BAbstractEditorModule *> BCodeEditor::modules() const
{
    return d_func()->modules.values();
}

BCodeEditorDocument *BCodeEditor::currentDocument() const
{
    return d_func()->document;
}

QList<BCodeEditorDocument *> BCodeEditor::documents() const
{
    const B_D(BCodeEditor);
    QList<BCodeEditorDocument *> list;
    for (int i = 0; i < d->twgt->count(); ++i)
        list << static_cast<BCodeEditorDocument *>( d->twgt->widget(i) );
    return list;
}

BAbstractDocumentDriver * BCodeEditor::driver() const
{
    return d_func()->driver;
}

bool BCodeEditor::documentAvailable() const
{
    return currentDocument();
}

QString BCodeEditor::currentFileName() const
{
    BCodeEditorDocument *doc = currentDocument();
    return doc ? doc->fileName() : QString();
}

QStringList BCodeEditor::fileNames() const
{
    QStringList list;
    foreach ( BCodeEditorDocument *doc, documents() )
        if (doc)
            list << doc->fileName();
    list.removeDuplicates();
    return list;
}

//

bool BCodeEditor::addDocument(const QString &fileName)
{
    return addDocument(fileName, "");
}

bool BCodeEditor::addDocument(const QString &fileName, const QString &text)
{
    B_D(BCodeEditor);
    if ( d->findDocument( BCodeEditorPrivate::createFileName(fileName) ) )
        return false;
    d->addDocument( d->createDocument(fileName, text) );
    return true;
}

bool BCodeEditor::openDocument(const QString &fileName)
{
    if ( fileName.isEmpty() )
        return false;
    B_D(BCodeEditor);
    if ( d->findDocument(fileName) )
        return false;
    BCodeEditorDocument *doc = d->createDocument(fileName);
    d->openingDocuments.insert(doc, fileName);
    bool b = doc->load(driver(), fileName);
    if (!b)
    {
        d->openingDocuments.remove(doc);
        doc->deleteLater();
        d->failedToOpenMessage(fileName);
    }
    return b;
}

bool BCodeEditor::saveCurrentDocument()
{
    return d_func()->saveDocument( currentDocument() );
}

bool BCodeEditor::saveCurrentDocumentAs(const QString &newFileName)
{
    if ( newFileName.isEmpty() )
        return false;
    return d_func()->saveDocument(currentDocument(), newFileName);
}

bool BCodeEditor::saveAllDocuments()
{
    foreach ( BCodeEditorDocument *doc, documents() )
        if ( doc->isModified() && !doc->isReadOnly() && !d_func()->saveDocument(doc) )
            return false;
    return true;
}

bool BCodeEditor::closeCurrentDocument()
{
    return d_func()->closeDocument( currentDocument() );
}

bool BCodeEditor::closeAllDocuments()
{
    return d_func()->closeAllDocuments();
}

//

BCodeEditor::BCodeEditor(BCodeEditorPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

//

bool BCodeEditor::shouldSaveAs(const QString &fileName, QString &newFileName)
{
    //TODO: Add possibility to choose local file name
    return !QFileInfo(fileName).exists();
}
