#include "bcodeeditor.h"
#include "bcodeeditor_p.h"
#include "babstractcodeeditordocument.h"
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
#include <QKeyEvent>
#include <QCheckBox>
#include <QMenu>
#include <QVariant>
#include <QSignalMapper>
#include <QComboBox>
#include <QCursor>

#include <QDebug>

/*============================================================================
================================ StructuredCodecsComboBox ====================
============================================================================*/

/*============================== Public constructors =======================*/

StructuredCodecsComboBox::StructuredCodecsComboBox(QWidget *parent) :
    QComboBox(parent)
{
    mnu = BCodeEditor::createStructuredCodecsMenu(0, 0, this);
    QAction *act = mnu->actions().first()->menu()->actions().first();
    cn = act->property("beqt/codec_name").toString();
    addItem(act->text());
}

/*============================== Public methods ============================*/

void StructuredCodecsComboBox::retranslate()
{
    BCodeEditor::retranslateCodecsMenu(mnu);
}

void StructuredCodecsComboBox::setCodecName(const QString &codecName)
{
    if (codecName.isEmpty() || !BCodeEditor::supportedCodecNames().contains(codecName, Qt::CaseInsensitive))
        return;
    cn = codecName;
    setItemText(0, BCodeEditor::fullCodecName(cn));
}

QString StructuredCodecsComboBox::codecName() const
{
    return cn;
}

/*============================== Protected methods =========================*/

void StructuredCodecsComboBox::showPopup()
{
    mnu->setMinimumWidth(width());
    QWidget *p = parentWidget();
    QAction *act = mnu->exec(p ? p->mapToGlobal(pos()) : pos());
    QComboBox::hidePopup();
    if (act)
    {
        cn = act->property("beqt/codec_name").toString();
        setItemText(0, act->text());
    }
}

void StructuredCodecsComboBox::hidePopup()
{
    mnu->close();
    QComboBox::hidePopup();
}

/*============================================================================
================================ SelectDocumentsDialogPrivate ================
============================================================================*/

/*============================== Public constructors =======================*/

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

/*============================== Public methods ============================*/

void BSelectDocumentsDialogPrivate::init()
{
    B_Q(BSelectDocumentsDialog);
    decision = QMessageBox::Cancel;
    //
    vlt = new QVBoxLayout(q);
      lblText = new QLabel(q);
        QFont fnt;
        fnt.setBold(true);
        lblText->setWordWrap(true);
        lblText->setFont(fnt);
        lblText->setText( tr("Some documents are modified. Do you want to save them before closing?", "lbl text") );
      vlt->addWidget(lblText);
      lblInformativeTextUpper = new QLabel(q);
        lblInformativeTextUpper->setWordWrap(true);
        lblInformativeTextUpper->setText( tr("Choose documents that you want to save:", "lbl text") );
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
        lblInformativeTextLower->setText( tr("If you don't save the documents, all changes will be lost",
                                             "lbl text") );
      vlt->addWidget(lblInformativeTextLower);
      dlgbbox = new QDialogButtonBox(q);
        dlgbbox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Discard | QDialogButtonBox::Cancel);
        dlgbbox->button(QDialogButtonBox::Discard)->setDefault(true);
        connect( dlgbbox, SIGNAL( clicked(QAbstractButton *) ), this, SLOT( dlgbboxClicked(QAbstractButton *) ) );
      vlt->addWidget(dlgbbox);
}

/*============================== Public slots ==============================*/

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
================================ BSelectDocumentsDialog ======================
============================================================================*/

/*============================== Public constructors =======================*/

BSelectDocumentsDialog::BSelectDocumentsDialog(const QList<BCodeEditorDocument *> &documents, QWidget *parent) :
    QDialog(parent), BBase( *new BSelectDocumentsDialogPrivate(this, documents) )
{
    d_func()->init();
}

BSelectDocumentsDialog::~BSelectDocumentsDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSelectDocumentsDialog::BSelectDocumentsDialog(BSelectDocumentsDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    //
}

/*============================== Public methods ============================*/

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

/*============================================================================
================================ BSplittedLinesDialogPrivate =================
============================================================================*/

/*============================== Public constructors =======================*/

BSplittedLinesDialogPrivate::BSplittedLinesDialogPrivate(
        BSplittedLinesDialog *q, const QList<BCodeEdit::SplittedLinesRange> &ranges, int lineLength) :
    BBasePrivate(q), Ranges(ranges), LineLength(lineLength)
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
    lwi->setData( Qt::UserRole, QPoint(0, lb) );
    lwi->setData( Qt::UserRole + 1, QPoint(lineLength, ub) );
    QString text = tr("Lines:", "lstwgti text") + " " + QString::number(lb + 1) + " - " + QString::number(ub + 1);
    text += " (" + tr("total:", "lstwgti text") + QString::number(ub - lb + 1) + ")";
    lwi->setText(text);
    return lwi;
}

/*============================== Public methods ============================*/

