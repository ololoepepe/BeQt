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
================================ Plugins Settings Tab Private Object
============================================================================*/

BPluginsSettingsTabPrivateObject::BPluginsSettingsTabPrivateObject(BPluginsSettingsTabPrivate *p) :
    BBasePrivateObject(p)
{
    //
}

BPluginsSettingsTabPrivateObject::~BPluginsSettingsTabPrivateObject()
{
    //
}

void BPluginsSettingsTabPrivateObject::lstwgtCurrentRowChanged(int currentRow)
{
    p_func()->lstwgtCurrentRowChanged(currentRow);
}

void BPluginsSettingsTabPrivateObject::lstwgtItemChanged(QListWidgetItem *item)
{
    p_func()->lstwgtItemChanged(item);
}

void BPluginsSettingsTabPrivateObject::btnSettingsClicked()
{
    p_func()->btnSettingsClicked();
}

void BPluginsSettingsTabPrivateObject::btnAboutClicked()
{
    p_func()->btnAboutClicked();
}

/*============================================================================
================================ Plugins Settings Tab Private
============================================================================*/

BPluginsSettingsTabPrivate::BPluginsSettingsTabPrivate(BPluginsSettingsTab *q) :
    BBasePrivate( *q, *new BPluginsSettingsTabPrivateObject(this) )
{
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
        QObject::connect( lstwgt, SIGNAL( currentRowChanged(int) ), o_func(), SLOT( lstwgtCurrentRowChanged(int) ) );
        QObject::connect( lstwgt, SIGNAL( itemChanged(QListWidgetItem *) ),
                          o_func(), SLOT( lstwgtItemChanged(QListWidgetItem *) ) );
      hlt->addWidget(lstwgt);
      vlt = new QVBoxLayout;
        btnSettings = new QPushButton(q);
          btnSettings->setIcon( BApplication::beqtIcon("configure") );
          btnSettings->setText( tr("Configure plugin", "btn text") );
          QObject::connect( btnSettings, SIGNAL( clicked() ), o_func(), SLOT( btnSettingsClicked() ) );
        vlt->addWidget(btnSettings);
        btnAbout = new QPushButton(q);
          btnAbout->setIcon( BApplication::beqtIcon("help_about") );
          btnAbout->setText( tr("About plugin", "btn text") );
          QObject::connect( btnAbout, SIGNAL( clicked() ), o_func(), SLOT( btnAboutClicked() ) );
        vlt->addWidget(btnAbout);
        vlt->addStretch();
      hlt->addLayout(vlt);
    //
    lstwgtCurrentRowChanged( lstwgt->currentRow() );
}

BPluginsSettingsTabPrivate::~BPluginsSettingsTabPrivate()
{
    //
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
    ad.setAbout(info.description, info.copyright, info.website);
    ad.setChangeLog(info.changelog);
    ad.setAuthorsInfos(info.authors);
    ad.setTranslationInfos(info.translators);
    ad.setThanksToInfos(info.thanksTo);
    ad.setLicense(info.license);
    BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>( pw->instance() );
    QPixmap pm;
    if (gpi)
        pm = gpi->pixmap();
    if ( !pm.isNull() )
        ad.setPixmap(pm);
    ad.exec(); //Maybe use open() instead?
}

//

BPluginsSettingsTabPrivate::BPluginsSettingsTabPrivate(BPluginsSettingsTab &q, BPluginsSettingsTabPrivateObject &o) :
    BBasePrivate(q, o)
{
    //
}

/*============================================================================
================================ Plugins Settings Tab
============================================================================*/

BPluginsSettingsTab::BPluginsSettingsTab() :
    BBase( *new BPluginsSettingsTabPrivate(this) )
{
    //
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
    //
}
