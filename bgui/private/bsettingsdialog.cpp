#include "bsettingsdialog.h"
#include "../babstractsettingstab.h"

#include <QString>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QList>
#include <QMap>
#include <QVariantMap>
#include <QTabWidget>
#include <QLabel>

const QString IdSingle = "single";

//

BSettingsDialog::BSettingsDialog(const QMap<QString, BAbstractSettingsTab *> &tabMap, QWidget *parent) :
    QDialog(parent)
{
    _m_tabMap = tabMap;
    //
    _m_vlt = new QVBoxLayout(this);
      _m_lbl = 0;
      _m_tabWgt = new QTabWidget(this);
      QList<QString> keys = _m_tabMap.keys();
      for (int i = 0; i < keys.size(); ++i)
      {
          BAbstractSettingsTab *tab = _m_tabMap.value( keys.at(i) );
          _m_tabWgt->addTab( tab, tab->title() );
      }
      _m_vlt->addWidget(_m_tabWgt);
      _m_initActions();
    //
    _m_retranslateUi();
}

BSettingsDialog::BSettingsDialog(BAbstractSettingsTab *tab, QWidget *parent) :
    QDialog(parent)
{
    _m_tabMap.insert(IdSingle, tab);
    //
    _m_vlt = new QVBoxLayout(this);
      _m_tabWgt = 0;
      _m_lbl = new QLabel(this);
        _m_lbl->setText( tab->title() );
      _m_vlt->addWidget(_m_lbl);
      _m_vlt->addWidget(tab);
      _m_initActions();
    //
    _m_retranslateUi();
}

//

QMap<QString, QVariantMap> BSettingsDialog::valueMapMap() const
{
    QMap<QString, QVariantMap> m;
    QList<QString> keys = _m_tabMap.keys();
    for (int i = 0; i < keys.size(); ++i)
    {
        const QString &key = keys.at(i);
        BAbstractSettingsTab *tab = _m_tabMap.value(key);
        m.insert( key, tab->valueMap() );
    }
    return m;
}

QVariantMap BSettingsDialog::valueMap() const
{
    if (_m_tabWgt || !_m_lbl)
        return QVariantMap();
    BAbstractSettingsTab *tab = _m_tabMap.value(IdSingle);
    return tab ? tab->valueMap() : QVariantMap();
}

//

void BSettingsDialog::_m_initActions()
{
    _m_hltActions = new QHBoxLayout(0);
      _m_hltActions->addStretch();
      _m_btnCancel = new QPushButton(this);
        connect( _m_btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
      _m_hltActions->addWidget(_m_btnCancel);
      _m_btnOk = new QPushButton(this);
        _m_btnOk->setDefault(true);
        connect( _m_btnOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
      _m_hltActions->addWidget(_m_btnOk);
    _m_vlt->addLayout(_m_hltActions);
}

void BSettingsDialog::_m_retranslateUi()
{
    setWindowTitle( tr("Settings", "windowTitle") );
    _m_btnCancel->setText( tr("Cancel", "btn text") );
    _m_btnOk->setText( tr("Ok", "btn text") );
}
