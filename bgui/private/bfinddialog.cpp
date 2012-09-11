class QWidget;

#include "bfinddialog.h"
#include "bguicommon.h"

#include "../bcore/bcore.h"

#include <QString>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSizePolicy>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QKeySequence>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QApplication>
#include <QVariant>
#include <QStringList>
#include <QTextDocument>
#include <QLineEdit>
#include <QShowEvent>

#include <QDebug>

const int _m_MaxHistorySize = 20;
//
//_m_GroupTextEditor
  const QString _m_GroupFindDialog = "find_dialog";
    const QString _m_KeyTextHistory = "text_history";
    const QString _m_KeyNewTextHistory = "new_text_history";
    const QString _m_GroupOptions = "options";
      const QString _m_KeyCaseSensitive = "case_sensitive";
      const QString _m_KeyWholeWords = "whole_words";
      const QString _m_KeyBackwardOrder = "backward_order";
      const QString _m_KeyCyclic = "cyclic";

//

BFindDialog::BFindDialog(const QString &id, QWidget *parent) :
    QDialog(parent), _m_CId(id)
{
    _m_documentAvailable = false;
    _m_selectionAvailable = false;
    _m_replaceAvailable = true;
    //
    _m_vlt = new QVBoxLayout(this);
      _m_vlt->setSizeConstraint(QVBoxLayout::SetFixedSize);
      _m_hltText = new QHBoxLayout;
        _m_lblText = new QLabel(this);
        _m_hltText->addWidget(_m_lblText);
        _m_cmboxText = new QComboBox(this);
          _m_cmboxText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
          _m_cmboxText->setMaxCount(_m_MaxHistorySize);
          _m_cmboxText->setInsertPolicy(QComboBox::InsertAtTop);
          _m_cmboxText->setEditable(true);
          _m_cmboxText->lineEdit()->setMaxLength(120);
          connect( _m_cmboxText, SIGNAL( editTextChanged(QString) ),
                   this, SLOT( _m_cmboxTextEditTextChanged(QString) ) );
        _m_hltText->addWidget(_m_cmboxText);
      _m_vlt->addLayout(_m_hltText);
      _m_hltNewText = new QHBoxLayout;
        _m_lblNewText = new QLabel(this);
        _m_hltNewText->addWidget(_m_lblNewText);
        _m_cmboxNewText = new QComboBox(this);
          _m_cmboxNewText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
          _m_cmboxNewText->setMaxCount(_m_MaxHistorySize);
          _m_cmboxNewText->setInsertPolicy(QComboBox::InsertAtTop);
          _m_cmboxNewText->setEditable(true);
          _m_cmboxNewText->lineEdit()->setMaxLength(120);
        _m_hltNewText->addWidget(_m_cmboxNewText);
      _m_vlt->addLayout(_m_hltNewText);
      _m_gboxOptions = new QGroupBox(this);
        _m_vltOptions = new QVBoxLayout;
          _m_cboxCaseSensitive = new QCheckBox;
          _m_vltOptions->addWidget(_m_cboxCaseSensitive);
          _m_cboxWholeWords = new QCheckBox;
          _m_vltOptions->addWidget(_m_cboxWholeWords);
          _m_cboxBackwardOrder = new QCheckBox;
          _m_vltOptions->addWidget(_m_cboxBackwardOrder);
          _m_cboxCyclic = new QCheckBox;
          _m_vltOptions->addWidget(_m_cboxCyclic);
        _m_gboxOptions->setLayout(_m_vltOptions);
      _m_vlt->addWidget(_m_gboxOptions);
      _m_hltActions = new QHBoxLayout;
        _m_hltActions->addStretch();
        _m_btnClose = new QPushButton(this);
          connect( _m_btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );
        _m_hltActions->addWidget(_m_btnClose);
        _m_btnReplaceAll = new QPushButton(this);
          _m_btnReplaceAll->setEnabled(false);
          _m_mnuReplaceAll = new QMenu(this);
            _m_actSelection = new QAction(this);
              _m_actSelection->setEnabled(false);
              connect( _m_actSelection, SIGNAL( triggered() ), this, SIGNAL( replaceInSelection() ) );
              connect( _m_actSelection, SIGNAL( triggered() ), this, SLOT( _m_appendTextHistory() ) );
              connect( _m_actSelection, SIGNAL( triggered() ), this, SLOT( _m_appendNewTextHistory() ) );
            _m_mnuReplaceAll->addAction(_m_actSelection);
            _m_actDocument = new QAction(this);
              connect( _m_actDocument, SIGNAL( triggered() ), this, SIGNAL( replaceInDocument() ) );
              connect( _m_actDocument, SIGNAL( triggered() ), this, SLOT( _m_appendTextHistory() ) );
              connect( _m_actDocument, SIGNAL( triggered() ), this, SLOT( _m_appendNewTextHistory() ) );
            _m_mnuReplaceAll->addAction(_m_actDocument);
            _m_actAllDocuments = new QAction(this);
              connect( _m_actAllDocuments, SIGNAL( triggered() ), this, SIGNAL( replaceInAllDocuments() ) );
              connect( _m_actAllDocuments, SIGNAL( triggered() ), this, SLOT( _m_appendTextHistory() ) );
              connect( _m_actAllDocuments, SIGNAL( triggered() ), this, SLOT( _m_appendNewTextHistory() ) );
            _m_mnuReplaceAll->addAction(_m_actAllDocuments);
          _m_btnReplaceAll->setMenu(_m_mnuReplaceAll);
        _m_hltActions->addWidget(_m_btnReplaceAll);
        _m_btnReplace = new QPushButton(this);
          _m_btnReplace->setEnabled(false);
          connect( _m_btnReplace, SIGNAL( clicked() ), this, SIGNAL( replaceNext() ) );
          connect( _m_actSelection, SIGNAL( triggered() ), this, SLOT( _m_appendTextHistory() ) );
          connect( _m_btnReplace, SIGNAL( clicked() ), this, SLOT( _m_appendNewTextHistory() ) );
        _m_hltActions->addWidget(_m_btnReplace);
        _m_btnFind = new QPushButton(this);
          _m_btnFind->setDefault(true);
          _m_btnFind->setEnabled(false);
          _m_btnFind->setDefault(true);
          connect( _m_btnFind, SIGNAL( clicked() ), this, SIGNAL( findNext() ) );
          connect( _m_btnFind, SIGNAL( clicked() ), this, SLOT( _m_appendTextHistory() ) );
        _m_hltActions->addWidget(_m_btnFind);
      _m_vlt->addLayout(_m_hltActions);
    _m_loadSettings();
    _m_retranslateUi();
}

