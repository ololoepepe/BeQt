class QWidget;

#include "bselectfilesdialog.h"
#include "btexteditordocument.h"
#include "bguicommon.h"

#include "../bcore/bcore.h"

#include <QRect>
#include <QString>
#include <QDialog>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVariant>
#include <QSettings>
#include <QApplication>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QFont>

const QRect _m_DefGeometry = QRect(200, 200, 600, 400);
//
//_m_GroupTextEditor
  const QString _m_GroupSelectFilesDialog = "select_files_dialog";
    const QString _m_KeyGeometry = "geometry";

//

BSelectFilesDialog::BSelectFilesDialog(const QString &id, const QList<BTextEditorDocument *> &documents, QWidget *parent) :
    QDialog(parent), _m_CId(id)
{
    _m_documents = documents;
    _m_decision = CancelDecision;
    //
    _m_vlt = new QVBoxLayout(this);
      _m_lblText = new QLabel(this);
        QFont fnt = _m_lblText->font();
        fnt.setBold(true);
        fnt.setPointSize(14);
        _m_lblText->setFont(fnt);
        _m_lblText->setWordWrap(true);
        _m_lblText->setText( tr("Some documents have unsaved changes. Do you want to save them before closing?",
                                "lbl text") );
      _m_vlt->addWidget(_m_lblText);
      _m_lblInformativeTextUpper = new QLabel(this);
        _m_lblInformativeTextUpper->setWordWrap(true);
        _m_lblInformativeTextUpper->setText( tr("Choose documents that you wish to save:", "lbl text") );
      _m_vlt->addWidget(_m_lblInformativeTextUpper);
      _m_lstwgt = new QListWidget(this);
        for (int i = 0; i < _m_documents.size(); ++i)
        {
            QString fn = _m_documents.at(i)->fileName();
            QListWidgetItem *lwi = new QListWidgetItem( QFileInfo(fn).fileName() );
            lwi->setData(Qt::ToolTipRole, fn);
            lwi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            lwi->setCheckState(Qt::Checked);
            _m_lstwgt->addItem(lwi);
        }
      _m_vlt->addWidget(_m_lstwgt);
      _m_lblInformativeTextLower = new QLabel(this);
        _m_lblInformativeTextLower->setWordWrap(true);
        _m_lblInformativeTextLower->setText( tr("If you don't save the documents, all changes will be lost.",
                                                "lbl text") );
      _m_vlt->addWidget(_m_lblInformativeTextLower);
      _m_hlt = new QHBoxLayout;
        _m_hlt->addStretch();
        _m_btnDiscard = new QPushButton(this);
          _m_btnDiscard->setFocus();
          _m_btnDiscard->setText( tr("Close without saving", "btn text") );
          connect( _m_btnDiscard, SIGNAL( clicked() ), this, SLOT( _m_btnDiscardClicked() ) );
        _m_hlt->addWidget(_m_btnDiscard);
        _m_btnCancel = new QPushButton(this);
          _m_btnCancel->setText( tr("Cancel", "btn text") );
          connect( _m_btnCancel, SIGNAL( clicked() ), this, SLOT( _m_btnCancelClicked() ) );
        _m_hlt->addWidget(_m_btnCancel);
        _m_btnSave = new QPushButton(this);
          _m_btnSave->setText( tr("Save", "btn text") );
          connect( _m_btnSave, SIGNAL( clicked() ), this, SLOT( _m_btnSaveClicked() ) );
        _m_hlt->addWidget(_m_btnSave);
      _m_vlt->addLayout(_m_hlt);
    _m_loadSettings();
}

//

BSelectFilesDialog::Decision BSelectFilesDialog::decision() const
{
    return _m_decision;
}

QList<BTextEditorDocument *> BSelectFilesDialog::selectedDocuments() const
{
    QList<BTextEditorDocument *> tedl;
    for (int i = 0; i < _m_lstwgt->count(); ++i)
        if (_m_lstwgt->item(i)->checkState() == Qt::Checked)
            tedl << _m_documents.at(i);
    return tedl;
}

//

void BSelectFilesDialog::_m_loadSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->beginGroup(_m_GroupSelectFilesDialog);
        setGeometry( s->value(_m_KeyGeometry, _m_DefGeometry).toRect() );
      s->endGroup();
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

void BSelectFilesDialog::_m_saveSettings()
{
    QScopedPointer<QSettings> s( BCore::newSettingsInstance() );
    if (!s)
        return;
    if ( !_m_CId.isEmpty() )
        s->beginGroup(_m_CId);
    s->beginGroup(_m_GroupTextEditor);
      s->beginGroup(_m_GroupSelectFilesDialog);
        s->setValue( _m_KeyGeometry, geometry() );
      s->endGroup();
    s->endGroup();
    if ( !_m_CId.isEmpty() )
        s->endGroup();
}

//

void BSelectFilesDialog::_m_btnDiscardClicked()
{
    _m_decision = DiscardDecision;
    _m_saveSettings();
    accept();
}

void BSelectFilesDialog::_m_btnCancelClicked()
{
    _m_decision = CancelDecision;
    _m_saveSettings();
    reject();
}

void BSelectFilesDialog::_m_btnSaveClicked()
{
    _m_decision = SaveDecision;
    _m_saveSettings();
    accept();
}
