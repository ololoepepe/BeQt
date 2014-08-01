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

class QObject;

#include "bguitools.h"

#include "bapplication_p.h"

#include <BeQtCore/BeQtGlobal>

#include <QAction>
#include <QColor>
#include <QComboBox>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPalette>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QToolBar>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace BGuiTools
{

void addRow(QVBoxLayout *vlt, const QString &label, QWidget *field)
{
    if (!vlt || !field)
        return;
    QHBoxLayout *hlt = new QHBoxLayout;
    hlt->addWidget(new QLabel(label));
    hlt->addWidget(field);
    vlt->addLayout(hlt);
}

void addRow(QVBoxLayout *vlt, const QString &label, QLayout *field)
{
    if (!vlt || !field)
        return;
    QHBoxLayout *hlt = new QHBoxLayout;
    hlt->addWidget(new QLabel(label));
    hlt->addLayout(field);
    vlt->addLayout(hlt);
}

QFrame *createFrame(QFrame::Shape shape, QWidget *parent)
{
    return createFrame(shape, QFrame::Plain, parent);
}

QFrame *createFrame(QFrame::Shape shape, QFrame::Shadow shadow, QWidget *parent)
{
    QFrame *fr = new QFrame(parent);
    fr->setFrameShape(shape);
    fr->setFrameShadow(shadow);
    return fr;
}

QFont createMonospaceFont(int pointSize)
{
#if defined Q_OS_WIN
    //On Windows some weird fonts are selected, so we set the font explicitly
    QString f = "Courier New";
#else
    QString f = "monospace";
#endif
    //Using such a construct to guarantee that the font will be monospaced
    QFont fnt(QFontInfo(QFont(f)).family());
    fnt.setPointSize((pointSize > 0) ? pointSize : QApplication::font().pointSize());
    return fnt;
}

QAction *createSeparator(QObject *parent)
{
    QAction *act = new QAction(parent);
    act->setSeparator(true);
    return act;
}

QAction *createStandardAction(StandardAction type, QObject *parent)
{
    return BApplicationPrivate::createStandardAction(type, parent);
}

void removeRow(QVBoxLayout *vlt, QWidget *field)
{
    if (!vlt || !field)
        return;
    QWidget *label = labelForField<QWidget>(field);
    field->deleteLater();
    if (label)
        label->deleteLater();
}

void removeRow(QVBoxLayout *vlt, QLayout *field)
{
    if (!vlt || !field)
        return;
    QWidget *label = labelForField<QWidget>(field);
    field->deleteLater();
    if (label)
        label->deleteLater();
}

void setItemEnabled(QComboBox *cmbox, int index, bool enabled)
{
    if (!cmbox || index < 0 || index >= cmbox->count())
        return;
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(cmbox->model());
    if (!model)
        return;
    QStandardItem *item = model->item(index);
    if (!item)
        return;
    Qt::ItemFlags flags = item->flags();
    QVariant color;
    if (enabled) {
        flags |= (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    } else {
        flags &= ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        color = cmbox->palette().color(QPalette::Disabled, QPalette::Text);
    }
    item->setFlags(flags);
    item->setData(color, Qt::TextColorRole);
}

void setRowEnabled(QWidget *field, bool enabled)
{
    QWidget *w = labelForField<QWidget>(field);
    if (!field || !w)
        return;
    w->setEnabled(enabled);
    field->setEnabled(enabled);
}

void setRowEnabled(QLayout *field, bool enabled)
{
    QWidget *w = labelForField<QWidget>(field);
    if (!field || !w)
        return;
    w->setEnabled(enabled);
    QStack<QLayout *> s;
    s.push(field);
    while (!s.isEmpty()) {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        foreach (int i, bRangeD(0, lt->count() - 1)) {
            s.push(lt->itemAt(i)->layout());
            QWidget *w = lt->itemAt(i)->widget();
            if (w)
                w->setEnabled(enabled);
        }
    }
}

void setRowVisible(QWidget *field, bool visible)
{
    QWidget *w = labelForField<QWidget>(field);
    if (!field || !w)
        return;
    w->setVisible(visible);
    field->setVisible(visible);
}

void setRowVisible(QLayout *field, bool visible)
{
    QWidget *w = labelForField<QWidget>(field);
    if (!field || !w)
        return;
    w->setVisible(visible);
    QStack<QLayout *> s;
    s.push(field);
    while (!s.isEmpty()) {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        foreach (int i, bRangeD(0, lt->count() - 1)) {
            s.push(lt->itemAt(i)->layout());
            QWidget *w = lt->itemAt(i)->widget();
            if (w)
                w->setVisible(visible);
        }
    }
}

QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action)
{
    if (!toolBar || !action)
        return 0;
    return static_cast<QToolButton *>(toolBar->widgetForAction(action));
}

}
