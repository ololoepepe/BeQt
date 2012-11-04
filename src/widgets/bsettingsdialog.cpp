#include "bsettingsdialog.h"
#include "babstractsettingstab.h"

#include <BeQtCore/private/bbase_p.h>

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
#include <QCoreApplication>

class BSettingsDialogPrivate : public BBasePrivate
{
    Q_DECLARE_TR_FUNCTIONS(BSettingsDialog)
    B_DECLARE_PUBLIC(BSettingsDialog)
public:
    BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs);
    ~BSettingsDialogPrivate();
    //
    BSettingsDialog::SettingsTabMap tabMap;
    QLabel *lbl;
    QVBoxLayout *vlt;
      QTabWidget *twgt;
      QHBoxLayout *hltActions;
        //stretch
        QPushButton *btnCancel;
        QPushButton *btnOk;
private:
    Q_DISABLE_COPY(BSettingsDialogPrivate)
};

//

BSettingsDialogPrivate::BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs) :
  BBasePrivate(q)
{
    tabMap = tabs;
    q->setWindowTitle( tr("Settings", "windowTitle") );
    q->setMinimumHeight(120);
    q->setMinimumWidth(240);
    vlt = new QVBoxLayout(q);
    if (tabMap.size() > 1)
    {
        lbl = 0;
        twgt = new QTabWidget(q);
        foreach (BAbstractSettingsTab *tab, tabMap)
            twgt->addTab( tab, tab->title() );
        vlt->addWidget(twgt);
    }
    else if ( !tabMap.isEmpty() )
    {
        BAbstractSettingsTab *tab = tabMap.value( tabMap.keys().first() );
        q->setWindowTitle( q->windowTitle() + ": " + tab->title() );
        twgt = 0;
        lbl = new QLabel(q);
          lbl->setText( tab->title() );
        vlt->addWidget(lbl);
        vlt->addWidget(tab);
    }
    else
    {
        lbl = 0;
        twgt = 0;
        hltActions = 0;
        btnCancel = 0;
        btnOk = 0;
        q->setWindowTitle(q->windowTitle() + " [" + tr("invalid", "windowTitle") + "]");
        return;
    }
    hltActions = new QHBoxLayout;
      hltActions->addStretch();
      btnCancel = new QPushButton(q);
        btnCancel->setText( tr("Cancel", "btn text") );
        QObject::connect( btnCancel, SIGNAL( clicked() ), q, SLOT( reject() ) );
      hltActions->addWidget(btnCancel);
      btnOk = new QPushButton(q);
        btnOk->setDefault(true);
        btnOk->setText( tr("Ok", "btn text") );
        QObject::connect( btnOk, SIGNAL( clicked() ), q, SLOT( accept() ) );
      hltActions->addWidget(btnOk);
    vlt->addLayout(hltActions);
}

BSettingsDialogPrivate::~BSettingsDialogPrivate()
{
    //
}

//

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs) )
{
    //
}

BSettingsDialog::~BSettingsDialog()
{
    //
}

//

BSettingsDialog::SettingsMap BSettingsDialog::settingsMap() const
{
    SettingsMap m;
    const BSettingsDialogPrivate *const d = d_func();
    foreach ( const QString &key, d->tabMap.keys() )
        m.insert( key, d->tabMap.value(key)->valueMap() );
    return m;
}

//

BSettingsDialog::BSettingsDialog(BSettingsDialogPrivate &d) :
    BBase(d)
{
    //
}
