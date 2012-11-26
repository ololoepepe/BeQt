#include "bpluginssettingstab.h"
#include "bpluginssettingstab_p.h"
#include "bapplication.h"
#include "bguiplugininterface.h"
#include "baboutdialog.h"
#include "bsettingsdialog.h"
#include "babstractsettingstab.h"

#include <BeQtCore/private/bbase_p.h>
#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BPluginWrapper>

#include <QString>
#include <QVariantMap>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QList>
#include <QObject>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QIcon>

#include <QDebug>

/*============================================================================
================================ Plugins Settings Tab Private
============================================================================*/

BPluginsSettingsTabPrivate::BPluginsSettingsTabPrivate(BPluginsSettingsTab *q) :
    BBasePrivate(q)
{
    //
}

BPluginsSettingsTabPrivate::~BPluginsSettingsTabPrivate()
{
    //
}

//

void BPluginsSettingsTabPrivate::init()
{
    B_Q(BPluginsSettingsTab);
    plugins = BApplication::pluginWrappers();
    //
    hlt = new QHBoxLayout(q);
      lstwgt = new QListWidget(q);
        foreach (BPluginWrapper *pw, plugins)
        {
            QListWidgetItem *lwi = new QListWidgetItem;
            lwi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            lwi->setCheckState(pw->isActivated() ? Qt::Checked : Qt::Unchecked);
            lwi->setText( pw->name() );
            BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>( pw->instance() );
            if (gpi)
                lwi->setIcon( QIcon( gpi->pixmap() ) );
            lstwgt->addItem(lwi);
        }
        connect( lstwgt, SIGNAL( currentRowChanged(int) ), this, SLOT( lstwgtCurrentRowChanged(int) ) );
        connect( lstwgt, SIGNAL( itemChanged(QListWidgetItem *) ),
                 this, SLOT( lstwgtItemChanged(QListWidgetItem *) ) );
      hlt->addWidget(lstwgt);
      vlt = new QVBoxLayout;
        btnSettings = new QPushButton(q);
          btnSettings->setIcon( BApplication::beqtIcon("configure") );
          btnSettings->setText( tr("Configure plugin", "btn text") );
          connect( btnSettings, SIGNAL( clicked() ), this, SLOT( btnSettingsClicked() ) );
        vlt->addWidget(btnSettings);
        btnAbout = new QPushButton(q);
          btnAbout->setIcon( BApplication::beqtIcon("help_about") );
          btnAbout->setText( tr("About plugin", "btn text") );
          connect( btnAbout, SIGNAL( clicked() ), this, SLOT( btnAboutClicked() ) );
        vlt->addWidget(btnAbout);
        vlt->addStretch();
      hlt->addLayout(vlt);
    //
    lstwgtCurrentRowChanged( lstwgt->currentRow() );
}

//

void BPluginsSettingsTabPrivate::lstwgtCurrentRowChanged(int currentRow)
{
    bool b = (currentRow >= 0);
    btnSettings->setEnabled( b && plugins.at(currentRow)->isActivated() );
    btnAbout->setEnabled(b);
}

void BPluginsSettingsTabPrivate::lstwgtItemChanged(QListWidgetItem *item)
{
    bool b = (item->checkState() == Qt::Checked);
    plugins.at( lstwgt->row(item) )->setActivated(b);
    btnSettings->setEnabled(b);
}

void BPluginsSettingsTabPrivate::btnSettingsClicked()
{
    BPluginWrapper *pw = plugins.at( lstwgt->currentRow() );
    BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>( pw->instance() );
    BAbstractSettingsTab *ast = gpi ? gpi->settingsTab() : 0;
    if (ast)
    {
        BSettingsDialog::SettingsTabMap tabs;
        tabs.insert("plugin", ast);
        BSettingsDialog sd( tabs, q_func() );
        if (sd.exec() != BSettingsDialog::Accepted)
            return;
        gpi->handleSettings( sd.settingsMap().value("plugin") );
    }
    else
    {
        QString title = tr("No settings", "msgbox title");
        QString text = tr("This plugin does not have any settings", "msgbox text");
        QMessageBox::information(q_func(), title, text, QMessageBox::Ok, QMessageBox::Ok);
    }
}

void BPluginsSettingsTabPrivate::btnAboutClicked()
{
    BPluginWrapper *pw = plugins.at( lstwgt->currentRow() );
    BAboutDialog ad( q_func() );
    BPluginInterface::PluginInfo info = pw->info();
    ad.setOrganization(info.organization, info.copyrightYears);
    ad.setWebsite(info.website);
    ad.setDescription(info.description);
    ad.setChangeLog(info.changelog);
    ad.setAuthors(info.authors);
    ad.setTranslators(info.translators);
    ad.setThanksTo(info.thanksTo);
    ad.setLicense(info.license);
    BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>( pw->instance() );
    QPixmap pm;
    if (gpi)
        pm = gpi->pixmap();
    if ( !pm.isNull() )
        ad.setPixmap(pm);
    ad.exec(); //Maybe use open() instead?
}

/*============================================================================
================================ Plugins Settings Tab
============================================================================*/

BPluginsSettingsTab::BPluginsSettingsTab() :
    BBase( *new BPluginsSettingsTabPrivate(this) )
{
    d_func()->init();
}

BPluginsSettingsTab::~BPluginsSettingsTab()
{
    //
}

//

QString BPluginsSettingsTab::title() const
{
    return tr("Plugins", "title");
}

QIcon BPluginsSettingsTab::icon() const
{
    return BApplication::beqtIcon("binary");
}

QVariantMap BPluginsSettingsTab::valueMap() const
{
    return QVariantMap();
}

//

BPluginsSettingsTab::BPluginsSettingsTab(BPluginsSettingsTabPrivate &d) :
    BBase(d)
{
    d_func()->init();
}
