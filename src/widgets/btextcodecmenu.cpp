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

#include "btextcodecmenu.h"
#include "btextcodecmenu_p.h"

#include "bapplication.h"

#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbaseobject_p.h>

#include <QAction>
#include <QList>
#include <QMenu>
#include <QSignalMapper>
#include <QString>
#include <QTextCodec>

/*============================================================================
================================ BTextCodecMenuPrivate =======================
============================================================================*/

/*============================== Public constructors =======================*/

BTextCodecMenuPrivate::BTextCodecMenuPrivate(BTextCodecMenu *q, BTextCodecMenu::Style s) :
    BBaseObjectPrivate(q), Style(s)
{
    //
}

BTextCodecMenuPrivate::~BTextCodecMenuPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BTextCodecMenuPrivate::init()
{
    mapper = new QSignalMapper(this);
    q_func()->setMapping(this, SLOT(codecSelected(QString)));
    if (BTextCodecMenu::StructuredStyle == Style) {
        foreach (BeQt::CodecsGroup gr, BeQt::codecsGroups()) {
            QMenu *submnu = q_func()->addMenu("");
            submnu->setProperty("beqt/codec_group", gr);
            foreach (const QString &cn, BeQt::codecNamesForGroup(gr)) {
                QAction *act = submnu->addAction("");
                act->setProperty("beqt/codec_name", cn);
                bSetMapping(mapper, act, SIGNAL(triggered()), cn);
            }
        }
    } else {
        foreach (const QString &cn, BeQt::supportedCodecsNames()) {
            QAction *act = q_func()->addAction("");
            act->setProperty("beqt/codec_name", cn);
            bSetMapping(mapper, act, SIGNAL(triggered()), cn);
        }
    }
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

/*============================== Public slots ==============================*/

void BTextCodecMenuPrivate::codecSelected(const QString &cn)
{
    codecName = cn;
}

void BTextCodecMenuPrivate::retranslateUi()
{
    QList<QAction *> actions;
    foreach (QAction *act, q_func()->actions()) {
        QMenu *submnu = act->menu();
        if (!submnu) {
            actions << act;
            continue;
        }
        BeQt::CodecsGroup gr = BeQt::codecsGroupFromInt(submnu->property("beqt/codec_group").toInt());
        if (BeQt::InvalidGroup == gr)
            continue;
        submnu->setTitle(BeQt::codecsGroupName(gr));
        actions << submnu->actions();
    }
    foreach (QAction *act, actions) {
        QString cn = act->property("beqt/codec_name").toString();
        if (cn.isEmpty() || !BeQt::supportedCodecsNames().contains(cn))
            continue;
        act->setText(BeQt::fullCodecName(cn));
    }
}

/*============================================================================
================================ BTextCodecMenu ==============================
============================================================================*/

/*============================== Public constructors =======================*/

BTextCodecMenu::BTextCodecMenu(QWidget *parent) :
    QMenu(parent), BBaseObject(*new BTextCodecMenuPrivate(this, StructuredStyle))
{
    d_func()->init();
}

BTextCodecMenu::BTextCodecMenu(Style s, QWidget *parent) :
    QMenu(parent), BBaseObject(*new BTextCodecMenuPrivate(this, s))
{
    d_func()->init();
}

BTextCodecMenu::~BTextCodecMenu()
{
    //
}

/*============================== Protected constructors ====================*/

BTextCodecMenu::BTextCodecMenu(BTextCodecMenuPrivate &d, QWidget *parent) :
    QMenu(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QTextCodec *BTextCodecMenu::selectedCodec() const
{
    return BeQt::codec(d_func()->codecName);
}

QString BTextCodecMenu::selectedCodecName() const
{
    return d_func()->codecName;
}

void BTextCodecMenu::setMapping(const QObject *receiver, const char *method)
{
    if (!receiver || !method)
        return;
    connect(d_func()->mapper, SIGNAL(mapped(QString)), receiver, method);
}

BTextCodecMenu::Style BTextCodecMenu::style() const
{
    return d_func()->Style;
}
