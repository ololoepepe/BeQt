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

#include "bsettingsdialog.h"
#include "bsettingsdialog_p.h"

#include "babstractsettingstab.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QByteArray>
#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMap>
#include <QMessageBox>
#include <QMetaObject>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QTimer>
#include <QVariant>
#include <QVariantMap>
#include <QVBoxLayout>

/*============================================================================
================================ BSettingsDialogPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsDialogPrivate::BSettingsDialogPrivate(BSettingsDialog *q, const QList<BAbstractSettingsTab *> &tabs,
                                               BSettingsDialog::TabNavigation navigation) :
    BBaseObjectPrivate(q), Navigation(navigation), Tabs(tabs)
{
    //
}

BSettingsDialogPrivate::~BSettingsDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

BAbstractSettingsTab *BSettingsDialogPrivate::currentTab() const
{
    if (stkdwgt)
        return Tabs.at(stkdwgt->currentIndex());
    else if (twgt)
        return Tabs.at(twgt->currentIndex());
    else if (!Tabs.isEmpty())
        return Tabs.first();
    else
        return 0;
}

void BSettingsDialogPrivate::init()
{
    valid = !Tabs.isEmpty();
    B_Q(BSettingsDialog);
    q->setWindowTitle(tr("Settings", "windowTitle"));
    QVBoxLayout *vlt = new QVBoxLayout(q);
      QHBoxLayout *hlt = new QHBoxLayout;
        btnRestoreDefault = new QPushButton(q);
          btnRestoreDefault->setText( tr("Restore default settings", "btn text") );
          btnRestoreDefault->setEnabled(false);
          connect( btnRestoreDefault, SIGNAL(clicked()), this, SLOT(btnRestoreDefaultClicked()));
        hlt->addWidget(btnRestoreDefault);
        cboxAdvancedMode = new QCheckBox(q);
          cboxAdvancedMode->setText( tr("Show additional settings", "cbox text") );
          connect(cboxAdvancedMode, SIGNAL(stateChanged(int)), this, SLOT(cboxAdvancedModeStateChanged(int)));
          cboxAdvancedMode->setEnabled(false);
        hlt->addWidget(cboxAdvancedMode);
        hlt->addStretch();
      vlt->addLayout(hlt);
    if (Tabs.size() > 1) {
        if (BSettingsDialog::ListNavigation == Navigation) {
            hspltr = new QSplitter(Qt::Horizontal, q);
            lstwgt = new QListWidget(q);
            hspltr->addWidget(lstwgt);
            stkdwgt = new QStackedWidget(q);
              connect(lstwgt, SIGNAL(currentRowChanged(int)), stkdwgt, SLOT(setCurrentIndex(int)));
              connect(stkdwgt, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
            hspltr->addWidget(stkdwgt);
            vlt->addWidget(hspltr);
            twgt = 0;
            foreach (BAbstractSettingsTab *tab, Tabs) {
                stkdwgt->addWidget(tab);
                QListWidgetItem *lwi = new QListWidgetItem;
                lwi->setText(tab->title());
                lwi->setIcon(tab->icon());
                lstwgt->addItem(lwi);
            }
            lstwgt->setCurrentRow(0);
        } else {
            hspltr = 0;
            lstwgt = 0;
            stkdwgt = 0;
            twgt = new QTabWidget(q);
            connect(twgt, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
            vlt->addWidget(twgt);
            foreach (BAbstractSettingsTab *tab, Tabs)
                twgt->addTab(tab, tab->icon(), tab->title());
        }
    } else if (!Tabs.isEmpty()) {
        BAbstractSettingsTab *tab = Tabs.first();
        vlt->addWidget(tab);
        hspltr = 0;
        lstwgt = 0;
        stkdwgt = 0;
        twgt = 0;
        q->setWindowTitle(q->windowTitle() + ": " + tab->title());
        currentChanged();
    } else {
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
      connect(dlgbbox, SIGNAL(accepted()), this, SLOT(accepted()));
      connect(dlgbbox, SIGNAL(rejected()), q, SLOT(reject()));
    vlt->addWidget(dlgbbox);
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

/*============================== Public slots ==============================*/

