#include "bsplittedlinesdialog.h"

#include <QDialog>
#include <QList>
#include <QVBoxLayout>
#include <QListWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <QVariant>
#include <QString>
#include <QCheckBox>
#include <QtAlgorithms>

BSplittedLinesDialog::BSplittedLinesDialog(const QList<int> &lineIndexes, QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle( tr("Splitted lines", "windowTitle") );
    _m_vlt = new QVBoxLayout(this);
      _m_lbl = new QLabel(this);
        _m_lbl->setWordWrap(true);
        _m_lbl->setText( tr("Some lines in this document were too long, "
                            "so they were splitted into several lines.", "lbl text") );
      _m_vlt->addWidget(_m_lbl);
      _m_lstwgt = new QListWidget(this);
        for (int i = 0; i < lineIndexes.size(); ++i)
        {
            int ind = lineIndexes.at(i);
            QListWidgetItem *lwi = new QListWidgetItem;
            lwi->setData(Qt::UserRole, ind);
            lwi->setText( tr("Line:", "lstwgti text") + " " + QString::number(ind + 1) );
            _m_lstwgt->addItem(lwi);
        }
        _m_lstwgt->setToolTip( tr("Doublecick an item to go to the corresponding line.", "lstwgt toolTip") );
        connect( _m_lstwgt, SIGNAL( itemDoubleClicked(QListWidgetItem *) ),
                 this, SLOT( _m_lstwgtItemDoubleClicked(QListWidgetItem *) ) );
      _m_vlt->addWidget(_m_lstwgt);
      _m_cbox = new QCheckBox(this);
        _m_cbox->setChecked(true);
        _m_cbox->setText( tr("Select text when going to line", "cbox text") );
      _m_vlt->addWidget(_m_cbox);
      _m_hlt = new QHBoxLayout;
        _m_hlt->addStretch();
        _m_btnGoto = new QPushButton(this);
          _m_btnGoto->setDefault(true);
          _m_btnGoto->setText( tr("Go to line", "btn text") );
          connect( _m_btnGoto, SIGNAL( clicked() ), this, SLOT( _m_btnGotoClicked() ) );
        _m_hlt->addWidget(_m_btnGoto);
        _m_btnClose = new QPushButton(this);
          _m_btnClose->setText( tr("Close", "btn text") );
          connect( _m_btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );
        _m_hlt->addWidget(_m_btnClose);
      _m_vlt->addLayout(_m_hlt);
}

//

void BSplittedLinesDialog::_m_btnGotoClicked()
{
    _m_lstwgtItemDoubleClicked( _m_lstwgt->currentItem() );
}

void BSplittedLinesDialog::_m_lstwgtItemDoubleClicked(QListWidgetItem *item)
{
    if (item)
        emit gotoLine( item->data(Qt::UserRole).toInt(), _m_cbox->isChecked() );
}
