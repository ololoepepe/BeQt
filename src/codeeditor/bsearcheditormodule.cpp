#include "bsearcheditormodule.h"
#include "bsearcheditormodule_p.h"
#include "bcodeeditordocument.h"
#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>
#include <BeQtWidgets/BApplication>

#include <QObject>
#include <QDialog>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QSizePolicy>
#include <QLineEdit>
#include <QEvent>
#include <QShowEvent>
#include <QTextDocument>
#include <QMetaObject>
#include <QPointer>
#include <QList>

#include <QDebug>

/*============================================================================
================================ Search Dialog Private
============================================================================*/

BSearchDialogPrivate::BSearchDialogPrivate(BSearchDialog *q) :
    BBasePrivate(q)
{
    //
}

BSearchDialogPrivate::~BSearchDialogPrivate()
{
    //
}

//

void BSearchDialogPrivate::init()
{
    B_Q(BSearchDialog);
    document = 0;
    q->installEventFilter(this);
    //
    vlt = new QVBoxLayout(q);
      hltSearch = new QHBoxLayout;
        lblSearch = new QLabel(q);
        hltSearch->addWidget(lblSearch);
        cmboxSearch = new QComboBox(q);
          cmboxSearch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
          cmboxSearch->setMaxCount(DefMaximumHistorySize);
          cmboxSearch->setMaxVisibleItems(DefMaximumHistorySize);
          cmboxSearch->setInsertPolicy(QComboBox::InsertAtTop);
          cmboxSearch->setEditable(true);
          connect( cmboxSearch, SIGNAL( editTextChanged(QString) ), this, SLOT( checkSearchReplace() ) );
        hltSearch->addWidget(cmboxSearch);
      vlt->addLayout(hltSearch);
      hltReplace = new QHBoxLayout;
        lblReplace = new QLabel(q);
        hltReplace->addWidget(lblReplace);
        cmboxReplace = new QComboBox(q);
          cmboxReplace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
          cmboxReplace->setMaxCount(DefMaximumHistorySize);
          cmboxReplace->setMaxVisibleItems(DefMaximumHistorySize);
          cmboxReplace->setInsertPolicy(QComboBox::InsertAtTop);
          cmboxReplace->setEditable(true);
        hltReplace->addWidget(cmboxReplace);
      vlt->addLayout(hltReplace);
      gboxOptions = new QGroupBox(q);
        vltOptions = new QVBoxLayout;
          cboxCaseSensitive = new QCheckBox;
          vltOptions->addWidget(cboxCaseSensitive);
          cboxWholeWords = new QCheckBox;
          vltOptions->addWidget(cboxWholeWords);
          cboxBackwardOrder = new QCheckBox;
          vltOptions->addWidget(cboxBackwardOrder);
          cboxCyclic = new QCheckBox;
            cboxCyclic->setChecked(true);
          vltOptions->addWidget(cboxCyclic);
        gboxOptions->setLayout(vltOptions);
      vlt->addWidget(gboxOptions);
      dlgbbox = new QDialogButtonBox(q);
        dlgbbox->setStandardButtons(QDialogButtonBox::Close);
        connect( dlgbbox->button(QDialogButtonBox::Close), SIGNAL( clicked() ), q, SLOT( close() ) );
        btnReplaceAll = new QPushButton;
          mnuReplaceAll = new QMenu(q);
            actSelection = new QAction(this);
              connect( actSelection, SIGNAL( triggered() ), this, SLOT( actSelectionTriggered() ) );
            mnuReplaceAll->addAction(actSelection);
            actDocument = new QAction(this);
              connect( actDocument, SIGNAL( triggered() ), this, SLOT( actDocumentTriggered() ) );
            mnuReplaceAll->addAction(actDocument);
          btnReplaceAll->setMenu(mnuReplaceAll);
        dlgbbox->addButton(btnReplaceAll, QDialogButtonBox::ActionRole);
        btnReplace = new QPushButton;
        connect( btnReplace, SIGNAL( clicked() ), q, SLOT( replaceNext() ) );
        dlgbbox->addButton(btnReplace, QDialogButtonBox::ApplyRole);
        btnFind = new QPushButton;
          btnFind->setDefault(true);
          connect( btnFind, SIGNAL( clicked() ), q, SLOT( findNext() ) );
        dlgbbox->addButton(btnFind, QDialogButtonBox::ApplyRole);
      vlt->addWidget(dlgbbox);
    //
    checkSearchReplace();
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

bool BSearchDialogPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::Show)
    {
        if ( document && document->hasSelection() )
            cmboxSearch->lineEdit()->setText( document->selectedText() );
        cmboxSearch->lineEdit()->setFocus();
        cmboxSearch->lineEdit()->selectAll();
    }
    return QObject::eventFilter(o, e);
}

