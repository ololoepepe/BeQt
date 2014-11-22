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

#include "bsearcheditormodule.h"
#include "bsearcheditormodule_p.h"

#include "babstracteditormodule.h"
#include "babstracteditormodule_p.h"
#include "bcodeeditor.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>
#include <BeQtWidgets/BApplication>

#include <QAction>
#include <QByteArray>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMenu>
#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QStringList>
#include <QTextDocument>
#include <QVariant>
#include <QVariantMap>
#include <QVBoxLayout>

/*============================================================================
================================ BSearchDialogPrivate ========================
============================================================================*/

/*============================== Static public constants ===================*/

const int BSearchDialogPrivate::DefMaximumHistorySize = 20;

/*============================== Public constructors =======================*/

BSearchDialogPrivate::BSearchDialogPrivate(BSearchDialog *q) :
    BBaseObjectPrivate(q)
{
    //
}

BSearchDialogPrivate::~BSearchDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSearchDialogPrivate::appendHistory(QComboBox *cmbox)
{
    QString text = cmbox->currentText();
    if (text.isEmpty())
        return;
    int index = cmbox->findText(text);
    if (index >= 0)
        cmbox->removeItem(index);
    cmbox->insertItem(0, text);
    cmbox->setCurrentIndex(0);
}

QTextDocument::FindFlags BSearchDialogPrivate::createFindFlags() const
{
    QTextDocument::FindFlags flags = 0;
    if (cboxCaseSensitive->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (cboxWholeWords->isChecked())
        flags |= QTextDocument::FindWholeWords;
    if (cboxBackwardOrder->isChecked())
        flags |= QTextDocument::FindBackward;
    return flags;
}

void BSearchDialogPrivate::emitTextReplaced(int count, const QString &oldText, const QString &newText)
{
    if (count < 0)
        return;
    QMetaObject::invokeMethod(q_func(), "textReplaced", Q_ARG(int, count), Q_ARG(QString, oldText),
                              Q_ARG(QString, newText));
}

bool BSearchDialogPrivate::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::Show) {
        if (document && document->hasSelection())
            cmboxSearch->lineEdit()->setText(document->selectedText());
        cmboxSearch->lineEdit()->setFocus();
        cmboxSearch->lineEdit()->selectAll();
    }
    return QObject::eventFilter(o, e);
}

