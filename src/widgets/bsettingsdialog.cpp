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

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsDialogPrivate::BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs,
                                               BSettingsDialog::Navigation navigation) :
    BBasePrivate(q), TabMap(tabs), Navigation(navigation)
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
    B_Q(BSettingsDialog);
    q->setWindowTitle( tr("Settings", "windowTitle") );
    vlt = new QVBoxLayout(q);
    if ( !TabMap.isEmpty() )
    {
        cboxAdvancedMode = new QCheckBox(q);
        cboxAdvancedMode->setText( tr("Show additional settings", "cbox text") );
        connect( cboxAdvancedMode, SIGNAL( stateChanged(int) ), this, SLOT( cboxAdvancedModeStateChanged(int) ) );
        vlt->addWidget(cboxAdvancedMode);
        cboxAdvancedMode->setVisible(false);
        foreach (BAbstractSettingsTab *t, TabMap)
        {
            if ( t->hasAdvancedMode() )
            {
                cboxAdvancedMode->setVisible(true);
                break;
            }
        }
    }
    if (TabMap.size() > 1)
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
            foreach (BAbstractSettingsTab *tab, TabMap)
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
            foreach (BAbstractSettingsTab *tab, TabMap)
                twgt->addTab( tab, tab->icon(), tab->title() );
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
        cboxAdvancedMode = 0;
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
}

/*============================================================================
================================ BSettingsDialog =============================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, ListNavigation) )
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, Navigation navigation, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, navigation) )
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
    return d_func()->dlgbbox;
}

BSettingsDialog::SettingsMap BSettingsDialog::settingsMap() const
{
    SettingsMap m;
    const BSettingsDialogPrivate *const d = d_func();
    foreach ( const QString &key, d->TabMap.keys() )
        m.insert( key, d->TabMap.value(key)->valueMap() );
    return m;
}