void BSearchDialogPrivate::appendHistory(QComboBox *cmbox)
{
    QString text = cmbox->currentText();
    if ( text.isEmpty() )
        return;
    int index = cmbox->findText(text);
    if (index >= 0)
        cmbox->removeItem(index);
    cmbox->insertItem(0, text);
    cmbox->setCurrentIndex(0);
}

void BSearchDialogPrivate::emitTextReplaced(int count, const QString &oldText, const QString &newText)
{
    if (count < 0)
        return;
    QMetaObject::invokeMethod( q_func(), "textReplaced", Q_ARG(int, count),
                               Q_ARG(QString, oldText), Q_ARG(QString, newText) );
}

QTextDocument::FindFlags BSearchDialogPrivate::createFindFlags() const
{
    QTextDocument::FindFlags flags = 0;
    if ( cboxCaseSensitive->isChecked() )
        flags |= QTextDocument::FindCaseSensitively;
    if ( cboxWholeWords->isChecked() )
        flags |= QTextDocument::FindWholeWords;
    if ( cboxBackwardOrder->isChecked() )
        flags |= QTextDocument::FindBackward;
    return flags;
}

QString BSearchDialogPrivate::windowTitle() const
{
    return cmboxReplace->isVisible() ? trq("Find and replace", "windowTitle") : trq("Find", "windowTitle");
}

//

void BSearchDialogPrivate::retranslateUi()
{
    q_func()->setWindowTitle( windowTitle() );
    lblSearch->setText( trq("Find:", "lbl text") );
    lblReplace->setText( trq("Replace with:", "lbl text") );
    gboxOptions->setTitle( trq("Options", "gbox title") );
    cboxCaseSensitive->setText( trq("Case sensitive", "cbox text") );
    cboxWholeWords->setText( trq("Whole words only", "cbox text") );
    cboxBackwardOrder->setText( trq("Reverse search", "cbox text") );
    cboxCyclic->setText( trq("Cyclic search", "cbox text") );
    btnReplaceAll->setText( trq("Replace all", "btn text") );
    actSelection->setText( trq("in selection", "act text") );
    actDocument->setText( trq("in document", "act text") );
    btnReplace->setText( trq("Replace", "btn text") );
    btnFind->setText( trq("Find", "btn text") );
}

void BSearchDialogPrivate::checkSearchReplace()
{
    bool hasText = !cmboxSearch->lineEdit()->text().isEmpty();
    bool hasSelection = document && document->hasSelection();
    bool writable = document && !document->isReadOnly();
    bool fnap = btnFind->isEnabled();
    bool rnap = btnReplace->isEnabled();
    btnReplaceAll->setEnabled(hasText && writable);
    actSelection->setEnabled(hasText && hasSelection && writable);
    actDocument->setEnabled(hasText && writable);
    btnReplace->setEnabled(hasSelection && writable);
    btnFind->setEnabled(hasText && document);
    if (btnFind->isEnabled() != fnap)
        QMetaObject::invokeMethod( q_func(), "findNextAvailableChanged", Q_ARG( bool, btnFind->isEnabled() ) );
    if (btnReplace->isEnabled() != rnap)
        QMetaObject::invokeMethod( q_func(), "replaceNextAvailableChanged", Q_ARG( bool, btnReplace->isEnabled() ) );
}

void BSearchDialogPrivate::actSelectionTriggered()
{
    QString oldText = cmboxSearch->lineEdit()->text();
    QString newText = cmboxReplace->lineEdit()->text();
    emitTextReplaced(document->replaceInSelection( oldText, newText, q_func()->caseSensitivity() ), oldText, newText);
}

void BSearchDialogPrivate::actDocumentTriggered()
{
    QString oldText = cmboxSearch->lineEdit()->text();
    QString newText = cmboxReplace->lineEdit()->text();
    emitTextReplaced(document->replaceInDocument( oldText, newText, q_func()->caseSensitivity() ), oldText, newText);
}

//

const int BSearchDialogPrivate::DefMaximumHistorySize = 20;
const QDataStream::Version BSearchDialogPrivate::DSVersion = QDataStream::Qt_4_8;

/*============================================================================
================================ Search Dialog
============================================================================*/

BSearchDialog::BSearchDialog(QWidget *parent) :
    QDialog(parent), BBase( *new BSearchDialogPrivate(this) )
{
    d_func()->init();
}

BSearchDialog::BSearchDialog(bool replaceEnabled, QWidget *parent) :
    QDialog(parent), BBase( *new BSearchDialogPrivate(this) )
{
    d_func()->init();
    setReplaceEnabled(replaceEnabled);
}

BSearchDialog::~BSearchDialog()
{
    //
}

//

void BSearchDialog::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    d_func()->cboxCaseSensitive->setChecked(Qt::CaseSensitive == cs);
}