void BSplittedLinesDialogPrivate::init()
{
    B_Q(BSplittedLinesDialog);
    q->setWindowTitle( tr("Splitted lines", "windowTitle") );
    q->resize(400, 300);
    vlt = new QVBoxLayout(q);
      lbl = new QLabel(q);
        lbl->setWordWrap(true);
        lbl->setText( tr("Some lines in this document were too long, "
                         "so they were splitted into several lines", "lbl text") );
      vlt->addWidget(lbl);
      lstwgt = new QListWidget(q);
        foreach (const BCodeEdit::SplittedLinesRange &range, Ranges)
            lstwgt->insertItem(0, createListWidgetItem(range, LineLength) );
        lstwgt->setToolTip( tr("Doubleclick an item to go to the corresponding line", "lstwgt toolTip") );
        connect( lstwgt, SIGNAL( itemDoubleClicked(QListWidgetItem *) ),
                 this, SLOT( lstwgtItemDoubleClicked(QListWidgetItem *) ) );
      vlt->addWidget(lstwgt);
      cboxSelect = new QCheckBox(q);
        cboxSelect->setChecked(true);
        cboxSelect->setText( tr("Select text when going to line", "cbox text") );
      vlt->addWidget(cboxSelect);
      dlgbbox = new QDialogButtonBox(q);
        dlgbbox->setStandardButtons(QDialogButtonBox::Close);
        connect( dlgbbox->button(QDialogButtonBox::Close), SIGNAL( clicked() ), q, SLOT( close() ) );
        btnGoto = new QPushButton;
          btnGoto->setDefault(true);
          btnGoto->setText( tr("Go to line", "btn text") );
          connect( btnGoto, SIGNAL( clicked() ), this, SLOT( btnGotoClicked() ) );
        dlgbbox->addButton(btnGoto, QDialogButtonBox::ActionRole);
      vlt->addWidget(dlgbbox);
}

/*============================== Public slots ==============================*/

void BSplittedLinesDialogPrivate::lstwgtItemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    if ( cboxSelect->isChecked() )
        QMetaObject::invokeMethod( q_func(), "selectLines", Q_ARG( QPoint, item->data(Qt::UserRole).toPoint() ),
                                   Q_ARG( QPoint, item->data(Qt::UserRole + 1).toPoint() ) );
    else
        QMetaObject::invokeMethod( q_func(), "gotoLine", Q_ARG( QPoint, item->data(Qt::UserRole).toPoint() ) );
}

void BSplittedLinesDialogPrivate::btnGotoClicked()
{
    lstwgtItemDoubleClicked( lstwgt->currentItem() );
}

/*============================================================================
================================ BSplittedLinesDialog ========================
============================================================================*/

/*============================== Public constructors =======================*/

BSplittedLinesDialog::BSplittedLinesDialog(const QList<BCodeEdit::SplittedLinesRange> &ranges,
                                           int lineLength, QWidget *parent) :
    QDialog(parent), BBase( *new BSplittedLinesDialogPrivate(this, ranges, lineLength) )
{
    d_func()->init();
}