//

void BFindDialog::setDocumentAvailable(bool available)
{
    _m_documentAvailable = available;
    _m_check();
}

void BFindDialog::setSelectionAvailable(bool available)
{
    _m_selectionAvailable = available;
    _m_check();
}

void BFindDialog::setText(const QString &text)
{
    _m_cmboxText->lineEdit()->setText(text);
}

void BFindDialog::setReplaceAvailable(bool available)
{
    _m_replaceAvailable = available;
    _m_check();
}

void BFindDialog::setLineLength(int length)
{
    if (length < 10)
        return;
    _m_cmboxText->lineEdit()->setMaxLength(length);
    _m_cmboxNewText->lineEdit()->setMaxLength(length);
}

QString BFindDialog::text() const
{
    return _m_cmboxText->currentText();
}

QString BFindDialog::newText() const
{
    return _m_cmboxNewText->currentText();
}

Qt::CaseSensitivity BFindDialog::caseSensitivity() const
{
    return _m_cboxCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
}

QTextDocument::FindFlags BFindDialog::findFlags() const
{
    QTextDocument::FindFlags flags = 0;
    if ( _m_cboxCaseSensitive->isChecked() )
        flags |= QTextDocument::FindCaseSensitively;
    if ( _m_cboxWholeWords->isChecked() )
        flags |= QTextDocument::FindWholeWords;
    if ( _m_cboxBackwardOrder->isChecked() )
        flags |= QTextDocument::FindBackward;
    return flags;
}

bool BFindDialog::cyclic() const
{
    return _m_cboxCyclic->isChecked();
}

//

void BFindDialog::changeEvent(QEvent *event)
{
    if (!event || event->type() != QEvent::LanguageChange)
        return QDialog::changeEvent(event);
    _m_retranslateUi();
}

void BFindDialog::closeEvent(QCloseEvent *event)
{
    _m_saveSettings();
    QDialog::closeEvent(event);
}

void BFindDialog::showEvent(QShowEvent *event)
{
    QLineEdit *le = _m_cmboxText->lineEdit();
    le->setFocus();
    le->selectAll();
    QDialog::showEvent(event);

}

//