void BSearchDialog::setMatchWholeWords(bool enabled)
{
    d_func()->cboxWholeWords->setChecked(enabled);
}

void BSearchDialog::setBackwardOrder(bool enabled)
{
    d_func()->cboxBackwardOrder->setChecked(enabled);
}

void BSearchDialog::setCyclicSearch(bool enabled)
{
    d_func()->cboxCyclic->setChecked(enabled);
}

void BSearchDialog::setSearchHistory(const QStringList &list)
{
    B_D(BSearchDialog);
    d->cmboxSearch->clear();
    d->cmboxSearch->addItems(list);
}

void BSearchDialog::setReplaceHistory(const QStringList &list)
{
    B_D(BSearchDialog);
    d->cmboxReplace->clear();
    d->cmboxReplace->addItems(list);
}

void BSearchDialog::setMaximumHistorySize(int sz)
{
    if (sz <= 0)
        return;
    B_D(BSearchDialog);
    d->cmboxSearch->setMaxCount(sz);
    d->cmboxSearch->setMaxVisibleItems(sz);
    d->cmboxReplace->setMaxCount(sz);
    d->cmboxReplace->setMaxVisibleItems(sz);
}

void BSearchDialog::setReplaceEnabled(bool enabled)
{
    B_D(BSearchDialog);
    d->lblReplace->setVisible(enabled);
    d->cmboxReplace->setVisible(enabled);
    d->btnReplaceAll->setVisible(enabled);
    d->btnReplace->setVisible(enabled);
    setWindowTitle( d->windowTitle() );
}

void BSearchDialog::setDocument(BCodeEditorDocument *doc)
{
    B_D(BSearchDialog);
    if (d->document)
        disconnect( d->document, SIGNAL( hasSelectionChanged(bool) ), d, SLOT( checkSearchReplace() ) );
    if (doc)
    {
        d->cmboxSearch->lineEdit()->setMaxLength( doc->editLineLength() );
        d->cmboxReplace->lineEdit()->setMaxLength( doc->editLineLength() );
        connect( doc, SIGNAL( hasSelectionChanged(bool) ), d, SLOT( checkSearchReplace() ) );
    }
    d->document = doc;
    d->checkSearchReplace();
}

