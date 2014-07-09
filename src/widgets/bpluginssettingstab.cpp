/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bpluginssettingstab.h"
#include "bpluginssettingstab_p.h"

#include "baboutdialog.h"
#include "babstractsettingstab.h"
#include "bapplication.h"
#include "bguiplugininterface.h"
#include "bhelpbrowser.h"
#include "bsettingsdialog.h"

#include <BeQtCore/BPluginInterface>
#include <BeQtCore/BPluginWrapper>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

/*============================================================================
================================ BPluginsSettingsTabPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BPluginsSettingsTabPrivate::BPluginsSettingsTabPrivate(BPluginsSettingsTab *q) :
    BBaseObjectPrivate(q)
{
    //
}

BPluginsSettingsTabPrivate::~BPluginsSettingsTabPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BPluginsSettingsTabPrivate::init()
{
    B_Q(BPluginsSettingsTab);
    plugins = BApplication::pluginWrappers();
    //
    QHBoxLayout *hlt = new QHBoxLayout(q);
      lstwgt = new QListWidget(q);
        foreach (BPluginWrapper *pw, plugins) {
            QListWidgetItem *lwi = new QListWidgetItem;
            lwi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
            lwi->setCheckState(pw->isActivated() ? Qt::Checked : Qt::Unchecked);
            lwi->setText( pw->name() );
            BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>(pw->instance());
            if (gpi)
                lwi->setIcon(QIcon(gpi->pixmap()));
            lstwgt->addItem(lwi);
        }
        connect(lstwgt, SIGNAL(currentRowChanged(int)), this, SLOT(lstwgtCurrentRowChanged(int)));
        connect(lstwgt, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(lstwgtItemChanged(QListWidgetItem *)));
      hlt->addWidget(lstwgt);
      QVBoxLayout *vlt = new QVBoxLayout;
        btnSettings = new QPushButton(q);
          btnSettings->setIcon(BApplication::icon("configure"));
          btnSettings->setText(tr("Configure plugin", "btn text"));
          connect(btnSettings, SIGNAL(clicked()), this, SLOT(btnSettingsClicked()));
        vlt->addWidget(btnSettings);
        btnAbout = new QPushButton(q);
          btnAbout->setIcon( BApplication::icon("help_about") );
          btnAbout->setText( tr("About plugin", "btn text") );
          connect(btnAbout, SIGNAL(clicked()), this, SLOT(btnAboutClicked()));
        vlt->addWidget(btnAbout);
        btnHelp = new QPushButton(q);
          btnHelp->setIcon( BApplication::icon("help_contents") );
          btnHelp->setText( tr("Plugin Help", "btn text") );
          connect(btnHelp, SIGNAL(clicked()), this, SLOT(btnHelpClicked()));
        vlt->addWidget(btnAbout);
        vlt->addStretch();
      hlt->addLayout(vlt);
    //
    lstwgtCurrentRowChanged(lstwgt->currentRow());
}

/*============================== Public slots ==============================*/

void BPluginsSettingsTabPrivate::btnAboutClicked()
{
    BPluginWrapper *pw = plugins.at(lstwgt->currentRow());
    if (!pw)
        return;
    BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>(pw->instance());
    if (gpi) {
        BAboutDialog *ad = gpi->createAboutDialog();
        if (ad)
            return bRet(ad->exec());
    }
    BAboutDialog ad(q_func(), pw->name(), pw->version());
    ad.setWindowTitle(pw->name());
    if (pw->prefereStaticInfo()) {
        BPluginInterface::StaticPluginInfo sinf = pw->staticInfo();
        ad.setOrganization(sinf.organization, sinf.copyrightYears);
        ad.setWebsite(sinf.website);
        ad.setDescription(sinf.description);
        ad.setChangeLog(sinf.changeLog);
        ad.setLicense(sinf.license);
        ad.setAuthors(sinf.authors);
        ad.setTranslators(sinf.translators);
        ad.setThanksTo(sinf.thanksTo);
    } else {
        BPluginInterface::PluginInfo inf = pw->info();
        ad.setOrganization(inf.organization, inf.copyrightYears);
        ad.setWebsite(inf.website);
        ad.setDescriptionFile(inf.descriptionFileName);
        ad.setChangeLogFile(inf.changeLogFileName);
        ad.setLicenseFile(inf.licenseFileName);
        ad.setAuthorsFile(inf.authorsFileName);
        ad.setTranslatorsFile(inf.translatorsFileName);
        ad.setThanksToFile(inf.thanksToFileName);
    }
    if (gpi)
        gpi->processStandardAboutDialog(&ad);
    QPixmap pm;
    if (gpi)
        pm = gpi->pixmap();
    if (!pm.isNull())
        ad.setPixmap(pm);
    ad.exec();
}

void BPluginsSettingsTabPrivate::btnHelpClicked()
{
    BPluginWrapper *pw = plugins.at(lstwgt->currentRow());
    if (!pw)
        return;
    BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>(pw->instance());
    if (!gpi)
        return;
    QString index = gpi->helpIndex();
    BHelpBrowser *hb = new BHelpBrowser(gpi->helpSearchPaths(), index, index);
    hb->setAttribute(Qt::WA_DeleteOnClose, true);
    hb->setGeometry(BApplication::helpBrowserDefaultGeometry());
    hb->show();
}

void BPluginsSettingsTabPrivate::btnSettingsClicked()
{
    BPluginWrapper *pw = plugins.at(lstwgt->currentRow());
    if (!pw)
        return;
    BGuiPluginInterface *gpi = qobject_cast<BGuiPluginInterface *>(pw->instance());
    BAbstractSettingsTab *ast = gpi ? gpi->createSettingsTab() : 0;
    if (ast) {
        QList<BAbstractSettingsTab *> tabs;
        tabs << ast;
        BSettingsDialog sd( tabs, q_func() );
        sd.exec();
    } else {
        QString title = tr("No settings", "msgbox title");
        QString text = tr("This plugin does not have any settings", "msgbox text");
        QMessageBox::information(q_func(), title, text, QMessageBox::Ok, QMessageBox::Ok);
    }
}

void BPluginsSettingsTabPrivate::lstwgtCurrentRowChanged(int currentRow)
{
    bool b = (currentRow >= 0);
    btnSettings->setEnabled(b && plugins.at(currentRow)->isActivated());
    btnAbout->setEnabled(b);
    BGuiPluginInterface *gpi = b ? qobject_cast<BGuiPluginInterface *>(plugins.at(currentRow)->instance()) : 0;
    btnHelp->setEnabled(gpi && !gpi->helpSearchPaths().isEmpty() && !gpi->helpIndex().isEmpty());
}

void BPluginsSettingsTabPrivate::lstwgtItemChanged(QListWidgetItem *item)
{
    bool b = (item->checkState() == Qt::Checked);
    plugins.at(lstwgt->row(item))->setActivated(b);
    btnSettings->setEnabled(b);
}

/*============================================================================
================================ BPluginsSettingsTab =========================
============================================================================*/

/*============================== Public constructors =======================*/

BPluginsSettingsTab::BPluginsSettingsTab() :
    BBaseObject(*new BPluginsSettingsTabPrivate(this))
{
    d_func()->init();
}

BPluginsSettingsTab::~BPluginsSettingsTab()
{
    //
}

/*============================== Protected constructors ====================*/

BPluginsSettingsTab::BPluginsSettingsTab(BPluginsSettingsTabPrivate &d) :
    BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QIcon BPluginsSettingsTab::icon() const
{
    return BApplication::icon("binary");
}

QString BPluginsSettingsTab::title() const
{
    return tr("Plugins", "title");
}