void BFindDialog::_m_retranslateUi()
{
    setWindowTitle( tr("Find and replace", "windowTitle") );
    _m_lblText->setText( tr("Find:", "lbl text") );
    _m_lblNewText->setText( tr("Replace with:", "lbl text") );
    _m_gboxOptions->setTitle( tr("Options", "gbox title") );
    _m_cboxCaseSensitive->setText( tr("Case sensitive", "cbox text") );
    _m_cboxWholeWords->setText( tr("Whole words only", "cbox text") );
    _m_cboxBackwardOrder->setText( tr("Reverse search", "cbox text") );
    _m_cboxCyclic->setText( tr("Cyclic search", "cbox text") );
    _m_btnClose->setText( tr("Close", "btn text") );
    _m_btnReplaceAll->setText( tr("Replace all", "btn text") );
    _m_actSelection->setText( tr("in selection", "act text") );
    _m_actDocument->setText( tr("in document", "act text") );
    _m_actAllDocuments->setText( tr("in all documents", "act text") );
    _m_btnReplace->setText( tr("Replace", "btn text") );
    _m_btnFind->setText( tr("Find", "btn text") );
}

void BFindDialog::_m_loadSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->beginGroup(_m_GroupFindDialog);
        _m_cmboxText->addItems( s->value(_m_KeyTextHistory).toStringList() );
        _m_cmboxNewText->addItems( s->value(_m_KeyNewTextHistory).toStringList() );
        s->beginGroup(_m_GroupOptions);
          _m_cboxCaseSensitive->setChecked( s->value(_m_KeyCaseSensitive).toBool() );
          _m_cboxWholeWords->setChecked( s->value(_m_KeyWholeWords).toBool() );
          _m_cboxBackwardOrder->setChecked( s->value(_m_KeyBackwardOrder).toBool() );
          _m_cboxCyclic->setChecked( s->value(_m_KeyCyclic).toBool() );
        s->endGroup();
      s->endGroup();
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

void BFindDialog::_m_saveSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->beginGroup(_m_GroupFindDialog);
        s->setValue( _m_KeyTextHistory, _m_textHistory() );
        s->setValue( _m_KeyNewTextHistory, _m_newTextHistory() );
        s->beginGroup(_m_GroupOptions);
          s->setValue( _m_KeyCaseSensitive, _m_cboxCaseSensitive->isChecked() );
          s->setValue( _m_KeyWholeWords, _m_cboxWholeWords->isChecked() );
          s->setValue( _m_KeyBackwardOrder, _m_cboxBackwardOrder->isChecked() );
          s->setValue( _m_KeyCyclic, _m_cboxCyclic->isChecked() );
        s->endGroup();
      s->endGroup();
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

void BFindDialog::_m_check()
{
    bool text = !_m_cmboxText->currentText().isEmpty();
    bool docsel = _m_documentAvailable && _m_selectionAvailable;
    _m_btnReplaceAll->setEnabled(_m_documentAvailable && text);
    _m_actDocument->setEnabled(_m_replaceAvailable);
    _m_actSelection->setEnabled(docsel && text && _m_replaceAvailable);
    _m_btnReplace->setEnabled(docsel && text && _m_replaceAvailable);
    _m_btnFind->setEnabled(_m_documentAvailable && text);
    findNextAvailableChanged( _m_btnFind->isEnabled() );
    replaceNextAvailableChanged( _m_btnReplace->isEnabled() );
}

QStringList BFindDialog::_m_textHistory() const
{
    QStringList sl;
    for (int i = 0; i < _m_cmboxText->count(); ++i)
        sl << _m_cmboxText->itemText(i);
    return sl;
}

QStringList BFindDialog::_m_newTextHistory() const
{
    QStringList sl;
    for (int i = 0; i < _m_cmboxNewText->count(); ++i)
        sl << _m_cmboxNewText->itemText(i);
    return sl;
}

//

void BFindDialog::_m_cmboxTextEditTextChanged(const QString &text)
{
    _m_check();
}

void BFindDialog::_m_appendTextHistory()
{
    QString text = _m_cmboxText->currentText();
    if ( text.isEmpty() )
        return;
    int index = _m_cmboxText->findText(text);
    if (index >= 0)
        _m_cmboxText->removeItem(index);
    else if (_m_cmboxText->count() >= _m_MaxHistorySize)
        _m_cmboxText->removeItem(_m_MaxHistorySize - 1);
    _m_cmboxText->insertItem(0, text);
    _m_cmboxText->setCurrentIndex(0);
}

void BFindDialog::_m_appendNewTextHistory()
{
    QString text = _m_cmboxNewText->currentText();
    if ( text.isEmpty() )
        return;
    int index = _m_cmboxNewText->findText(text);
    if (index >= 0)
        _m_cmboxNewText->removeItem(index);
    else if (_m_cmboxNewText->count() >= _m_MaxHistorySize)
        _m_cmboxNewText->removeItem(_m_MaxHistorySize - 1);
    _m_cmboxNewText->insertItem(0, text);
    _m_cmboxNewText->setCurrentIndex(0);
}
