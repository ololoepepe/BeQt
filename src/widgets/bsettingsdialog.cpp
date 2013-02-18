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

BSettingsDialogPrivate::BSettingsDialogPrivate(BSettingsDialog *q, const QList<BAbstractSettingsTab *> &tabs,
                                               BSettingsDialog::Navigation navigation) :
    BBasePrivate(q), Tabs(tabs), Navigation(navigation)
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
    valid = !Tabs.isEmpty();
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
          foreach (BAbstractSettingsTab *t, Tabs)
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
    if (Tabs.size() > 1)
    {
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
            foreach (BAbstractSettingsTab *tab, Tabs)
            {
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
            foreach (BAbstractSettingsTab *tab, Tabs)
                twgt->addTab( tab, tab->icon(), tab->title() );
        }
    }
    else if ( !Tabs.isEmpty() )
    {
        BAbstractSettingsTab *tab = Tabs.first();
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
    previousSize = q->size();
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

/*============================== Public slots ==============================*/

void BSettingsDialogPrivate::accepted()
{
    foreach (BAbstractSettingsTab *t, Tabs)
        if ( !t->saveSettings() )
            return;
    q_func()->accept();
}

void BSettingsDialogPrivate::cboxAdvancedModeStateChanged(int state)
{
    bool b = (Qt::Checked == state);
    foreach (BAbstractSettingsTab *t, Tabs)
        t->setAdvancedMode(b);
    QSize sz = previousSize;
    previousSize = q_func()->size();
    q_func()->resize(sz);
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
        foreach (BAbstractSettingsTab *t, Tabs)
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

BSettingsDialog::BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs) )
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, Navigation navigation, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, navigation) )
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(BAbstractSettingsTab *tab, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, QList<BAbstractSettingsTab *>() << tab) )
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