void BSearchDialogPrivate::init()
{
    B_Q(BSearchDialog);
    document = 0;
    q->installEventFilter(this);
    //
    QVBoxLayout *vlt = new QVBoxLayout(q);
      QHBoxLayout *hltSearch = new QHBoxLayout;
        lblSearch = new QLabel(q);
        hltSearch->addWidget(lblSearch);
        cmboxSearch = new QComboBox(q);
          cmboxSearch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
          cmboxSearch->setMaxCount(DefMaximumHistorySize);
          cmboxSearch->setMaxVisibleItems(DefMaximumHistorySize);
          cmboxSearch->setInsertPolicy(QComboBox::InsertAtTop);
          cmboxSearch->setEditable(true);
          cmboxSearch->lineEdit()->completer()->setCaseSensitivity(Qt::CaseSensitive);
          connect(cmboxSearch, SIGNAL(editTextChanged(QString)), this, SLOT(checkSearchReplace()));
        hltSearch->addWidget(cmboxSearch);
      vlt->addLayout(hltSearch);
      QHBoxLayout *hltReplace = new QHBoxLayout;
        lblReplace = new QLabel(q);
        hltReplace->addWidget(lblReplace);
        cmboxReplace = new QComboBox(q);
          cmboxReplace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
          cmboxReplace->setMaxCount(DefMaximumHistorySize);
          cmboxReplace->setMaxVisibleItems(DefMaximumHistorySize);
          cmboxReplace->setInsertPolicy(QComboBox::InsertAtTop);
          cmboxReplace->setEditable(true);
          cmboxReplace->lineEdit()->completer()->setCaseSensitivity(Qt::CaseSensitive);
        hltReplace->addWidget(cmboxReplace);
      vlt->addLayout(hltReplace);
      gboxOptions = new QGroupBox(q);
        QVBoxLayout *vltOptions = new QVBoxLayout;
          cboxRegexp = new QCheckBox;
          vltOptions->addWidget(cboxRegexp);
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
        connect(dlgbbox->button(QDialogButtonBox::Close), SIGNAL(clicked()), q, SLOT(close()));
        btnReplaceAll = new QPushButton;
          mnuReplaceAll = new QMenu(q);
            actSelection = new QAction(this);
              connect(actSelection, SIGNAL(triggered()), this, SLOT(actSelectionTriggered()));
            mnuReplaceAll->addAction(actSelection);
            actDocument = new QAction(this);
              connect(actDocument, SIGNAL(triggered()), this, SLOT(actDocumentTriggered()));
            mnuReplaceAll->addAction(actDocument);
          btnReplaceAll->setMenu(mnuReplaceAll);
        dlgbbox->addButton(btnReplaceAll, QDialogButtonBox::ActionRole);
        btnReplace = new QPushButton;
          connect(btnReplace, SIGNAL(clicked()), q, SLOT(replaceNext()));
        dlgbbox->addButton(btnReplace, QDialogButtonBox::ApplyRole);
        btnFind = new QPushButton;
          btnFind->setDefault(true);
          connect(btnFind, SIGNAL(clicked()), q, SLOT(findNext()));
        dlgbbox->addButton(btnFind, QDialogButtonBox::ApplyRole);
      vlt->addWidget(dlgbbox);
    //
    checkSearchReplace();
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

QString BSearchDialogPrivate::windowTitle() const
{
    return cmboxReplace->isVisible() ? tr("Find and replace", "windowTitle") : tr("Find", "windowTitle");
}

/*============================== Public slots ==============================*/

void BSearchDialogPrivate::actDocumentTriggered()
{
    QString oldText = cmboxSearch->lineEdit()->text();
    QString newText = cmboxReplace->lineEdit()->text();
    int count = cboxRegexp->isChecked() ?
                document->replaceInDocumentRegexp(QRegExp(oldText, q_func()->caseSensitivity()), newText) :
                document->replaceInDocument(oldText, newText, createFindFlags());
    if (count > 0) {
        appendHistory(cmboxSearch);
        appendHistory(cmboxReplace);
    }
    emitTextReplaced(count, oldText, newText);
}

void BSearchDialogPrivate::actSelectionTriggered()
{
    QString oldText = cmboxSearch->lineEdit()->text();
    QString newText = cmboxReplace->lineEdit()->text();
    int count = cboxRegexp->isChecked() ?
                document->replaceInSelectionRegexp(QRegExp(oldText, q_func()->caseSensitivity()), newText) :
                document->replaceInSelection(oldText, newText, createFindFlags());
    if (count > 0) {
        appendHistory(cmboxSearch);
        appendHistory(cmboxReplace);
    }
    emitTextReplaced(count, oldText, newText);
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
        QMetaObject::invokeMethod(q_func(), "findNextAvailableChanged", Q_ARG(bool, btnFind->isEnabled()));
    if (btnReplace->isEnabled() != rnap)
        QMetaObject::invokeMethod(q_func(), "replaceNextAvailableChanged", Q_ARG(bool, btnReplace->isEnabled()));
}

void BSearchDialogPrivate::retranslateUi()
{
    q_func()->setWindowTitle(windowTitle());
    lblSearch->setText(tr("Find:", "lbl text"));
    lblReplace->setText(tr("Replace with:", "lbl text"));
    gboxOptions->setTitle(tr("Options", "gbox title"));
    cboxRegexp->setText(tr("Treat as regular expression", "cbox text"));
    cboxCaseSensitive->setText(tr("Case sensitive", "cbox text"));
    cboxWholeWords->setText(tr("Whole words only", "cbox text"));
    cboxBackwardOrder->setText(tr("Reverse search", "cbox text"));
    cboxCyclic->setText(tr("Cyclic search", "cbox text"));
    btnReplaceAll->setText(tr("Replace all", "btn text"));
    actSelection->setText(tr("in selection", "act text"));
    actDocument->setText(tr("in document", "act text"));
    btnReplace->setText(tr("Replace", "btn text"));
    btnFind->setText(tr("Find", "btn text"));
}

/*============================================================================
================================ BSearchDialog ===============================
============================================================================*/

/*============================== Public constructors =======================*/

BSearchDialog::BSearchDialog(QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSearchDialogPrivate(this))
{
    d_func()->init();
}

BSearchDialog::BSearchDialog(bool replaceEnabled, QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSearchDialogPrivate(this))
{
    d_func()->init();
    setReplaceEnabled(replaceEnabled);
}

BSearchDialog::~BSearchDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSearchDialog::BSearchDialog(BSearchDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BSearchDialog::backwardOrder() const
{
    return d_func()->cboxBackwardOrder->isChecked();
}

Qt::CaseSensitivity BSearchDialog::caseSensitivity() const
{
    return d_func()->cboxCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

bool BSearchDialog::cyclicSearch() const
{
    return d_func()->cboxCyclic->isChecked();
}

BAbstractCodeEditorDocument *BSearchDialog::document() const
{
    return d_func()->document;
}

bool BSearchDialog::findNextAvailable() const
{
    return d_func()->btnFind->isEnabled();
}

bool BSearchDialog::matchWholeWords() const
{
    return d_func()->cboxWholeWords->isChecked();
}

int BSearchDialog::maximumHistorySize() const
{
    return d_func()->cmboxSearch->maxCount();
}

bool BSearchDialog::regexp() const
{
    return d_func()->cboxRegexp->isChecked();
}

bool BSearchDialog::replaceEnabled() const
{
    return d_func()->cmboxReplace->isVisible();
}

QStringList BSearchDialog::replaceHistory() const
{
    QStringList sl;
    const B_D(BSearchDialog);
    for (int i = 0; i < d->cmboxReplace->count(); ++i)
        sl << d->cmboxReplace->itemText(i);
    return sl;
}

bool BSearchDialog::replaceNextAvailable() const
{
    return d_func()->btnReplace->isEnabled();
}

void BSearchDialog::restoreState(const QByteArray &ba)
{
    QVariantMap m = BeQt::deserialize(ba).toMap();
    setRegexp(m.value("regexp").toBool());
    setCaseSensitivity(enum_cast<Qt::CaseSensitivity>(m.value("case_sensitivity"), Qt::CaseInsensitive,
                                                      Qt::CaseSensitive));
    setMatchWholeWords(m.value("match_whole_words").toBool());
    setBackwardOrder(m.value("backward_order").toBool());
    setCyclicSearch(m.value("cyclic_search", true).toBool());
    setSearchHistory(m.value("search_history").toStringList());
    setReplaceHistory(m.value("replace_history").toStringList());
}

QByteArray BSearchDialog::saveState() const
{
    QVariantMap m;
    m.insert("regexp", regexp());
    m.insert("case_sensitivity", (int) caseSensitivity());
    m.insert("match_whole_words", matchWholeWords());
    m.insert("backward_order", backwardOrder());
    m.insert("cyclic_search", cyclicSearch());
    m.insert("search_history", searchHistory());
    m.insert("replace_history", replaceHistory());
    return BeQt::serialize(m);
}

QStringList BSearchDialog::searchHistory() const
{
    QStringList sl;
    const B_D(BSearchDialog);
    for (int i = 0; i < d->cmboxSearch->count(); ++i)
        sl << d->cmboxSearch->itemText(i);
    return sl;
}

void BSearchDialog::setBackwardOrder(bool enabled)
{
    d_func()->cboxBackwardOrder->setChecked(enabled);
}

void BSearchDialog::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    d_func()->cboxCaseSensitive->setChecked(Qt::CaseSensitive == cs);
}

void BSearchDialog::setCyclicSearch(bool enabled)
{
    d_func()->cboxCyclic->setChecked(enabled);
}

void BSearchDialog::setDocument(BAbstractCodeEditorDocument *doc)
{
    B_D(BSearchDialog);
    if (d->document)
        disconnect(d->document, SIGNAL(hasSelectionChanged(bool)), d, SLOT(checkSearchReplace()));
    if (doc) {
        BCodeEditorDocument *ddoc = qobject_cast<BCodeEditorDocument *>(doc);
        if (ddoc) {
            d->cmboxSearch->lineEdit()->setMaxLength(ddoc->editLineLength());
            d->cmboxReplace->lineEdit()->setMaxLength(ddoc->editLineLength());
        }
        connect(doc, SIGNAL(hasSelectionChanged(bool)), d, SLOT(checkSearchReplace()));
    }
    d->document = doc;
    d->checkSearchReplace();
}

void BSearchDialog::setMatchWholeWords(bool enabled)
{
    d_func()->cboxWholeWords->setChecked(enabled);
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

void BSearchDialog::setRegexp(bool b)
{
    d_func()->cboxRegexp->setChecked(b);
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

void BSearchDialog::setReplaceHistory(const QStringList &list)
{
    B_D(BSearchDialog);
    d->cmboxReplace->clear();
    d->cmboxReplace->addItems(list);
}

void BSearchDialog::setSearchHistory(const QStringList &list)
{
    B_D(BSearchDialog);
    d->cmboxSearch->clear();
    d->cmboxSearch->addItems(list);
}

/*============================== Public slots ==============================*/

void BSearchDialog::findNext()
{
    if (!findNextAvailable())
        return;
    B_D(BSearchDialog);
    QString text = d->cmboxSearch->lineEdit()->text();
    bool b = d->cboxRegexp->isChecked() ? d->document->findNextRegexp(QRegExp(text, caseSensitivity()),
                                                                      d->createFindFlags(), cyclicSearch()) :
                                          d->document->findNext(text, d->createFindFlags(), cyclicSearch());
    if (b)
        d->appendHistory(d->cmboxSearch);
    Q_EMIT textFound(b, text);
}

void BSearchDialog::replaceNext()
{
    if (!replaceNextAvailable())
        return;
    B_D(BSearchDialog);
    QString text = d->cmboxSearch->lineEdit()->text();
    QString ntext = d->cmboxReplace->lineEdit()->text();
    bool b = d->document->replaceNext(ntext);
    if (b)
        d->appendHistory(d->cmboxReplace);
    Q_EMIT textReplaced(b, text, ntext);
    if (d->cboxRegexp->isChecked())
        d->document->findNextRegexp(QRegExp(text, caseSensitivity()), d->createFindFlags(), cyclicSearch());
    else
        d->document->findNext(text, d->createFindFlags(), cyclicSearch());
}

/*============================================================================
================================ BSearchEditorModulePrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BSearchEditorModulePrivate::BSearchEditorModulePrivate(BSearchEditorModule *q) :
    BAbstractEditorModulePrivate(q)
{
    //
}

BSearchEditorModulePrivate::~BSearchEditorModulePrivate()
{
    //
}

/*============================== Public methods ============================*/

QString BSearchEditorModulePrivate::createNotFoundMessage(const QString &text)
{
    QString t = (text.length() > 50) ? (text.left(47) + "...") : text;
    QString m1 = sdlg->regexp() ? tr("Expression", "msg text") : tr("Text", "msg text");
    QString m2 = sdlg->regexp() ? tr("not found", "msg text (regexp)") : tr("not found", "msg text (text)");
    return m1 + " \"" + t + "\" " + m2;
}

void BSearchEditorModulePrivate::init()
{
    sdlg = new BSearchDialog;
    connect(sdlg, SIGNAL(textFound(bool, QString)), this, SLOT(textFound(bool, QString)));
    connect(sdlg, SIGNAL(textReplaced(int, QString, QString)), this, SLOT(textReplaced(int, QString, QString)));
    //
    B_Q(BSearchEditorModule);
    actFind = new QAction(this);
      actFind->setEnabled(false);
      actFind->setIcon(BApplication::icon("find"));
      actFind->setShortcut(QKeySequence::Find);
      connect(actFind, SIGNAL(triggered()), q, SLOT(find()));
    actFindNext = new QAction(this);
      actFindNext->setEnabled(false);
      actFindNext->setIcon(BApplication::icon("edit_find_next"));
      actFindNext->setShortcut(QKeySequence::FindNext);
      connect(actFindNext, SIGNAL(triggered()), sdlg, SLOT(findNext()));
      connect(sdlg, SIGNAL(findNextAvailableChanged(bool)), actFindNext, SLOT(setEnabled(bool)));
    //
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

void BSearchEditorModulePrivate::setDialogParent(QWidget *parent)
{
    Qt::WindowFlags flags = sdlg->windowFlags();
    sdlg->setParent(parent);
    sdlg->setWindowFlags(flags);
}

/*============================== Public slots ==============================*/

void BSearchEditorModulePrivate::retranslateUi()
{
    if (!actFind.isNull()) {
        actFind->setText(tr("Find and replace...", "act text"));
        actFind->setToolTip(tr("Find and replace text", "act toolTip"));
        actFind->setWhatsThis(tr("Use this action to open a find and replace dialog", "act whatsThis"));
    }
    if (!actFindNext.isNull()) {
        actFindNext->setText(tr("Find next", "act text"));
        actFindNext->setToolTip(tr("Find next", "act toolTip"));
        actFindNext->setWhatsThis(tr("Use this action to find next occurrence of the text", "act whatsThis"));
    }
}

void BSearchEditorModulePrivate::textFound(bool found, const QString &text)
{
    B_Q(BSearchEditorModule);
    if (found) {
        QMetaObject::invokeMethod(q, "textFound", Q_ARG(QString, text));
    } else {
        QMetaObject::invokeMethod(q, "textNotFound", Q_ARG(QString, text));
        QMetaObject::invokeMethod(q, "message", Q_ARG(QString, createNotFoundMessage(text)));
    }
}

void BSearchEditorModulePrivate::textReplaced(int count, const QString &oldText, const QString &newText)
{
    B_Q(BSearchEditorModule);
    if (count > 0) {
        QMetaObject::invokeMethod(q, "textReplaced", Q_ARG(int, count), Q_ARG(QString, oldText),
                                  Q_ARG(QString, newText));
        if (count > 1) {
            QString msg = tr("Found and replaced", "msg text") + " "
                    + QString::number(count) + " " + tr("entries", "msg text");
            QMetaObject::invokeMethod(q, "message", Q_ARG(QString, msg));
        }
    } else {
        QMetaObject::invokeMethod(q, "textNotFound", Q_ARG(QString, oldText));
        QMetaObject::invokeMethod(q, "message", Q_ARG(QString, createNotFoundMessage(oldText)));
    }
}

/*============================================================================
================================ BSearchEditorModule =========================
============================================================================*/

/*============================== Public constructors =======================*/

BSearchEditorModule::BSearchEditorModule(QObject *parent) :
    BAbstractEditorModule(*new BSearchEditorModulePrivate(this), parent)
{
    d_func()->init();
}

BSearchEditorModule::~BSearchEditorModule()
{
    //
}

/*============================== Protected constructors ====================*/

BSearchEditorModule::BSearchEditorModule(BSearchEditorModulePrivate &d, QObject *parent) :
    BAbstractEditorModule(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QAction *BSearchEditorModule::action(int type)
{
    switch (type) {
    case FindAction:
        return d_func()->actFind;
    case FindNextAction:
        return d_func()->actFindNext;
    default:
        return 0;
    }
}

QList<QAction *> BSearchEditorModule::actions(bool)
{
    const B_D(BSearchEditorModule);
    QList<QAction *> list;
    if (!d->actFind.isNull())
        list << d->actFind;
    if (!d->actFindNext.isNull())
        list << d->actFindNext;
    return list;
}

bool BSearchEditorModule::backwardOrder() const
{
    return d_func()->sdlg->backwardOrder();
}

Qt::CaseSensitivity BSearchEditorModule::caseSensitivity() const
{
    return d_func()->sdlg->caseSensitivity();
}

bool BSearchEditorModule::cyclicSearch() const
{
    return d_func()->sdlg->cyclicSearch();
}

QString BSearchEditorModule::id() const
{
    return "beqt/search";
}

bool BSearchEditorModule::matchWholeWords() const
{
    return d_func()->sdlg->matchWholeWords();
}

int BSearchEditorModule::maximumHistorySize() const
{
    return d_func()->sdlg->maximumHistorySize();
}

bool BSearchEditorModule::regexp() const
{
    return d_func()->sdlg->regexp();
}

bool BSearchEditorModule::replaceEnabled() const
{
    return d_func()->sdlg->replaceEnabled();
}

void BSearchEditorModule::restoreState(const QByteArray &state)
{
    d_func()->sdlg->restoreState(state);
}

QByteArray BSearchEditorModule::saveState() const
{
    return d_func()->sdlg->saveState();
}

void BSearchEditorModule::setBackwardOrder(bool enabled)
{
    d_func()->sdlg->setBackwardOrder(enabled);
}

void BSearchEditorModule::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    d_func()->sdlg->setCaseSensitivity(cs);
}

void BSearchEditorModule::setCyclicSearch(bool enabled)
{
    d_func()->sdlg->setCyclicSearch(enabled);
}

void BSearchEditorModule::setMatchWholeWords(bool enabled)
{
    d_func()->sdlg->setMatchWholeWords(enabled);
}

void BSearchEditorModule::setMaximumHistorySize(int sz)
{
    d_func()->sdlg->setMaximumHistorySize(sz);
}

void BSearchEditorModule::setRegexp(bool b)
{
    d_func()->sdlg->setRegexp(b);
}

void BSearchEditorModule::setReplaceEnabled(bool enabled)
{
    d_func()->sdlg->setReplaceEnabled(enabled);
}

/*============================== Public slots ==============================*/

void BSearchEditorModule::find()
{
    B_D(BSearchEditorModule);
    if (d->sdlg->isVisible())
        d->sdlg->activateWindow();
    else
        d->sdlg->show();
}

void BSearchEditorModule::findNext(bool showDialog)
{
    if (showDialog)
        find();
    d_func()->sdlg->findNext();
}

/*============================== Protected methods =========================*/

void BSearchEditorModule::currentDocumentChanged(BAbstractCodeEditorDocument *doc)
{
    B_D(BSearchEditorModule);
    d->sdlg->setDocument(doc);
    if (!d->actFind.isNull())
        d->actFind->setEnabled(doc);
}

void BSearchEditorModule::editorSet(BCodeEditor *edr)
{
    B_D(BSearchEditorModule);
    BAbstractCodeEditorDocument *doc = edr ? edr->currentDocument() : 0;
    d->setDialogParent(edr);
    d->sdlg->setDocument(doc);
    if (!d->actFind.isNull())
        d->actFind->setEnabled(doc);
}

void BSearchEditorModule::editorUnset(BCodeEditor *)
{
    B_D(BSearchEditorModule);
    d->setDialogParent(0);
    d->sdlg->setDocument(0);
    if (!d->actFind.isNull())
        d->actFind->setEnabled(false);
}
