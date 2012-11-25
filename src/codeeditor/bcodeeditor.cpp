#include "bcodeeditor.h"
#include "bcodeeditor_p.h"
#include "bcodeeditordocument.h"
#include "babstracteditormodule.h"
#include "blocaldocumentdriver.h"
#include "babstracteditormodule_p.h"
#include "bbookmarkseditormodule.h"
#include "bsearcheditormodule.h"
#include "babstractfiletype.h"
#include "bindicatorseditormodule.h"
#include "babstractdocumentdriver_p.h"
#include "bopensaveeditormodule.h"
#include "bediteditormodule.h"
#include "bcodeedit_p.h"

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
#include <QTextCodec>
#include <QByteArray>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QUrl>
#include <QMimeData>

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
================================ Drop Handler
============================================================================*/

BDropHandler::BDropHandler(BCodeEditorPrivate *parent) :
    QObject(parent), Editor(parent)
{
    //
}

BDropHandler::~BDropHandler()
{
    //
}

//

bool BDropHandler::eventFilter(QObject *o, QEvent *e)
{
    if (!Editor)
        return QObject::eventFilter(o, e);
    switch ( e->type() )
    {
    case QEvent::DragEnter:
        static_cast<QDragEnterEvent *>(e)->acceptProposedAction();
        return true;
    case QEvent::DragMove:
        static_cast<QDragMoveEvent *>(e)->acceptProposedAction();
        return true;
    case QEvent::DragLeave:
        static_cast<QDragLeaveEvent *>(e)->accept();
        return true;
    case QEvent::Drop:
    {
        QStringList list;
        foreach ( const QUrl &url, static_cast<QDropEvent *>(e)->mimeData()->urls() )
            list << url.toLocalFile();
        Editor->q_func()->openDocuments(list);
        return true;
    }
    default:
        return QObject::eventFilter(o, e);
    }
}

/*============================================================================
================================ Code Editor Private
============================================================================*/

QString BCodeEditorPrivate::defaultFileName()
{
    return trq("New document", "fileName");
}

QString BCodeEditorPrivate::createFileName(const QString &fileName, const QString &defaultName,
                                           const QStringList &existingNames)
{
    if ( !fileName.isEmpty() )
        return fileName;
    QString fnbase = !defaultName.isEmpty() ? defaultName : defaultFileName();
    int i = 1;
    QString fn = fnbase + " " + QString::number(i++);
    while ( existingNames.contains(fn) )
        fn = fnbase + " " + QString::number(i++);
    return fn;
}

QMap<QTextCodec *, QString> BCodeEditorPrivate::createCodecNamesMap()
{
    QMap<QTextCodec *, QString> m;
    foreach (const QString &cn, SupportedCodecs)
        m.insert(QTextCodec::codecForName( cn.toLatin1() ), cn);
    return m;
}

