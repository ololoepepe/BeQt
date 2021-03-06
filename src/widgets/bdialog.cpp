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

#include "bdialog.h"
#include "bdialog_p.h"

#include <BeQtCore/BeQt>

#include <QAbstractButton>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QList>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ BDialogPrivate ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BDialogPrivate::BDialogPrivate(BDialog *q) :
    BBaseObjectPrivate(q)
{
    //
}

BDialogPrivate::~BDialogPrivate()
{
    resetWidget();
}

/*============================== Public methods ============================*/

void BDialogPrivate::init()
{
    wgt = 0;
    vlt = new QVBoxLayout(q_func());
      dlgbbox = new QDialogButtonBox;
      vlt->addWidget(dlgbbox);
}

void BDialogPrivate::resetWidget(QWidget *w)
{
    if (wgt) {
        vlt->removeWidget(wgt);
        wgt->deleteLater();
        wgt = 0;
    }
    wgt = w;
    if (wgt) {
        wgt->setParent(q_func());
        vlt->insertWidget(0, wgt);
    }
}

/*============================================================================
================================ BDialog =====================================
============================================================================*/

/*============================== Public constructors =======================*/

BDialog::BDialog(QWidget *parent) :
    QDialog(parent), BBaseObject(*new BDialogPrivate(this))
{
    d_func()->init();
}

BDialog::BDialog(QDialogButtonBox::StandardButtons buttons, QWidget *parent) :
    QDialog(parent), BBaseObject(*new BDialogPrivate(this))
{
    d_func()->init();
    d_func()->dlgbbox->setStandardButtons(buttons);
}

BDialog::~BDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BDialog::BDialog(BDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BDialog::addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const char *method)
{
    addButton(button, role, this, method);
}

void BDialog::addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QObject *object,
                        const char *method)
{
    addButton(button, role, QList<BeQt::Target>() << BeQt::target(object, method));
}

void BDialog::addButton(QAbstractButton *button, QDialogButtonBox::ButtonRole role, const QList<BeQt::Target> &targets)
{
    d_func()->dlgbbox->addButton(button, role);
    if (!button)
        return;
    foreach (const BeQt::Target &t, targets) {
        if (t.first && t.second)
            connect(button, SIGNAL(clicked()), t.first, t.second);
    }
}

QPushButton *BDialog::addButton(const QString &text, QDialogButtonBox::ButtonRole role, const char *method)
{
    return addButton(text, role, this, method);
}

QPushButton *BDialog::addButton(const QString &text, QDialogButtonBox::ButtonRole role, const QObject *object,
                                const char *method)
{
    return addButton(text, role, QList<BeQt::Target>() << BeQt::target(object, method));
}

QPushButton *BDialog::addButton(const QString &text, QDialogButtonBox::ButtonRole role,
                                const QList<BeQt::Target> &targets)
{
    QPushButton *btn = d_func()->dlgbbox->addButton(text, role);
    if (btn) {
        foreach (const BeQt::Target &t, targets) {
            if (t.first && t.second)
                connect(btn, SIGNAL(clicked()), t.first, t.second);
        }
    }
    return btn;
}

QPushButton *BDialog::addButton(QDialogButtonBox::StandardButton button, const char *method)
{
    return addButton(button, this, method);
}

QPushButton *BDialog::addButton(QDialogButtonBox::StandardButton button, const QObject *object, const char *method)
{
    return addButton(button, QList<BeQt::Target>() << BeQt::target(object, method));
}

QPushButton *BDialog::addButton(QDialogButtonBox::StandardButton button, const QList<BeQt::Target> &targets)
{
    QPushButton *btn = d_func()->dlgbbox->addButton(button);
    if (btn) {
        foreach (const BeQt::Target &t, targets) {
            if (t.first && t.second)
                connect(btn, SIGNAL(clicked()), t.first, t.second);
        }
    }
    return btn;
}

QPushButton *BDialog::button(QDialogButtonBox::StandardButton which) const
{
    return d_func()->dlgbbox->button(which);
}

QDialogButtonBox *BDialog::buttonBox() const
{
    return d_func()->dlgbbox;
}

QDialogButtonBox::ButtonRole BDialog::buttonRole(QAbstractButton *button) const
{
    return d_func()->dlgbbox->buttonRole(button);
}

QList<QAbstractButton *> BDialog::buttons() const
{
    return d_func()->dlgbbox->buttons();
}

bool BDialog::centerButtons() const
{
    return d_func()->dlgbbox->centerButtons();
}

void BDialog::removeButton(QAbstractButton *button)
{
    d_func()->dlgbbox->removeButton(button);
    if (button)
        button->disconnect(SIGNAL(clicked()));
}

void BDialog::setCenterButtons(bool center)
{
    d_func()->dlgbbox->setCenterButtons(center);
}

void BDialog::setStandardButtons(QDialogButtonBox::StandardButtons buttons)
{
    d_func()->dlgbbox->setStandardButtons(buttons);
}

void BDialog::setStretchEnabled(bool b)
{
    if (stretchEnabled() == b)
        return;
    d_func()->vlt->insertStretch(d_func()->vlt->count() - 1);
}

QDialogButtonBox::StandardButton BDialog::standardButton(QAbstractButton *button) const
{
    return d_func()->dlgbbox->standardButton(button);
}

QDialogButtonBox::StandardButtons BDialog::standardButtons() const
{
    return d_func()->dlgbbox->standardButtons();
}

void BDialog::setWidget(QWidget *w)
{
    d_func()->resetWidget(w);
}

bool BDialog::stretchEnabled() const
{
    return d_func()->vlt->count() == (d_func()->wgt ? 3 : 2);
}

QWidget *BDialog::widget() const
{
    return d_func()->wgt;
}
