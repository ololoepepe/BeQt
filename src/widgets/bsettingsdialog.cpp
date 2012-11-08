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
#include <QListWidget>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QSplitter>

class B_WIDGETS_EXPORT BSettingsDialogPrivate : public BBasePrivate
{
    Q_DECLARE_TR_FUNCTIONS(BSettingsDialog)
    B_DECLARE_PUBLIC(BSettingsDialog)
public:
    BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs,
                           BSettingsDialog::Navigation navigation);
    ~BSettingsDialogPrivate();
    //
    const BSettingsDialog::SettingsTabMap TabMap;
    //
    QVBoxLayout *vlt;
      QSplitter *hspltr;
        QListWidget *lstwgt;
        QStackedWidget *stkdwgt;
      QTabWidget *twgt;
      QHBoxLayout *hltActions;
        //stretch
        QPushButton *btnCancel;
        QPushButton *btnOk;
private:
    Q_DISABLE_COPY(BSettingsDialogPrivate)
};

//

BSettingsDialogPrivate::BSettingsDialogPrivate(BSettingsDialog *q, const BSettingsDialog::SettingsTabMap &tabs,
                                               BSettingsDialog::Navigation navigation) :
    BBasePrivate(q), TabMap(tabs)
{
    q->setWindowTitle( tr("Settings", "windowTitle") );
    q->setMinimumHeight(120);
    q->setMinimumWidth(240);
    vlt = new QVBoxLayout(q);
    if (TabMap.size() > 1)
    {
        if (BSettingsDialog::ListNavigation == navigation)
        {
            hspltr = new QSplitter(Qt::Horizontal, q);
            lstwgt = new QListWidget(q);
            hspltr->addWidget(lstwgt);
            stkdwgt = new QStackedWidget(q);
              QObject::connect( lstwgt, SIGNAL( currentRowChanged(int) ), stkdwgt, SLOT( setCurrentIndex(int) ) );
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
        hspltr = 0;
        lstwgt = 0;
        stkdwgt = 0;
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
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, ListNavigation) )
{
    //
}

BSettingsDialog::BSettingsDialog(const SettingsTabMap &tabs, Navigation navigation, QWidget *parent) :
    QDialog(parent), BBase( *new BSettingsDialogPrivate(this, tabs, navigation) )
{
    //
}

BSettingsDialog::~BSettingsDialog()
{
    //
}

//

bool BSettingsDialog::isValid() const
{
    return d_func()->hltActions;
}

BSettingsDialog::SettingsMap BSettingsDialog::settingsMap() const
{
    SettingsMap m;
    const BSettingsDialogPrivate *const d = d_func();
    foreach ( const QString &key, d->TabMap.keys() )
        m.insert( key, d->TabMap.value(key)->valueMap() );
    return m;
}

//

BSettingsDialog::BSettingsDialog(BSettingsDialogPrivate &d) :
    BBase(d)
{
    //
}