void BSettingsDialogPrivate::accepted()
{
    foreach (BAbstractSettingsTab *t, Tabs) {
        if (!t->saveSettings())
            return;
    }
    q_func()->accept();
}

void BSettingsDialogPrivate::btnRestoreDefaultClicked()
{
    BAbstractSettingsTab *tab = currentTab();
    if (!tab)
        return;
    QMessageBox msg(q_func());
    msg.setWindowTitle(tr("Restoring default settings", "msgbox windowTitle"));
    msg.setIcon(QMessageBox::Question);
    QString text = tab->restoreDefaultHint();
    if (text.isEmpty())
        text = tr("You are about to restore settings to their default state. Do you want to continue?", "msgbox text");
    msg.setText(text);
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    msg.setDefaultButton(QMessageBox::Yes);
    if (msg.exec() == QMessageBox::Yes)
        tab->restoreDefault();
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

void BSettingsDialogPrivate::cboxAdvancedModeStateChanged(int state)
{
    BAbstractSettingsTab *tab = currentTab();
    if (!tab)
        return;
    bool b = (Qt::Checked == state);
    if (tab->isInAdvancedMode() != b) {
        tab->setAdvancedMode(b);
        QTimer::singleShot(10, this, SLOT(updateSize()));
    }
    QWidget *wgt = cboxAdvancedMode->nextInFocusChain();
    if (wgt)
        wgt->setFocus();
}

void BSettingsDialogPrivate::currentChanged(int)
{
    BAbstractSettingsTab *tab = currentTab();
    if (!tab)
        return;
    btnRestoreDefault->setEnabled(tab->hasDefault());
    cboxAdvancedMode->setEnabled(tab->hasAdvancedMode());
    cboxAdvancedMode->setChecked(tab->isInAdvancedMode());
}

void BSettingsDialogPrivate::updateSize()
{
    q_func()->resize(q_func()->sizeHint());
}

/*============================================================================
================================ BSettingsDialog =============================
============================================================================*/

/*============================== Public constructors =======================*/

BSettingsDialog::BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSettingsDialogPrivate(this, tabs))
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(const QList<BAbstractSettingsTab *> &tabs, TabNavigation navigation,
                                 QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSettingsDialogPrivate(this, tabs, navigation))
{
    d_func()->init();
}

BSettingsDialog::BSettingsDialog(BAbstractSettingsTab *tab, QWidget *parent) :
    QDialog(parent), BBaseObject(*new BSettingsDialogPrivate(this, QList<BAbstractSettingsTab *>() << tab))
{
    d_func()->init();
}

BSettingsDialog::~BSettingsDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BSettingsDialog::BSettingsDialog(BSettingsDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

bool BSettingsDialog::isValid() const
{
    return d_func()->valid;
}

void BSettingsDialog::restoreState(const QByteArray &state)
{
    if (!isValid())
        return;
    B_D(BSettingsDialog);
    if (!d->lstwgt && !d->twgt)
        return;
    QVariantMap m = BeQt::deserialize(state).toMap();
    QByteArray cn = m.value("current_tab_class_name").toByteArray();
    if (cn.isEmpty())
        return;
    if (d->lstwgt) {
        foreach (int i, bRangeD(0, d->stkdwgt->count() - 1)) {
            if (QByteArray(d->stkdwgt->widget(i)->metaObject()->className()) == cn) {
                d->lstwgt->setCurrentRow(i);
                break;
            }
        }
    } else if (d->twgt) {
        foreach (int i, bRangeD(0, d->twgt->count() - 1)) {
            if (QByteArray(d->twgt->widget(i)->metaObject()->className()) == cn) {
                d->twgt->setCurrentIndex(i);
                break;
            }
        }
    }
}

QByteArray BSettingsDialog::saveState() const
{
    QVariantMap m;
    BAbstractSettingsTab *tab = d_func()->currentTab();
    if (tab)
        m.insert("current_tab_class_name", QByteArray(tab->metaObject()->className()));
    return BeQt::serialize(m);
}
