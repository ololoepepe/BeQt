#include "bsettingsdialog.h"
#include "bsettingsdialog_p.h"
#include "babstractsettingstab.h"

#include <BeQtCore/private/bbase_p.h>

#include <QString>
#include <QDialog>
#include <QVBoxLayout>
#include <QList>
#include <QMap>
#include <QVariantMap>
#include <QTabWidget>
#include <QLabel>
#include <QCoreApplication>
#include <QListWidget>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QSplitter>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStringList>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsDialogPrivate::BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs,
                                               BSettingsDialog::Navigation navigation, const QStringList &tabOrder) :
    BBasePrivate(q), TabMap(tabs), Navigation(navigation), TabOrder(tabOrder)
{
    //
}

BSettingsDialogPrivate::~BSettingsDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BSettingsDialogPrivate::init()
{
    valid = !TabMap.isEmpty();
    B_Q(BSettingsDialog);
    q->setWindowTitle( tr("Settings", "windowTitle") );
    vlt = new QVBoxLayout(q);
      QHBoxLayout *hlt = new QHBoxLayout;
        btnRestoreDefault = new QPushButton(q);
          btnRestoreDefault->setText( tr("Restore default settings", "btn text") );
          connect( btnRestoreDefault, SIGNAL( clicked() ), this, SLOT( btnRestoreDefaultClicked() ) );
        hlt->addWidget(btnRestoreDefault);
        cboxAdvancedMode = new QCheckBox(q);
          cboxAdvancedMode->setText( tr("Show additional settings", "cbox text") );
          connect( cboxAdvancedMode, SIGNAL( stateChanged(int) ), this, SLOT( cboxAdvancedModeStateChanged(int) ) );
          cboxAdvancedMode->setEnabled(false);
          foreach (BAbstractSettingsTab *t, TabMap)
          {
              if ( t->hasAdvancedMode() )
              {
                  cboxAdvancedMode->setEnabled(true);
                  break;
              }
          }
        hlt->addWidget(cboxAdvancedMode);
        hlt->addStretch();
      vlt->addLayout(hlt);
    if (TabMap.size() > 1)
    {
        QStringList keys = TabMap.keys();
        foreach ( int i, bRange(TabOrder.size() - 1, 0) )
        {
            const QString &key = TabOrder.at(i);
            if ( !keys.removeAll(key) )
                continue;
            keys.prepend(key);
        }
        if (BSettingsDialog::ListNavigation == Navigation)
        {
            hspltr = new QSplitter(Qt::Horizontal, q);
            lstwgt = new QListWidget(q);
            hspltr->addWidget(lstwgt);
            stkdwgt = new QStackedWidget(q);
              connect( lstwgt, SIGNAL( currentRowChanged(int) ), stkdwgt, SLOT( setCurrentIndex(int) ) );
            hspltr->addWidget(stkdwgt);
            vlt->addWidget(hspltr);
            twgt = 0;

            foreach (const QString &key, keys)
            {
                BAbstractSettingsTab *tab = TabMap.value(key);
                stkdwgt->addWidget(tab);
                QListWidgetItem *lwi = new QListWidgetItem;
                lwi->setText( tab->title() );
                lwi->setIcon( tab->icon() );
                lstwgt->addItem(lwi);
            }
            lstwgt->setCurrentRow(0);
        }
        else
        {
            hspltr = 0;
            lstwgt = 0;
            stkdwgt = 0;
            twgt = new QTabWidget(q);
            vlt->addWidget(twgt);
            foreach (const QString &key, keys)
            {
                BAbstractSettingsTab *tab = TabMap.value(key);
                twgt->addTab( tab, tab->icon(), tab->title() );
            }
        }
    }
    else if ( !TabMap.isEmpty() )
    {
        BAbstractSettingsTab *tab = TabMap.value( TabMap.keys().first() );
        vlt->addWidget(tab);
        hspltr = 0;
        lstwgt = 0;
        stkdwgt = 0;
        twgt = 0;
        q->setWindowTitle( q->windowTitle() + ": " + tab->title() );
    }
    else
    {
        hspltr = 0;
        lstwgt = 0;
        stkdwgt = 0;
        twgt = 0;
        dlgbbox = 0;
        q->setWindowTitle(q->windowTitle() + " [" + tr("invalid", "windowTitle") + "]");
        return;
    }
    dlgbbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, q);
      dlgbbox->button(QDialogButtonBox::Ok)->setDefault(true);
      connect( dlgbbox, SIGNAL( accepted() ), this, SLOT( accepted() ) );
      connect( dlgbbox, SIGNAL( rejected() ), q, SLOT( reject() ) );
    vlt->addWidget(dlgbbox);
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

/*============================== Public slots ==============================*/

void BSettingsDialogPrivate::accepted()
{
    foreach ( BAbstractSettingsTab *t, TabMap.values() )
        if ( !t->preconfirm() )
            return;
    q_func()->accept();
}

void BSettingsDialogPrivate::cboxAdvancedModeStateChanged(int state)
{
    bool b = (Qt::Checked == state);
    foreach (BAbstractSettingsTab *t, TabMap)
        t->setAdvancedMode(b);
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

void BSettingsDialogPrivate::btnRestoreDefaultClicked()
{
    QMessageBox msg( q_func() );
    msg.setWindowTitle( tr("", "msgbox windowTitle") );
    msg.setIcon(QMessageBox::Question);
    msg.setText( tr("You are about to restore the application settings to their default state", "msgbox text") );
    msg.setInformativeText( tr("All settings changes will be discarded. Do you want to continue?",
                               "msgbox informativeText") );
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    msg.setDefaultButton(QMessageBox::Yes);
    if (msg.exec() == QMessageBox::Yes)
        foreach (BAbstractSettingsTab *t, TabMap)
            if ( !t->restoreDefault() )
                break;
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

/*============================================================================
================================ BSettingsDialog =============================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs) )
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, Navigation navigation, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, navigation) )
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, const QStringList &tabOrder, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, ListNavigation, tabOrder) )
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, Navigation navigation,
                                 const QStringList &tabOrder, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, navigation, tabOrder) )
{
    d_func()->init();
}

BSettingsDialog::~BSettingsDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSettingsDialog::BSettingsDialog(BSettingsDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BSettingsDialog::isValid() const
{
    return d_func()->valid;
}

BSettingsDialog::SettingsMap BSettingsDialog::settingsMap() const
{
    SettingsMap m;
    const BSettingsDialogPrivate *const d = d_func();
    foreach ( const QString &key, d->TabMap.keys() )
        m.insert( key, d->TabMap.value(key)->valueMap() );
    return m;
}