void BSearchDialog::restoreState(const QByteArray &ba)
{
    QDataStream out(ba);
    out.setVersion(BSearchDialogPrivate::DSVersion);
    bool cs = false;
    bool wholeWords = false;
    bool backward = false;
    bool cyclic = true;
    QStringList searchHist;
    QStringList replaceHist;
    out >> cs;
    out >> wholeWords;
    out >> backward;
    out >> cyclic;
    out >> searchHist;
    out >> replaceHist;
    setCaseSensitivity(cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
    setMatchWholeWords(wholeWords);
    setBackwardOrder(backward);
    setCyclicSearch(cyclic);
    setSearchHistory(searchHist);
    setReplaceHistory(replaceHist);
}

Qt::CaseSensitivity BSearchDialog::caseSensitivity() const
{
    return d_func()->cboxCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

bool BSearchDialog::matchWholeWords() const
{
    return d_func()->cboxWholeWords->isChecked();
}

bool BSearchDialog::backwardOrder() const
{
    return d_func()->cboxBackwardOrder->isChecked();
}

bool BSearchDialog::cyclicSearch() const
{
    return d_func()->cboxCyclic->isChecked();
}

QStringList BSearchDialog::searchHistory() const
{
    QStringList sl;
    const B_D(BSearchDialog);
    for (int i = 0; i < d->cmboxSearch->count(); ++i)
        sl << d->cmboxSearch->itemText(i);
    return sl;
}

QStringList BSearchDialog::replaceHistory() const
{
    QStringList sl;
    const B_D(BSearchDialog);
    for (int i = 0; i < d->cmboxReplace->count(); ++i)
        sl << d->cmboxReplace->itemText(i);
    return sl;
}

int BSearchDialog::maximumHistorySize() const
{
    return d_func()->cmboxSearch->maxCount();
}

bool BSearchDialog::replaceEnabled() const
{
    return d_func()->cmboxReplace->isVisible();
}

BCodeEditorDocument *BSearchDialog::document() const
{
    return d_func()->document;
}

bool BSearchDialog::findNextAvailable() const
{
    return d_func()->btnFind->isEnabled();
}

bool BSearchDialog::replaceNextAvailable() const
{
    return d_func()->btnReplace->isEnabled();
}

QByteArray BSearchDialog::saveState() const
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out.setVersion(BSearchDialogPrivate::DSVersion);
    out << (caseSensitivity() == Qt::CaseSensitive);
    out << matchWholeWords();
    out << backwardOrder();
    out << cyclicSearch();
    out << searchHistory();
    out << replaceHistory();
    return ba;
}

//

void BSearchDialog::findNext()
{
    if ( !findNextAvailable() )
        return;
    B_D(BSearchDialog);
    QString text = d->cmboxSearch->lineEdit()->text();
    bool b = d->document->findNext( text, d->createFindFlags(), cyclicSearch() );
    if (b)
    {
        d->document->window()->activateWindow();
        d->document->setFocus();
    }
    emit textFound(b, text);
}

void BSearchDialog::replaceNext()
{
    if ( !replaceNextAvailable() )
        return;
    B_D(BSearchDialog);
    QString text = d->cmboxReplace->lineEdit()->text();
    emit textReplaced(d->document->replaceNext(text) ? 1 : 0, d->cmboxSearch->lineEdit()->text(), text);
    findNext();
}

//

BSearchDialog::BSearchDialog(BSearchDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================================================================
================================ Search Editor Module Private
============================================================================*/

BSearchEditorModulePrivate::BSearchEditorModulePrivate(BSearchEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BSearchEditorModulePrivate::~BSearchEditorModulePrivate()
{
    //
}

void BSearchEditorModulePrivate::init()
{
    sdlg = new BSearchDialog;
    //sdlg->setModal(false);
    //
    B_Q(BSearchEditorModule);
    actFind = new QAction(this);
      actFind->setEnabled(false);
      actFind->setIcon( BApplication::icon("find") );
      connect( actFind.data(), SIGNAL( triggered() ), q, SLOT( find() ) );
    actFindNext = new QAction(this);
      actFindNext->setEnabled(false);
      actFindNext->setIcon( BApplication::icon("next") );
      connect( actFindNext.data(), SIGNAL( triggered() ), q, SLOT( findNext() ) );
      connect( sdlg, SIGNAL( findNextAvailableChanged(bool) ), actFindNext.data(), SLOT( setEnabled(bool) ) );
    //
    retranslateUi();
    connect( bApp, SIGNAL( languageChanged() ), this, SLOT( retranslateUi() ) );
}

void BSearchEditorModulePrivate::setDialogParent(QWidget *parent)
{
    Qt::WindowFlags flags = sdlg->windowFlags();
    sdlg->setParent(parent);
    sdlg->setWindowFlags(flags);
}

//

void BSearchEditorModulePrivate::retranslateUi()
{
    if ( !actFind.isNull() )
    {
        actFind->setText( trq("find", "act text") );
        actFind->setToolTip( trq("find", "act toolTip") );
        actFind->setWhatsThis( trq("find", "act whatsThis") );
    }
    if ( !actFindNext.isNull() )
    {
        actFindNext->setText( trq("find next", "act text") );
        actFindNext->setToolTip( trq("find next", "act toolTip") );
        actFindNext->setWhatsThis( trq("find next", "act whatsThis") );
    }
}

/*============================================================================
================================ Search Editor Module
============================================================================*/

BSearchEditorModule::BSearchEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BSearchEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BSearchEditorModule::~BSearchEditorModule()
{
    //
}

//

QString BSearchEditorModule::name() const
{
    return "beqt/search";
}

QAction *BSearchEditorModule::action(Action type) const
{
    switch (type)
    {
    case FindAction:
        return d_func()->actFind.data();
    case FindNextAction:
        return d_func()->actFindNext.data();
    default:
        return 0;
    }
}

QList<QAction *> BSearchEditorModule::actions() const
{
    const B_D(BSearchEditorModule);
    QList<QAction *> list;
    if ( !d->actFind.isNull() )
        list << d->actFind.data();
    if ( !d->actFindNext.isNull() )
        list << d->actFindNext.data();
    return list;
}

//

void BSearchEditorModule::find()
{
    B_D(BSearchEditorModule);
    if ( d->sdlg->isVisible() )
        d->sdlg->activateWindow();
    else
        d->sdlg->show();
}

void BSearchEditorModule::findNext()
{
    find(); //Not sure if needed
    d_func()->sdlg->findNext();
}

//

void BSearchEditorModule::editorSet(BCodeEditor *edr)
{
    B_D(BSearchEditorModule);
    BCodeEditorDocument *doc = edr ? edr->currentDocument() : 0;
    d->setDialogParent(edr);
    d->sdlg->setDocument(doc);
    d->actFind->setEnabled(doc);
}

void BSearchEditorModule::editorUnset(BCodeEditor *edr)
{
    B_D(BSearchEditorModule);
    d->setDialogParent(0);
    d->sdlg->setDocument(0);
    d->actFind->setEnabled(0);
}

void BSearchEditorModule::currentDocumentChanged(BCodeEditorDocument *doc)
{
    B_D(BSearchEditorModule);
    d->sdlg->setDocument(doc);
    d->actFind->setEnabled(doc);
}