QString BCodeEditorPrivate::codecDescriptiveName(const QString &codecName)
{
    if ( UnicodeCodecs.contains(codecName) )
    {
        return trq("Unicode", "codec descriptiveName");
    }
    else if ( EastEuropeanCodecs.contains(codecName) )
    {
        if ( (QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257").contains(codecName,
                                                                                         Qt::CaseInsensitive) )
            return trq("Baltic", "codec descriptiveName");
        else if ( (QStringList() << "ISO 8859-5" << "KOI8-R" << "Windows-1251").contains(codecName,
                                                                                         Qt::CaseInsensitive) )
            return trq("Cyrillic", "codec descriptiveName");
        else if ( !codecName.compare("KOI8-U", Qt::CaseInsensitive) )
            return trq("Cyrillic (Ukrainian)", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-16", Qt::CaseInsensitive) )
            return trq("Romanic", "codec descriptiveName");
        else if ( (QStringList() << "ISO 8859-2" << "Windows-1250").contains(codecName, Qt::CaseInsensitive) )
            return trq("Central European", "codec descriptiveName");
    }
    else if ( WestEuropeanCodecs.contains(codecName) )
    {
        if ( (QStringList() << "ISO 8859-7" << "Windows-1253").contains(codecName, Qt::CaseInsensitive) )
            return trq("Greek", "codec descriptiveName");
        else if ( (QStringList() << "IBM 850" << "ISO 8859-1" << "ISO 8859-15"
                   << "Apple Roman" << "Windows-1252").contains(codecName, Qt::CaseInsensitive) )
            return trq("Western", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-14", Qt::CaseInsensitive) )
            return trq("Celtic", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-10", Qt::CaseInsensitive) )
            return trq("Nordic", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-3", Qt::CaseInsensitive) )
            return trq("South European", "codec descriptiveName");
    }
    else if ( EastAsianCodecs.contains(codecName) )
    {
        if ( !codecName.compare("Windows-1258", Qt::CaseInsensitive) )
            return trq("Vietnamese", "codec descriptiveName");
        else if ( (QStringList() << "Big5" << "Big5-HKSCS").contains(codecName, Qt::CaseInsensitive) )
            return trq("Traditional Chinese", "codec descriptiveName");
        else if ( !codecName.compare("GB18030-0", Qt::CaseInsensitive) )
            return trq("Simplified Chinese", "codec descriptiveName");
        else if ( (QStringList() << "EUC-KR" << "JOHAB").contains(codecName, Qt::CaseInsensitive) )
            return trq("Korean", "codec descriptiveName");
        else if ( (QStringList() << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS").contains(codecName,
                                                                                       Qt::CaseInsensitive) )
            return trq("Japanese", "codec descriptiveName");
    }
    else if ( SouthEastSouthWestAsianCodecs.contains(codecName) )
    {
        if ( !codecName.compare("TIS-620", Qt::CaseInsensitive) )
            return trq("Thai", "codec descriptiveName");
        else if ( (QStringList() << "ISO 8859-9" << "Windows-1254").contains(codecName, Qt::CaseInsensitive) )
            return trq("Turkish", "codec descriptiveName");
    }
    else if ( MiddleEastCodecs.contains(codecName) )
    {
        if ( (QStringList() << "ISO 8859-6" << "Windows-1256").contains(codecName, Qt::CaseInsensitive) )
            return trq("Arabic", "codec descriptiveName");
        else if ( !codecName.compare("Windows-1255", Qt::CaseInsensitive) )
            return trq("Hebrew", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-8", Qt::CaseInsensitive) )
            return trq("Hebrew (visual)", "codec descriptiveName");
    }
    return "";
}



//

BCodeEditorPrivate::BCodeEditorPrivate(BCodeEditor *q) :
    BBasePrivate(q)
{
    //
}

BCodeEditorPrivate::~BCodeEditorPrivate()
{
    foreach (BAbstractFileType *ft, fileTypes)
        delete ft;
    delete defaultFileType;
}

//

void BCodeEditorPrivate::init()
{
    B_Q(BCodeEditor);
    document = 0;
    editFont = BApplication::createMonospaceFont();
    editMode = BCodeEdit::NormalMode;
    editLineLength = 120;
    editTabWidth = BCodeEdit::TabWidth4;
    bracketsHighlighting = true;
    driver = 0;
    q->setDriver(new BLocalDocumentDriver);
    defaultFileType = BAbstractFileType::defaultFileType();
    defaultCodec = QTextCodec::codecForName("UTF-8");
    defaultFN = defaultFileName();
    //
    vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      twgt = new QTabWidget(q);
        twgt->setMovable(true);
        twgt->setTabsClosable(true);
        connect( twgt, SIGNAL( currentChanged(int) ), this, SLOT( twgtCurrentChanged(int) ) );
        connect( twgt, SIGNAL( tabCloseRequested(int) ), this, SLOT( twgtTabCloseRequested(int) ) );
      vlt->addWidget(twgt);
   //
   createDropHandler();
}

bool BCodeEditorPrivate::tryAddFileType(BAbstractFileType *ft)
{
    if ( !ft || ft->id() == defaultFileType->id() || fileTypes.contains( ft->id() ) )
        return false;
    fileTypes.insert(ft->id(), ft);
    return true;
}

bool BCodeEditorPrivate::tryRemoveFileType(const QString &id)
{
    if ( id.isEmpty() )
        return false;
    BAbstractFileType *ft = fileTypes.take(id);
    if (!ft)
        return false;
    foreach ( BCodeEditorDocument *doc, q_func()->documents() )
        if (doc->fileType() == ft)
            doc->setFileType( selectDocumentFileType(doc) );
    delete ft;
    return true;
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
    doc->innerEdit()->viewport()->installEventFilter(dropHandler);
    doc->setFileName( createFileName( fileName, defaultFN, q_func()->fileNames() ) );
    if ( !text.isEmpty() )
        doc->setText(text);
    doc->setEditFont(editFont);
    doc->setEditMode(editMode);
    doc->setEditLineLength(editLineLength);
    doc->setEditTabWidth(editTabWidth);
    doc->setBracketHighlightingEnabled(bracketsHighlighting);
    doc->setCodec(defaultCodec);
    doc->setFileType( selectDocumentFileType(doc) );
    //
    connect( doc, SIGNAL( fileNameChanged(QString) ), this, SLOT( documentFileNameChanged(QString) ) );
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

BAbstractFileType *BCodeEditorPrivate::selectDocumentFileType(BCodeEditorDocument *doc)
{
    if (!doc)
        return 0;
    QString fn = doc->fileName();
    foreach (BAbstractFileType *ft, fileTypes)
        if ( ft->matchesFileName(fn) )
            return ft;
    return defaultFileType;
}

bool BCodeEditorPrivate::openDocument(const QString &fileName, QTextCodec *codec)
{
    if ( fileName.isEmpty() || findDocument(fileName) )
        return false;
    BCodeEditorDocument *doc = createDocument(fileName);
    openingDocuments.insert(doc, fileName);
    bool b = doc->load(driver, codec, fileName);
    if (!b)
    {
        openingDocuments.remove(doc);
        doc->deleteLater();
        failedToOpenMessage(fileName);
    }
    return b;
}

bool BCodeEditorPrivate::reopenDocument(BCodeEditorDocument *doc, QTextCodec *codec)
{
    if ( !doc || doc->isBuisy() )
        return false;
    if ( doc->isModified() && reopenModifiedMessage( doc->fileName() ) != QMessageBox::Yes)
        return false;
    bool b = doc->load(driver, codec);
    if (!b)
        failedToOpenMessage( doc->fileName() );
    return b;
}

bool BCodeEditorPrivate::saveDocument(BCodeEditorDocument *doc, const QString &newFileName, QTextCodec *codec)
{
    if ( !doc || savingDocuments.contains(doc) )
        return false;
    QString nfn = newFileName;
    bool ssa = !nfn.isEmpty() || !driver->checkFileExistance( doc->fileName() );
    if (ssa)
    {
        if (!codec)
            codec = doc->codec();
        bool b = !nfn.isEmpty() || driver->getSaveAsFileName(q_func(), doc->fileName(), nfn, codec);
        if ( !b || nfn.isEmpty() || findDocument(nfn) )
            return false;
    }
    else
    {
        if ( !doc->isModified() )
            return true;
        else if ( doc->isReadOnly() )
            return false;
    }
    savingDocuments.insert(doc, nfn);
    bool b = doc->save(driver, codec, nfn);
    if (!b)
    {
        savingDocuments.remove(doc);
        failedToSaveMessage(doc->fileName(), nfn);
    }
    return b;
}

bool BCodeEditorPrivate::saveDocuments(const QList<BCodeEditorDocument *> &list)
{
    if ( list.isEmpty() )
        return true;
    foreach (BCodeEditorDocument *doc, list)
        if ( !saveDocument(doc) )
            return false;
    return true;
}

bool BCodeEditorPrivate::closeDocument(BCodeEditorDocument *doc)
{
    if ( !doc || openingDocuments.contains(doc) || savingDocuments.contains(doc) )
        return false;
    if ( doc->isModified() )
    {
        switch ( closeModifiedMessage( doc->fileName() ) )
        {
        case QMessageBox::Save:
            return tryCloseDocument(doc);
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
    QList<BCodeEditorDocument *> list = q_func()->documents();
    if ( list.isEmpty() )
        return true;
    QList<BCodeEditorDocument *> nslist;
    for (int i = list.size() - 1; i >= 0; --i)
    {
        BCodeEditorDocument *doc = list.at(i);
        if ( openingDocuments.contains(doc) || savingDocuments.contains(doc) )
        {
            list.removeAt(i);
            continue;
        }
        if ( doc->isModified() )
        {
            list.removeAt(i);
            nslist << doc;
        }
    }
    if ( !nslist.isEmpty() )
    {
        if ( nslist.size() == 1 && !closeDocument( nslist.first() ) )
            return false;
        BSelectDocumentsDialog sdlg(nslist);
        sdlg.exec();
        switch ( sdlg.decision() )
        {
        case QMessageBox::Save:
        {
            QList<BCodeEditorDocument *> selected = sdlg.selectedDocuments();
            for (int i = nslist.size() - 1; i >= 0; --i)
            {
                BCodeEditorDocument *doc = nslist.at(i);
                if ( !selected.contains(doc) )
                {
                    nslist.removeAt(i);
                    list << doc;
                }
            }
            foreach (BCodeEditorDocument *doc, nslist)
                if ( !tryCloseDocument(doc) )
                    return false;
            break;
        }
        case QMessageBox::Discard:
            foreach (BCodeEditorDocument *doc, nslist)
                list << doc;
            break;
        case QMessageBox::Cancel:
        default:
            return false;
        }
    }
    foreach (BCodeEditorDocument *doc, list)
        removeDocument(doc);
    return true;
}

bool BCodeEditorPrivate::tryCloseDocument(BCodeEditorDocument *doc)
{
    if ( !doc || openingDocuments.contains(doc) || savingDocuments.contains(doc) || closingDocuments.contains(doc) )
        return false;
    closingDocuments << doc;
    if ( saveDocument(doc) )
    {
        return true;
    }
    else
    {
        closingDocuments.removeAll(doc);
        return false;
    }
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

int BCodeEditorPrivate::reopenModifiedMessage(const QString &fileName)
{
    QMessageBox msg( q_func() );
    msg.setWindowTitle( trq("Reopening modified document", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Question);
    msg.setText(trq("Document is modified:", "msgbox text") + "\n" + fileName);
    msg.setInformativeText( trq("Do you want to reopen it anyway?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec();
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

void BCodeEditorPrivate::emitDefaultCodecChanged(const QString &codecName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->defaultCodecChanged(codecName);
    QMetaObject::invokeMethod( q_func(), "defaultCodecChanged", Q_ARG(QString, codecName) );
}

void BCodeEditorPrivate::emitEditModeChanged(BCodeEdit::EditMode mode)
{
    foreach (BAbstractEditorModule *mdl, modules)
        mdl->editModeChanged(mode);
    QMetaObject::invokeMethod( q_func(), "editModeChanged", Q_ARG(BCodeEdit::EditMode, mode) );
}

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

void BCodeEditorPrivate::emitFileTypesChanged()
{
    foreach (BAbstractEditorModule *module, modules)
        module->fileTypesChanged();
    QMetaObject::invokeMethod(q_func(), "fileTypesChanged");
}

//External private class call

void BCodeEditorPrivate::setModuleEditor(BAbstractEditorModule *mdl, BCodeEditor *edr)
{
    if (!mdl)
        return;
    mdl->d_func()->setEditor(edr);
}

void BCodeEditorPrivate::setDriverEditor(BAbstractDocumentDriver *drv, BCodeEditor *edr)
{
    if (!drv)
        return;
    drv->d_func()->setEditor(edr);
}

//

const QStringList BCodeEditorPrivate::UnicodeCodecs = QStringList() << "UTF-16" << "UTF-8";
const QStringList BCodeEditorPrivate::EastEuropeanCodecs = QStringList() << "ISO 8859-13" << "ISO 8859-4"
    << "Windows-1257" << "ISO 8859-5" << "KOI8-R" << "Windows-1251" << "KOI8-U" << "ISO 8859-16" << "ISO 8859-2"
    << "Windows-1250";
const QStringList BCodeEditorPrivate::WestEuropeanCodecs = QStringList() << "ISO 8859-7" << "Windows-1253" << "IBM 850"
    << "ISO 8859-1" << "ISO 8859-15" << "Apple Roman" << "Windows-1252" << "ISO 8859-14" << "ISO 8859-10"
    << "ISO 8859-3";
const QStringList BCodeEditorPrivate::EastAsianCodecs = QStringList() << "Windows-1258" << "Big5" << "Big5-HKSCS"
    << "GB18030-0" << "EUC-KR" << "JOHAB" << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS";
const QStringList BCodeEditorPrivate::SouthEastSouthWestAsianCodecs = QStringList() << "TIS-620" << "ISO 8859-9"
    << "Windows-1254";
const QStringList BCodeEditorPrivate::MiddleEastCodecs = QStringList() << "ISO 8859-6" << "Windows-1256"
    << "Windows-1255" << "ISO 8859-8";
const QStringList BCodeEditorPrivate::SupportedCodecs = QStringList() << BCodeEditorPrivate::UnicodeCodecs
    << BCodeEditorPrivate::EastEuropeanCodecs << BCodeEditorPrivate::WestEuropeanCodecs
    << BCodeEditorPrivate::EastAsianCodecs << BCodeEditorPrivate::SouthEastSouthWestAsianCodecs
    << BCodeEditorPrivate::MiddleEastCodecs;
const QMap<QTextCodec *, QString> BCodeEditorPrivate::CodecNames = BCodeEditorPrivate::createCodecNamesMap();

//

void BCodeEditorPrivate::createDropHandler()
{
    dropHandler = new BDropHandler(this);
    foreach ( BCodeEditorDocument *doc, q_func()->documents() )
        doc->innerEdit()->viewport()->installEventFilter(dropHandler);
    connect( dropHandler, SIGNAL( destroyed() ), this, SLOT( createDropHandler() ) );
}

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
        disconnect( document, SIGNAL( codecChanged(QString) ), this, SLOT( documentCodecChanged(QString) ) );
        disconnect( document, SIGNAL( fileTypeChanged(BAbstractFileType *) ),
                    this, SLOT( documentFileTypeChanged(BAbstractFileType *) ) );
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
        connect( document, SIGNAL( codecChanged(QString) ), this, SLOT( documentCodecChanged(QString) ) );
        connect( document, SIGNAL( fileTypeChanged(BAbstractFileType *) ),
                 this, SLOT( documentFileTypeChanged(BAbstractFileType *) ) );
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
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if (!doc)
        return;
    if (buisy)
    {
        if ( !processedDocuments.contains(doc) )
            processedDocuments << doc;
    }
    else
    {
        processedDocuments.removeAll(doc);
        if ( processedDocuments.isEmpty() )
            QMetaObject::invokeMethod(q_func(), "allDocumentsProcessed");
    }
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
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if (!doc)
        return;
    updateDocumentTab(doc);
    if (doc == document)
    {
        foreach (BAbstractEditorModule *module, modules)
            module->documentFileNameChanged(fn);
    }
}

void BCodeEditorPrivate::documentCodecChanged(const QString &codecName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentCodecChanged(codecName);
}

void BCodeEditorPrivate::documentFileTypeChanged(BAbstractFileType *ft)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentFileTypeChanged(ft);
}

void BCodeEditorPrivate::documentLoadingFinished(bool success)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if (!doc)
        return;
    QString fn = doc->fileName();
    if ( openingDocuments.contains(doc) )
    {
        fn = openingDocuments.take(doc);
        if (success)
            addDocument(doc);
        else
            doc->deleteLater();
    }
    if (!success)
        failedToOpenMessage(fn);
}

void BCodeEditorPrivate::documentSavingFinished(bool success)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if ( !doc || !savingDocuments.contains(doc) )
        return;
    QString fn = savingDocuments.take(doc);
    if (!success)
        failedToSaveMessage(doc->fileName(), fn);
    if ( closingDocuments.contains(doc) )
    {
        closingDocuments.removeAll(doc);
        removeDocument(doc);
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

bool BCodeEditor::supportsCodec(QTextCodec *codec)
{
    return codec && BCodeEditorPrivate::CodecNames.contains(codec);
}

bool BCodeEditor::supportsCodec(const QString &codecName)
{
    return !codecName.isEmpty() && supportsCodec( QTextCodec::codecForName( codecName.toLatin1() ) );
}

QList<QTextCodec *> BCodeEditor::supportedCodecs()
{
    return BCodeEditorPrivate::CodecNames.keys();
}

QStringList BCodeEditor::supportedCodecNames()
{
    return BCodeEditorPrivate::SupportedCodecs;
}

QString BCodeEditor::codecName(QTextCodec *codec)
{
    return BCodeEditorPrivate::CodecNames.value(codec);
}

QString BCodeEditor::fullCodecName(QTextCodec *codec)
{
    if ( !codec || !BCodeEditorPrivate::CodecNames.contains(codec) )
        return "";
    QString cn = BCodeEditorPrivate::CodecNames.value(codec);
    return BCodeEditorPrivate::codecDescriptiveName(cn) + " (" + cn + ")";
}

QString BCodeEditor::fullCodecName(const QString &codecName)
{
    return !codecName.isEmpty() ? fullCodecName( QTextCodec::codecForName( codecName.toLatin1() ) ) : QString();
}

QString BCodeEditor::codecGroupName(CodecGroup group)
{
    switch (group)
    {
    case UnicodeGroup:
        return tr("Unicode", "encodingGroup name");
    case EastEuropeanGroup:
        return tr("Eastern Europe", "encodingGroup name");
    case WestEuropeanGroup:
        return tr("Western Europe", "encodingGroup name");
    case EastAsianGroup:
        return tr("East Asia", "encodingGroup name");
    case SouthEastSouthWestAsianGroup:
        return tr("South-East and South-West Asia", "encodingGroup name");
    case MiddleEastGroup:
        return tr("Middle East", "encodingGroup name");
    default:
        return "";
    }
}

QList<QTextCodec *> BCodeEditor::codecsForGroup(CodecGroup group)
{
    QList<QTextCodec *> list;
    foreach ( const QString &name, codecNamesForGroup(group) )
        list << QTextCodec::codecForName( name.toLatin1() );
    return list;
}

QStringList BCodeEditor::codecNamesForGroup(CodecGroup group)
{
    switch (group)
    {
    case UnicodeGroup:
        return BCodeEditorPrivate::UnicodeCodecs;
    case EastEuropeanGroup:
        return BCodeEditorPrivate::EastEuropeanCodecs;
    case WestEuropeanGroup:
        return BCodeEditorPrivate::WestEuropeanCodecs;
    case EastAsianGroup:
        return BCodeEditorPrivate::EastAsianCodecs;
    case SouthEastSouthWestAsianGroup:
        return BCodeEditorPrivate::SouthEastSouthWestAsianCodecs;
    case MiddleEastGroup:
        return BCodeEditorPrivate::MiddleEastCodecs;
    default:
        return QStringList();
    }
}

//

BCodeEditor::BCodeEditor(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
    addModule(EditModule);
    addModule(IndicatorsModule);
    addModule(OpenSaveModule);
    addModule(SearchModule);
}

BCodeEditor::BCodeEditor(const QList<BAbstractFileType *> &fileTypes, QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
    foreach (BAbstractFileType *ft, fileTypes)
        d_func()->tryAddFileType(ft);
    addModule(EditModule);
    addModule(IndicatorsModule);
    addModule(OpenSaveModule);
    addModule(SearchModule);
}

BCodeEditor::BCodeEditor(const QList<BAbstractEditorModule *> &moduleList, QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
    setModules(moduleList);
}

BCodeEditor::BCodeEditor(const QList<BAbstractFileType *> &fileTypes,
                         const QList<BAbstractEditorModule *> &moduleList, QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
    foreach (BAbstractFileType *ft, fileTypes)
        d_func()->tryAddFileType(ft);
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
    d->emitEditModeChanged(mode);
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

void BCodeEditor::setDefaultCodec(QTextCodec *codec)
{
    if ( !codec || !supportsCodec(codec) )
        return;
    d_func()->defaultCodec = codec;
    d_func()->emitDefaultCodecChanged( codecName(codec) );
}

void BCodeEditor::setDefaultCodec(const QString &codecName)
{
    if ( codecName.isEmpty() )
        return;
    setDefaultCodec( QTextCodec::codecForName( codecName.toLatin1() ) );
}

void BCodeEditor::setDefaultFileName(const QString &fileName)
{
    d_func()->defaultFN = !fileName.isEmpty() ? fileName : BCodeEditorPrivate::defaultFileName();
}

void BCodeEditor::addModule(BAbstractEditorModule *mdl)
{
    if ( !mdl || mdl->isBuisy() )
        return;
    B_D(BCodeEditor);
    if ( d->modules.contains( mdl->id() ) )
        return;
    d->modules.insert(mdl->id(), mdl);
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
    removeModule( mdl->id() );
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
    if (d->driver)
    {
        if ( d->driver->isBuisy() )
            return;
        if (d->driver->parent() == (QObject *) this)
        {
            d->setDriverEditor(d->driver, 0);
            d->driver->deleteLater();
        }
    }
    d->driver = drv;
    if (drv)
    {
        d->setDriverEditor(drv, this);
        if ( !drv->parent() )
            drv->setParent(this);
    }
}

void BCodeEditor::addFileType(BAbstractFileType *ft)
{
    if ( d_func()->tryAddFileType(ft) )
        d_func()->emitFileTypesChanged();
}

void BCodeEditor::removeFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    removeFileType( ft->id() );
}

void BCodeEditor::removeFileType(const QString &id)
{
    if ( d_func()->tryRemoveFileType(id) )
        d_func()->emitFileTypesChanged();
}

void BCodeEditor::setFileTypes(const QList<BAbstractFileType *> &list)
{
    bool b = false;
    foreach ( BAbstractFileType *ft, fileTypes() )
        b = ( b || d_func()->tryRemoveFileType( ft->id() ) );
    foreach (BAbstractFileType *ft, list)
        b = ( b || d_func()->tryAddFileType(ft) );
    if (b)
        d_func()->emitFileTypesChanged();
}

bool BCodeEditor::waitForAllDocumentsProcessed(int msecs)
{
    B_D(BCodeEditor);
    if ( d->processedDocuments.isEmpty() )
        return true;
    if (msecs <= 0)
        return false;
    QEventLoop el;
    QTimer::singleShot( msecs, &el, SLOT( quit() ) );
    connect( this, SIGNAL( allDocumentsProcessed() ), &el, SLOT( quit() ) );
    el.exec();
    return d->processedDocuments.isEmpty();
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

QTextCodec *BCodeEditor::defaultCodec() const
{
    return d_func()->defaultCodec;
}

QString BCodeEditor::defaultCodecName() const
{
    return QString::fromAscii( d_func()->defaultCodec->name().data() );
}

QString BCodeEditor::defaultFileName() const
{
    return d_func()->defaultFN;
}

BAbstractEditorModule *BCodeEditor::module(const QString &name) const
{
    return d_func()->modules.value(name);
}

BAbstractEditorModule *BCodeEditor::module(StandardModule type) const
{
    foreach (BAbstractEditorModule *mdl, d_func()->modules)
    {
        QVariant v = mdl->property("beqt/standard_module_type");
        if ( v.isNull() )
            continue;
        if (static_cast<StandardModule>( v.toInt() ) == type)
            return mdl;
    }
    return 0;
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

BAbstractFileType *BCodeEditor::fileType(const QString &id) const
{
    BAbstractFileType *ft = d_func()->defaultFileType;
    if (ft->id() != id)
        ft = d_func()->fileTypes.value(id);
    return ft;
}

QList<BAbstractFileType *> BCodeEditor::fileTypes() const
{
    QList<BAbstractFileType *> list = d_func()->fileTypes.values();
    list << d_func()->defaultFileType;
    return list;
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

QObject *BCodeEditor::dropHandler() const
{
    return d_func()->dropHandler;
}

//

bool BCodeEditor::addDocument(const QString &fileName)
{
    return addDocument(fileName, "");
}

bool BCodeEditor::addDocument(const QString &fileName, const QString &text)
{
    B_D(BCodeEditor);
    if ( d->findDocument( BCodeEditorPrivate::createFileName( fileName, d->defaultFN, fileNames() ) ) )
        return false;
    d->addDocument( d->createDocument(fileName, text) );
    return true;
}

bool BCodeEditor::openDocuments()
{
    QStringList list;
    QTextCodec *codec = d_func()->defaultCodec;
    return d_func()->driver->getOpenFileNames(this, list, codec) && openDocuments(list, codec);
}

bool BCodeEditor::openDocuments(const QStringList &fileNames, QTextCodec *codec)
{
    foreach (const QString &fn, fileNames)
        if ( !openDocument(fn, codec) )
            return false;
    return true;
}

bool BCodeEditor::openDocument(const QString &fileName, QTextCodec *codec)
{
    return d_func()->openDocument(fileName, codec);
}

bool BCodeEditor::saveCurrentDocument()
{
    return currentDocument() && d_func()->saveDocument( currentDocument() );
}

bool BCodeEditor::reopenCurrentDocument(QTextCodec *codec)
{
    return currentDocument() && d_func()->reopenDocument(currentDocument(), codec);
}

bool BCodeEditor::saveCurrentDocumentAs()
{
    if ( !currentDocument() )
        return false;
    QString nfn;
    QTextCodec *codec = currentDocument()->codec();
    return d_func()->driver->getSaveAsFileName(this, currentDocument()->fileName(), nfn, codec) &&
            saveCurrentDocumentAs(nfn, codec);
}

bool BCodeEditor::saveCurrentDocumentAs(const QString &newFileName, QTextCodec *codec)
{
    if ( newFileName.isEmpty() || !currentDocument() )
        return false;
    return d_func()->saveDocument(currentDocument(), newFileName, codec);
}

bool BCodeEditor::saveAllDocuments()
{
    return d_func()->saveDocuments( documents() );
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