BSplittedLinesDialog::~BSplittedLinesDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSplittedLinesDialog::BSplittedLinesDialog(BSplittedLinesDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================================================================
================================ BDropHandler ================================
============================================================================*/

/*============================== Public constructors =======================*/

BDropHandler::BDropHandler(BCodeEditorPrivate *parent) :
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
================================ BCodeEditorPrivate ==========================
============================================================================*/

/*============================== Public constructors =======================*/

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

/*============================== Static public methods =====================*/

QString BCodeEditorPrivate::defaultFileName()
{
    return tr("New document", "fileName");
}

QString BCodeEditorPrivate::createFileName(const QString &fileName, const QString &defaultName,
                                           const QStringList &existingNames)
{
    QFileInfo fi(!defaultName.isEmpty() ? defaultName : defaultFileName());
    QString fnbasens = fi.baseName();
    QString fnbasesuff = fi.suffix();
    QFileInfo fif(fileName);
    if ( !fileName.isEmpty() && (fif.baseName().remove( QRegExp(" \\d$") ) != fnbasens || fif.suffix() != fnbasesuff) )
        return fileName;
    int i = 1;
    QString fn = fnbasens + " " + QString::number(i++) + (!fnbasesuff.isEmpty() ? ("." + fnbasesuff) : "");
    while (existingNames.contains(fn))
        fn = fnbasens + " " + QString::number(i++) + (!fnbasesuff.isEmpty() ? ("." + fnbasesuff) : "");
    return fn;
}

void BCodeEditorPrivate::removeUnsupportedCodecNames(QStringList &list)
{
    foreach (int i, bRangeR(list.size() - 1, 0))
        if (!QTextCodec::codecForName(list.at(i).toLatin1()))
            list.removeAt(i);
}

QStringList BCodeEditorPrivate::supportedUnicodeCodecNames()
{
    init_once(QStringList, list, QStringList() << "UTF-8" << "UTF-16")
        removeUnsupportedCodecNames(list);
    return list;
}

QStringList BCodeEditorPrivate::supportedEastEuropeanCodecNames()
{
    init_once(QStringList, list, QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257" << "ISO 8859-5")
    {
        list << "KOI8-R" << "Windows-1251" << "KOI8-U" << "ISO 8859-16" << "ISO 8859-2" << "Windows-1250";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

QStringList BCodeEditorPrivate::supportedWestEuropeanCodecNames()
{
    init_once(QStringList, list, QStringList() << "ISO 8859-7" << "Windows-1253" << "IBM 850" << "ISO 8859-1")
    {
        list << "ISO 8859-15" << "Apple Roman" << "Windows-1252" << "ISO 8859-14" << "ISO 8859-10" << "ISO 8859-3";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

QStringList BCodeEditorPrivate::supportedEastAsianCodecNames()
{
    init_once(QStringList, list, QStringList() << "Windows-1258" << "Big5" << "Big5-HKSCS" << "GB18030-0" << "EUC-KR")
    {
        list << "JOHAB" << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS";
        removeUnsupportedCodecNames(list);
    }
    return list;
}

QStringList BCodeEditorPrivate::supportedSouthEastSouthWestAsianCodecNames()
{
    init_once(QStringList, list, QStringList() << "TIS-620" << "ISO 8859-9" << "Windows-1254")
        removeUnsupportedCodecNames(list);
    return list;
}

QStringList BCodeEditorPrivate::supportedMiddleEastCodecNames()
{
    init_once(QStringList, list, QStringList()  << "ISO 8859-6" << "Windows-1256" << "Windows-1255" << "ISO 8859-8")
        removeUnsupportedCodecNames(list);
    return list;
}

QStringList BCodeEditorPrivate::supportedCodecNames()
{
    static QStringList list = QStringList() << supportedUnicodeCodecNames() << supportedEastEuropeanCodecNames()
        << supportedWestEuropeanCodecNames() << supportedEastAsianCodecNames()
        << supportedSouthEastSouthWestAsianCodecNames() << supportedMiddleEastCodecNames();
    return list;
}

QMap<QTextCodec *, QString> BCodeEditorPrivate::supportedCodecsMap()
{
    typedef QMap<QTextCodec *, QString> CodecMap;
    init_once(CodecMap, m, CodecMap())
        foreach (const QString &cn, supportedCodecNames())
            m.insert(QTextCodec::codecForName(cn.toLatin1()), cn);
    return m;
}

QString BCodeEditorPrivate::codecDescriptiveName(const QString &codecName)
{
    if ( supportedUnicodeCodecNames().contains(codecName) )
    {
        return tr("Unicode", "codec descriptiveName");
    }
    else if ( supportedEastEuropeanCodecNames().contains(codecName) )
    {
        if ( (QStringList() << "ISO 8859-13" << "ISO 8859-4" << "Windows-1257").contains(codecName,
                                                                                         Qt::CaseInsensitive) )
            return tr("Baltic", "codec descriptiveName");
        else if ( (QStringList() << "ISO 8859-5" << "KOI8-R" << "Windows-1251").contains(codecName,
                                                                                         Qt::CaseInsensitive) )
            return tr("Cyrillic", "codec descriptiveName");
        else if ( !codecName.compare("KOI8-U", Qt::CaseInsensitive) )
            return tr("Cyrillic (Ukrainian)", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-16", Qt::CaseInsensitive) )
            return tr("Romanic", "codec descriptiveName");
        else if ( (QStringList() << "ISO 8859-2" << "Windows-1250").contains(codecName, Qt::CaseInsensitive) )
            return tr("Central European", "codec descriptiveName");
    }
    else if ( supportedWestEuropeanCodecNames().contains(codecName) )
    {
        if ( (QStringList() << "ISO 8859-7" << "Windows-1253").contains(codecName, Qt::CaseInsensitive) )
            return tr("Greek", "codec descriptiveName");
        else if ( (QStringList() << "IBM 850" << "ISO 8859-1" << "ISO 8859-15"
                   << "Apple Roman" << "Windows-1252").contains(codecName, Qt::CaseInsensitive) )
            return tr("Western", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-14", Qt::CaseInsensitive) )
            return tr("Celtic", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-10", Qt::CaseInsensitive) )
            return tr("Nordic", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-3", Qt::CaseInsensitive) )
            return tr("South European", "codec descriptiveName");
    }
    else if ( supportedEastAsianCodecNames().contains(codecName) )
    {
        if ( !codecName.compare("Windows-1258", Qt::CaseInsensitive) )
            return tr("Vietnamese", "codec descriptiveName");
        else if ( (QStringList() << "Big5" << "Big5-HKSCS").contains(codecName, Qt::CaseInsensitive) )
            return tr("Traditional Chinese", "codec descriptiveName");
        else if ( !codecName.compare("GB18030-0", Qt::CaseInsensitive) )
            return tr("Simplified Chinese", "codec descriptiveName");
        else if ( (QStringList() << "EUC-KR" << "JOHAB").contains(codecName, Qt::CaseInsensitive) )
            return tr("Korean", "codec descriptiveName");
        else if ( (QStringList() << "EUC-JP" << "ISO 2022-JP" << "Shift-JIS").contains(codecName,
                                                                                       Qt::CaseInsensitive) )
            return tr("Japanese", "codec descriptiveName");
    }
    else if ( supportedSouthEastSouthWestAsianCodecNames().contains(codecName) )
    {
        if ( !codecName.compare("TIS-620", Qt::CaseInsensitive) )
            return tr("Thai", "codec descriptiveName");
        else if ( (QStringList() << "ISO 8859-9" << "Windows-1254").contains(codecName, Qt::CaseInsensitive) )
            return tr("Turkish", "codec descriptiveName");
    }
    else if ( supportedMiddleEastCodecNames().contains(codecName) )
    {
        if ( (QStringList() << "ISO 8859-6" << "Windows-1256").contains(codecName, Qt::CaseInsensitive) )
            return tr("Arabic", "codec descriptiveName");
        else if ( !codecName.compare("Windows-1255", Qt::CaseInsensitive) )
            return tr("Hebrew", "codec descriptiveName");
        else if ( !codecName.compare("ISO 8859-8", Qt::CaseInsensitive) )
            return tr("Hebrew (visual)", "codec descriptiveName");
    }
    return "";
}

/*============================== Public methods ============================*/

void BCodeEditorPrivate::init()
{
    B_Q(BCodeEditor);
    docType = BCodeEditor::StandardDocument;
    document = 0;
    editFont = BApplication::createMonospaceFont();
    editMode = BCodeEdit::NormalMode;
    editLineLength = 120;
    editTabWidth = BCodeEdit::TabWidth4;
    bracketsHighlighting = true;
    driver = 0;
    q->setDriver(new BLocalDocumentDriver);
    defaultFileType = BAbstractFileType::defaultFileType();
    preferredFileType = defaultFileType;
    defaultCodec = QTextCodec::codecForName("UTF-8");
    defaultFN = defaultFileName();
    maximumFileSize = BeQt::Megabyte;
    maxHistoryCount = 20;
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

bool BCodeEditorPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::KeyPress)
        return BBasePrivate::eventFilter(o, e);
    QKeyEvent *ke = static_cast<QKeyEvent *>(e);
    if (ke->modifiers() != Qt::ControlModifier || ke->key() != Qt::Key_Tab)
        return BBasePrivate::eventFilter(o, e);
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
    if (ft == preferredFileType)
        preferredFileType = defaultFileType;
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
            return true;
    foreach (const QString &fn, savingDocuments)
        if (fn == fileName)
            return true;
    return false;
}

BCodeEditorDocument *BCodeEditorPrivate::createDocument(const QString &fileName, const QString &text)
{
    BCodeEditorDocument *doc = q_func()->createDocument(q_func());
    doc->setFileName( createFileName( fileName, defaultFN, q_func()->fileNames() ) );
    doc->innerEdit()->viewport()->installEventFilter(dropHandler);
    doc->innerEdit()->installEventFilter(this);
    doc->setEditFont(editFont);
    doc->setEditMode(editMode);
    doc->setEditLineLength(editLineLength);
    doc->setEditTabWidth(editTabWidth);
    doc->setBracketHighlightingEnabled(bracketsHighlighting);
    doc->setCodec(defaultCodec);
    doc->setFileType( selectDocumentFileType(doc) );
    //
    connect( doc, SIGNAL( lineSplitted(BCodeEdit::SplittedLinesRange) ),
             this, SLOT( documentLineSplitted(BCodeEdit::SplittedLinesRange) ) );
    connect( doc, SIGNAL( linesSplitted(QList<BCodeEdit::SplittedLinesRange>) ),
             this, SLOT( documentLinesSplitted(QList<BCodeEdit::SplittedLinesRange>) ) );
    connect( doc, SIGNAL( modificationChanged(bool) ), this, SLOT( documentModificationChanged(bool) ) );
    connect( doc, SIGNAL( fileNameChanged(QString) ), this, SLOT( documentFileNameChanged(QString) ) );
    connect( doc, SIGNAL( loadingFinished(bool) ), this, SLOT( documentLoadingFinished(bool) ) );
    connect( doc, SIGNAL( savingFinished(bool) ), this, SLOT( documentSavingFinished(bool) ) );
    connect( doc, SIGNAL( buisyChanged(bool) ), this, SLOT( documentBuisyChanged(bool) ) );
    if ( !text.isEmpty() )
        doc->setText(text);
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
    QString fn = doc->fileName();
    emitDocumentAboutToBeRemoved(doc);
    twgt->removeTab(twgt->indexOf(doc));
    BSplittedLinesDialog *sld = doc->splittedLinesDialog();
    if (sld)
    {
        sld->close();
        sld->deleteLater();
    }
    delete doc;
    emitDocumentRemoved(fn);
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

BCodeEditorDocument *BCodeEditorPrivate::openDocument(QString fileName, QTextCodec *codec)
{
    fileName = QDir::fromNativeSeparators(fileName);
    if ( fileName.isEmpty() || findDocument(fileName) )
        return 0;
    if (QFileInfo(fileName).size() > maximumFileSize)
    {
        failedToOpenMessage(fileName, tr("The file is too large", "msgbox informativeText"));
        return 0;
    }
    BCodeEditorDocument *doc = createDocument(fileName);
    openingDocuments.insert(doc, fileName);
    if ( !doc->load(driver, codec, fileName) )
    {
        openingDocuments.remove(doc);
        doc->deleteLater();
        doc = 0;
    }
    return doc;
}

bool BCodeEditorPrivate::reopenDocument(BCodeEditorDocument *doc, QTextCodec *codec)
{
    if ( !doc || doc->isBuisy() )
        return false;
    if ( doc->isModified() && reopenModifiedMessage( doc->fileName() ) != QMessageBox::Yes)
        return false;
    return doc->load(driver, codec);
}

bool BCodeEditorPrivate::saveDocument(BCodeEditorDocument *doc, const QString &newFileName, QTextCodec *codec)
{
    if ( !doc || savingDocuments.contains(doc) )
        return false;
    QString nfn = newFileName;
    if (nfn.isEmpty() && !driver->testFileExistance(doc->fileName()))
    {
        if (!codec)
            codec = doc->codec();
        if (nfn.isEmpty() && !driver->getSaveAsFileName(q_func(), doc->fileName(), nfn, codec))
            return false;
        if (doc->fileName() != nfn && findDocument(nfn))
        {
            alreadyOpenedMessage(nfn);
            return false;
        }
    }
    else
    {
        if (doc->isReadOnly())
        {
            failedToSaveMessage(nfn);
            return false;
        }
        if (nfn.isEmpty())
            nfn = doc->fileName();
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
    foreach (BCodeEditorDocument *doc, list)
    {
        if (!doc->isModified())
            continue;
        if ( !saveDocument(doc) )
            return false;
    }
    return true;
}

bool BCodeEditorPrivate::closeDocument(BCodeEditorDocument *doc)
{
    if (!doc || openingDocuments.contains(doc) || savingDocuments.contains(doc))
        return false;
    if ((!isDefaultFileName(doc->fileName()) && !driver->testFileExistance(doc->fileName())) || doc->isModified())
    {
        switch (closeModifiedMessage(doc->fileName()))
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

bool BCodeEditorPrivate::closeAllDocuments(bool save)
{
    QList<BCodeEditorDocument *> list = q_func()->documents();
    if ( list.isEmpty() )
        return true;
    if (save)
    {
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
            if (nslist.size() == 1)
                return closeDocument( nslist.first() );
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

void BCodeEditorPrivate::updateDocumentTab(BCodeEditorDocument *doc)
{
    if (!doc)
        return;
    int ind = twgt->indexOf(doc);
    if (ind < 0)
        return;
    twgt->setTabText( ind, QFileInfo( doc->fileName() ).fileName() );
    twgt->setTabIcon( ind, BApplication::icon(doc->isModified() ? "filesave" : "") );
    twgt->setTabToolTip(ind, QDir::toNativeSeparators(doc->fileName()));
}

void BCodeEditorPrivate::appendFileHistory(const QString &fileName, const QString &oldFileName)
{
    if (!maxHistoryCount)
        return;
    if ( fileName.isEmpty() )
        return;
    if ( !oldFileName.isEmpty() )
        fileHistory.removeAll(oldFileName);
    fileHistory.removeAll(fileName);
    fileHistory.prepend(fileName);
    if (maxHistoryCount >= 0)
        while (fileHistory.size() > maxHistoryCount)
            fileHistory.removeLast();
    emitFileHistoryChanged(fileHistory);
}

void BCodeEditorPrivate::failedToOpenMessage(const QString &fileName, const QString &info)
{
    if ( fileName.isEmpty() )
        return;
    QMessageBox msg( q_func() );
    msg.setWindowTitle( tr("Failed to open file", "msgbox windowTitle") );
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
    if ( fileName.isEmpty() )
        return;
    QMessageBox msg( q_func() );
    msg.setWindowTitle( tr("Failed to save file", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Warning);
    QString text = tr("Failed to open file:", "msgbox text") + "\n" + fileName;
    if (!newFileName.isEmpty() && fileName != newFileName)
        text += "\n" + tr("as:", "msgbox text") + "\n" + newFileName;
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
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

int BCodeEditorPrivate::reopenModifiedMessage(const QString &fileName)
{
    QMessageBox msg( q_func() );
    msg.setWindowTitle( tr("Reopening modified document", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Question);
    msg.setText(tr("Document is modified:", "msgbox text") + "\n" + fileName);
    msg.setInformativeText( tr("Do you want to reopen it anyway?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec();
}

int BCodeEditorPrivate::closeModifiedMessage(const QString &fileName)
{
    QMessageBox msg( q_func() );
    msg.setWindowTitle( tr("Closing modified document", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Question);
    msg.setText(tr("Document is modified:", "msgbox text") + "\n" + fileName);
    msg.setInformativeText( tr("Do you want to save it before closing?", "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Discard);
    return msg.exec();
}

BSplittedLinesDialog *BCodeEditorPrivate::createSplittedLinesDialog(BCodeEditorDocument *doc,
                                                                    const QList<BCodeEdit::SplittedLinesRange> ranges)
{
    if ( !doc || !processedDocuments.contains(doc) )
        return 0;
    BSplittedLinesDialog *sld = doc->splittedLinesDialog();
    if (sld)
    {
        sld->close();
        sld->deleteLater();
    }
    sld = new BSplittedLinesDialog( ranges, doc->editLineLength(), q_func() );
    doc->setSplittedLinesDialog(sld);
    connect( sld, SIGNAL( gotoLine(QPoint) ), doc, SLOT( moveCursor(QPoint) ) );
    connect( sld, SIGNAL( selectLines(QPoint, QPoint) ), doc, SLOT( selectText(QPoint, QPoint) ) );
    connect( sld, SIGNAL( gotoLine(QPoint) ), doc, SLOT( activateWindow() ) );
    connect( sld, SIGNAL( selectLines(QPoint, QPoint) ), doc, SLOT( activateWindow() ) );
    sld->show();
    return sld;
}

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

void BCodeEditorPrivate::emitDocumentRemoved(const QString &fileName)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentRemoved(fileName);
    QMetaObject::invokeMethod(q_func(), "documentRemoved", Q_ARG(QString, fileName));
}

void BCodeEditorPrivate::emitCurrentDocumentChanged(BCodeEditorDocument *doc)
{
    foreach (BAbstractEditorModule *module, modules)
        module->currentDocumentChanged(doc);
    QMetaObject::invokeMethod( q_func(), "currentDocumentChanged", Q_ARG(BCodeEditorDocument *, doc) );
    QMetaObject::invokeMethod( q_func(), "documentAvailableChanged", Q_ARG(bool, doc) );
}

void BCodeEditorPrivate::emitCurrentDocumentFileNameChanged(const QString &fileName)
{
    QMetaObject::invokeMethod( q_func(), "currentDocumentFileNameChanged", Q_ARG(QString, fileName) );
}

void BCodeEditorPrivate::emitCurrentDocumentModificationChanged(bool modified)
{
    QMetaObject::invokeMethod( q_func(), "currentDocumentModificationChanged", Q_ARG(bool, modified) );
}

void BCodeEditorPrivate::emitFileTypesChanged()
{
    foreach (BAbstractEditorModule *module, modules)
        module->fileTypesChanged();
    QMetaObject::invokeMethod(q_func(), "fileTypesChanged");
}

void BCodeEditorPrivate::emitFileHistoryChanged(const QStringList &list)
{
    foreach (BAbstractEditorModule *module, modules)
        module->fileHistoryChanged(list);
    QMetaObject::invokeMethod( q_func(), "fileHistoryChanged", Q_ARG(QStringList, list) );
}

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

/*============================== Public slots ==============================*/

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
        disconnect( document, SIGNAL( codecChanged(QString) ), this, SLOT( documentCodecChanged(QString) ) );
        disconnect( document, SIGNAL( fileTypeChanged(BAbstractFileType *) ),
                    this, SLOT( documentFileTypeChanged(BAbstractFileType *) ) );
    }
    document = doc;
    if (document)
    {
        connect( document, SIGNAL( readOnlyChanged(bool) ), this, SLOT( documentReadOnlyChanged(bool) ) );
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
        connect( document, SIGNAL( codecChanged(QString) ), this, SLOT( documentCodecChanged(QString) ) );
        connect( document, SIGNAL( fileTypeChanged(BAbstractFileType *) ),
                 this, SLOT( documentFileTypeChanged(BAbstractFileType *) ) );
        document->setFocus();
    }
    emitCurrentDocumentChanged(document);
    emitCurrentDocumentFileNameChanged( document ? document->fileName() : QString() );
}

void BCodeEditorPrivate::twgtTabCloseRequested(int index)
{
    BCodeEditorDocument *doc = (index >= 0) ? static_cast<BCodeEditorDocument *>( twgt->widget(index) ) : 0;
    if (doc)
        closeDocument(doc);
}

void BCodeEditorPrivate::documentReadOnlyChanged(bool ro)
{
    foreach (BAbstractEditorModule *module, modules)
        module->documentReadOnlyChanged(ro);
}

void BCodeEditorPrivate::documentModificationChanged(bool modified)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if (!doc)
        return;
    updateDocumentTab(doc);
    if (doc == document)
    {
        emitCurrentDocumentModificationChanged(modified);
        foreach (BAbstractEditorModule *module, modules)
            module->documentModificationChanged(modified);
    }
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
    if (doc == document)
    {
        foreach (BAbstractEditorModule *module, modules)
            module->documentBuisyChanged(buisy);
    }
}

void BCodeEditorPrivate::documentLineSplitted(const BCodeEdit::SplittedLinesRange &linesRange)
{
    createSplittedLinesDialog(static_cast<BCodeEditorDocument *>( sender() ),
                              QList<BCodeEdit::SplittedLinesRange>() << linesRange);
}

void BCodeEditorPrivate::documentLinesSplitted(const QList<BCodeEdit::SplittedLinesRange> linesRanges)
{
    createSplittedLinesDialog(static_cast<BCodeEditorDocument *>( sender() ), linesRanges);
}

void BCodeEditorPrivate::documentFileNameChanged(const QString &fn)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if (!doc)
        return;
    updateDocumentTab(doc);
    if (doc == document)
    {
        emitCurrentDocumentFileNameChanged( doc->fileName() );
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
        {
            addDocument(doc);
            appendFileHistory(fn);
        }
        else
        {
            doc->deleteLater();
        }
    }
    if (success)
        doc->setReadOnly( driver->testFileReadOnly(fn) );
    else
        failedToOpenMessage(fn);
}

void BCodeEditorPrivate::documentSavingFinished(bool success)
{
    BCodeEditorDocument *doc = static_cast<BCodeEditorDocument *>( sender() );
    if ( !doc || !savingDocuments.contains(doc) )
        return;
    QString fn = savingDocuments.take(doc);
    if (success)
    {
        doc->setReadOnly( driver->testFileReadOnly( !fn.isEmpty() ? fn : doc->fileName() ) );
        appendFileHistory( !fn.isEmpty() ? fn : doc->fileName(), doc->fileName() );
    }
    else
    {
        failedToSaveMessage(doc->fileName(), fn);
    }
    if ( closingDocuments.contains(doc) )
    {
        closingDocuments.removeAll(doc);
        removeDocument(doc);
    }
}

/*============================================================================
================================ BCodeEditor =================================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditor::BCodeEditor(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditorPrivate(this) )
{
    d_func()->init();
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
    QWidget(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Static public methods =====================*/

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
    return codec && BCodeEditorPrivate::supportedCodecsMap().contains(codec);
}

bool BCodeEditor::supportsCodec(const QString &codecName)
{
    return !codecName.isEmpty() && supportsCodec( QTextCodec::codecForName( codecName.toLatin1() ) );
}

QList<QTextCodec *> BCodeEditor::supportedCodecs()
{
    return BCodeEditorPrivate::supportedCodecsMap().keys();
}

QStringList BCodeEditor::supportedCodecNames()
{
    return BCodeEditorPrivate::supportedCodecNames();
}

QString BCodeEditor::codecName(QTextCodec *codec)
{
    return BCodeEditorPrivate::supportedCodecsMap().value(codec);
}

QString BCodeEditor::fullCodecName(QTextCodec *codec)
{
    if ( !codec || !BCodeEditorPrivate::supportedCodecsMap().contains(codec) )
        return "";
    QString cn = BCodeEditorPrivate::supportedCodecsMap().value(codec);
    return BCodeEditorPrivate::codecDescriptiveName(cn) + " (" + cn + ")";
}

QString BCodeEditor::fullCodecName(const QString &codecName)
{
    return !codecName.isEmpty() ? fullCodecName( QTextCodec::codecForName( codecName.toLatin1() ) ) : QString();
}

QList<BCodeEditor::CodecGroup> BCodeEditor::codecGroups()
{
    static QList<CodecGroup> list = QList<CodecGroup>() << UnicodeGroup << EastEuropeanGroup << WestEuropeanGroup
        << EastAsianGroup << SouthEastSouthWestAsianGroup << MiddleEastGroup;
    return list;
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
        return BCodeEditorPrivate::supportedUnicodeCodecNames();
    case EastEuropeanGroup:
        return BCodeEditorPrivate::supportedEastEuropeanCodecNames();
    case WestEuropeanGroup:
        return BCodeEditorPrivate::supportedWestEuropeanCodecNames();
    case EastAsianGroup:
        return BCodeEditorPrivate::supportedEastAsianCodecNames();
    case SouthEastSouthWestAsianGroup:
        return BCodeEditorPrivate::supportedSouthEastSouthWestAsianCodecNames();
    case MiddleEastGroup:
        return BCodeEditorPrivate::supportedMiddleEastCodecNames();
    default:
        return QStringList();
    }
}

QMenu *BCodeEditor::createPlainCodecsMenu(QObject *receiver, const char *member, QWidget *parent)
{
    QMenu *mnu = new QMenu(parent);
    QSignalMapper *mpr = (receiver && member) ? new QSignalMapper(mnu) : 0;
    if (mpr)
        connect(mpr, SIGNAL(mapped(QString)), receiver, member);
    foreach (const QString &cn, supportedCodecNames())
    {
        QAction *act = mnu->addAction("");
        act->setProperty("beqt/codec_name", cn);
        bSetMapping(mpr, act, SIGNAL(triggered()), cn);
    }
    retranslateCodecsMenu(mnu);
    return mnu;
}

QMenu *BCodeEditor::createStructuredCodecsMenu(QObject *receiver, const char *member, QWidget *parent)
{
    QMenu *mnu = new QMenu(parent);
    QSignalMapper *mpr = (receiver && member) ? new QSignalMapper(mnu) : 0;
    if (mpr)
        connect(mpr, SIGNAL(mapped(QString)), receiver, member);
    foreach (CodecGroup gr, codecGroups())
    {
        QMenu *submnu = mnu->addMenu("");
        submnu->setProperty("beqt/codec_group", gr);
        foreach (const QString &cn, codecNamesForGroup(gr))
        {
            QAction *act = submnu->addAction("");
            act->setProperty("beqt/codec_name", cn);
            bSetMapping(mpr, act, SIGNAL(triggered()), cn);
        }
    }
    retranslateCodecsMenu(mnu);
    return mnu;
}

void BCodeEditor::retranslateCodecsMenu(QMenu *mnu)
{
    if (!mnu)
        return;
    QList<QAction *> actions;
    foreach (QAction *act, mnu->actions())
    {
        QMenu *submnu = act->menu();
        if (!submnu)
        {
            actions << act;
            continue;
        }
        bool ok = false;
        CodecGroup gr = static_cast<CodecGroup>(submnu->property("beqt/codec_group").toInt(&ok));
        if (!ok || InvalidGroup == gr)
            continue;
        submnu->setTitle(codecGroupName(gr));
        actions << submnu->actions();
    }
    foreach (QAction *act, actions)
    {
        QString cn = act->property("beqt/codec_name").toString();
        if (cn.isEmpty() || !supportedCodecNames().contains(cn))
            continue;
        act->setText(fullCodecName(cn));
    }
}

QComboBox *BCodeEditor::createPlainCodecsComboBox(QWidget *parent)
{
    QComboBox *cmbox = new QComboBox(parent);
    foreach (const QString &cn, supportedCodecNames())
        cmbox->addItem("", cn);
    retranslateCodecsComboBox(cmbox);
    return cmbox;
}

QComboBox *BCodeEditor::createStructuredCodecsComboBox(QWidget *parent)
{
    StructuredCodecsComboBox *cmbox = new StructuredCodecsComboBox(parent);
    cmbox->retranslate();
    return cmbox;
}

void BCodeEditor::retranslateCodecsComboBox(QComboBox *cmbox)
{
    if (!cmbox)
        return;
    if (!QString::compare(cmbox->metaObject()->className(), StructuredCodecsComboBox::staticMetaObject.className()))
        return static_cast<StructuredCodecsComboBox *>(cmbox)->retranslate();
    foreach (int i, bRangeD(0, cmbox->count() - 1))
    {
        QString cn = cmbox->itemData(i).toString();
        if (cn.isEmpty() || !supportedCodecNames().contains(cn))
            continue;
        cmbox->setItemText(i, fullCodecName(cn));
    }
}

void BCodeEditor::selectCodec(QComboBox *cmbox, QTextCodec *codec)
{
    if (!codec)
        return;
    selectCodec(cmbox, QString::fromLatin1(codec->name()));
}

void BCodeEditor::selectCodec(QComboBox *cmbox, const QString &codecName)
{
    if (!cmbox || codecName.isEmpty() || !supportedCodecNames().contains(codecName, Qt::CaseInsensitive))
        return;
    if (!QString::compare(cmbox->metaObject()->className(), StructuredCodecsComboBox::staticMetaObject.className()))
        return static_cast<StructuredCodecsComboBox *>(cmbox)->setCodecName(codecName);
    int ind = cmbox->findData(codecName, Qt::UserRole, 0);
    if (ind >= 0)
        cmbox->setCurrentIndex(ind);
}

QTextCodec *BCodeEditor::selectedCodec(QComboBox *cmbox)
{
    return QTextCodec::codecForName(selectedCodecName(cmbox).toLatin1());
}

QString BCodeEditor::selectedCodecName(QComboBox *cmbox)
{
    if (!cmbox)
        return "";
    if (!QString::compare(cmbox->metaObject()->className(), StructuredCodecsComboBox::staticMetaObject.className()))
        return static_cast<StructuredCodecsComboBox *>(cmbox)->codecName();
    int ind = cmbox->currentIndex();
    return (ind >= 0) ? cmbox->itemData(ind).toString() : QString();
}

/*============================== Public methods ============================*/

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

void BCodeEditor::setEditFontFamily(const QString &family)
{
    if ( family.isEmpty() )
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
    if (ll == d->editLineLength)
        return;
    d->editLineLength = ll;
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

void BCodeEditor::setMaximumFileSize(int sz)
{
    if (sz < 1)
        return;
    d_func()->maximumFileSize = sz;
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
    if ( !mdl->parent() )
        mdl->setParent(this);
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

void BCodeEditor::setPreferredFileType(BAbstractFileType *ft)
{
    if (!ft)
        return;
    setPreferredFileType( ft->id() );
}

void BCodeEditor::setPreferredFileType(const QString &id)
{
    if ( id.isEmpty() )
        return;
    B_D(BCodeEditor);
    if ( !d->fileTypes.contains(id) )
        return;
    d->preferredFileType = d->fileTypes.value(id);
}

void BCodeEditor::setFileHistory(const QStringList &list)
{
    B_D(BCodeEditor);
    if (!d->maxHistoryCount || list == d->fileHistory)
        return;
    d->fileHistory = list;
    if (d->maxHistoryCount >= 0 && d->fileHistory.size() > d->maxHistoryCount)
        while (d->fileHistory.size() > d->maxHistoryCount)
            d->fileHistory.removeLast();
    d->emitFileHistoryChanged(d->fileHistory);
}

void BCodeEditor::setMaxHistoryCount(int count)
{
    B_D(BCodeEditor);
    if (count == d->maxHistoryCount)
        return;
    if (count < 0)
        count = -1;
    d->maxHistoryCount = count;
    if (d->maxHistoryCount >= 0 && d->fileHistory.size() > count)
    {
        while (d->fileHistory.size() > count)
            d->fileHistory.removeLast();
        d->emitFileHistoryChanged(d->fileHistory);
    }
}

bool BCodeEditor::mergeWith(BCodeEditor *other)
{
    if (!other)
        return true;
    foreach ( BCodeEditorDocument *doc, other->documents() )
    {
        BCodeEditorDocument *ndoc = !doc->isBuisy() ? addDocument( doc->fileName(), doc->text() ) : 0;
        if (!ndoc)
            return false;
        ndoc->setModification( doc->isModified() );
    }
    other->closeAllDocuments(false);
    return true;
}

bool BCodeEditor::waitForAllDocumentsProcessed(int msecs)
{
    B_D(BCodeEditor);
    if (d->processedDocuments.isEmpty())
        return true;
    BeQt::waitNonBlocking(this, SIGNAL(allDocumentsProcessed()), msecs);
    return d->processedDocuments.isEmpty();
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
    return QString::fromLatin1( d_func()->defaultCodec->name().data() );
}

QString BCodeEditor::defaultFileName() const
{
    return d_func()->defaultFN;
}

int BCodeEditor::maximumFileSize() const
{
    return d_func()->maximumFileSize;
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

BCodeEditorDocument *BCodeEditor::document(const QString &fileName) const
{
    if (fileName.isEmpty())
        return 0;
    foreach (BCodeEditorDocument *doc, documents())
        if (doc->fileName() == fileName)
            return doc;
    return 0;
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

BAbstractFileType *BCodeEditor::preferredFileType() const
{
    return d_func()->preferredFileType;
}

QString BCodeEditor::preferredFileTypeId() const
{
    return preferredFileType()->id();
}

QStringList BCodeEditor::fileHistory() const
{
    return d_func()->fileHistory;
}

int BCodeEditor::maxHistoryCount() const
{
    return d_func()->maxHistoryCount;
}

bool BCodeEditor::documentAvailable() const
{
    return currentDocument();
}

QString BCodeEditor::currentDocumentFileName() const
{
    BCodeEditorDocument *doc = currentDocument();
    return doc ? doc->fileName() : QString();
}

bool BCodeEditor::isCurrentDocumentModified() const
{
    BCodeEditorDocument *doc = currentDocument();
    return doc && doc->isModified();
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

/*============================== Public slots ==============================*/

BCodeEditorDocument *BCodeEditor::addDocument(const QString &fileName)
{
    return addDocument(fileName, "");
}

BCodeEditorDocument *BCodeEditor::addDocument(const QString &fileName, const QString &text)
{
    B_D(BCodeEditor);
    if ( d->findDocument( BCodeEditorPrivate::createFileName( fileName, d->defaultFN, fileNames() ) ) )
        return 0;
    BCodeEditorDocument *doc = d->createDocument(fileName, text);
    d->addDocument(doc);
    return doc;
}

QList<BCodeEditorDocument *> BCodeEditor::openDocuments()
{
    QStringList list;
    QTextCodec *codec = d_func()->defaultCodec;
    return d_func()->driver->getOpenFileNames(this, list, codec) ? openDocuments(list, codec) :
                                                                   QList<BCodeEditorDocument *>();
}

QList<BCodeEditorDocument *> BCodeEditor::openDocuments(const QStringList &fileNames, QTextCodec *codec)
{
    QList<BCodeEditorDocument *> list;
    foreach (const QString &fn, fileNames)
    {
        BCodeEditorDocument *doc = openDocument(fn, codec);
        if (!doc)
            break;
        list << doc;
    }
    return list;
}

BCodeEditorDocument *BCodeEditor::openDocument(const QString &fileName, QTextCodec *codec)
{
    return d_func()->openDocument(fileName, codec);
}

bool BCodeEditor::saveCurrentDocument()
{
    BCodeEditorDocument *doc = currentDocument();
    return doc && (!doc->isModified() || d_func()->saveDocument(doc));
}

bool BCodeEditor::reopenCurrentDocument(QTextCodec *codec)
{
    return currentDocument() && d_func()->reopenDocument(currentDocument(), codec);
}

bool BCodeEditor::reopenCurrentDocument(const QString &codecName)
{
    return reopenCurrentDocument(QTextCodec::codecForName(codecName.toLatin1()));
}

bool BCodeEditor::saveCurrentDocumentAs()
{
    if ( !currentDocument() )
        return false;
    QString nfn;
    QTextCodec *codec = currentDocument()->codec();
    return d_func()->driver->getSaveAsFileName(this, currentDocument()->fileName(), nfn, codec) &&
            d_func()->saveDocument(currentDocument(), nfn, codec);
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

bool BCodeEditor::closeAllDocuments(bool save)
{
    return d_func()->closeAllDocuments(save);
}

void BCodeEditor::insertTextIntoCurrentDocument(const QString &text)
{
    if ( !currentDocument() )
        return;
    currentDocument()->insertText(text);
}

void BCodeEditor::setCurrentDocumentText(const QString &text)
{
    if ( !currentDocument() )
        return;
    currentDocument()->setText(text);
}

/*============================== Protected methods =========================*/

BCodeEditorDocument *BCodeEditor::createDocument(BCodeEditor *editor) const
{
    switch (d_func()->docType)
    {
    case StandardDocument:
        return new BCodeEditorDocument(editor);
    default:
        return 0;
    }
}
