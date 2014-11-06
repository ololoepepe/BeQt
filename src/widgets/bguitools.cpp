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
#include <QApplication>
#include <QChildEvent>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QEvent>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QPalette>
#include <QRect>
#include <QSize>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QToolBar>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ BDockWidgetTabifyHelper =====================
============================================================================*/

class BDockWidgetTabifyHelper : public QObject
{
public:
    explicit BDockWidgetTabifyHelper(QObject *parent = 0);
public:
    bool eventFilter(QObject *o, QEvent *e);
};

/*============================================================================
================================ BDockWidgetTabifyHelper =====================
============================================================================*/

/*============================== Public constructors =======================*/

BDockWidgetTabifyHelper::BDockWidgetTabifyHelper(QObject *parent) :
    QObject(parent)
{
    //
}

/*============================== Public methods ============================*/

bool BDockWidgetTabifyHelper::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::ChildPolished)
        return false;
    QChildEvent *ce = static_cast<QChildEvent *>(e);
    QDockWidget *dwgt = qobject_cast<QDockWidget *>(ce->child());
    if (!dwgt)
        return false;
    BGuiTools::tabifyDockWidgets(qobject_cast<QMainWindow *>(o));
    return false;
}

/*============================================================================
================================ BGuiTools ===================================
============================================================================*/

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

QRect centerOnScreenGeometry(const QSize &preferredSize, int minimumLeftOffset, int minimumTopOffset)
{
    return centerOnScreenGeometry(preferredSize.width(), preferredSize.height(), minimumLeftOffset, minimumTopOffset);
}

QRect centerOnScreenGeometry(int preferredWidth, int preferredHeight, int minimumLeftOffset, int minimumTopOffset)
{
    if (preferredWidth <= 0 || preferredHeight <= 0)
        return QRect();
    if (minimumLeftOffset < 0)
        minimumLeftOffset = 0;
    if (minimumTopOffset < 0)
        minimumTopOffset = 0;
    QRect dg = QApplication::desktop()->geometry();
    int height = qMin(preferredHeight, dg.height());
    int width = qMin(preferredWidth, dg.width());
    int topOffset = qMax((dg.height() - height) / 2, minimumTopOffset);
    int leftOffset = qMax((dg.width() - width) / 2, minimumLeftOffset);
    height = qMin(height, dg.height() - topOffset);
    width = qMin(width, dg.width() - leftOffset);
    if (height <= 0 || width <= 0)
        return QRect();
    return QRect(leftOffset, topOffset, width, height);
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

bool dockWidgetsAutoTabificationEnabled(QMainWindow *mw)
{
    if (!mw)
        return false;
    return mw->findChild<QObject *>("beqt/dock_widget_tabify_helper");
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

void setDockWidgetsAutoTabificationEnabled(QMainWindow *mw, bool enabled)
{
    if (!mw)
        return;
    QObject *o = mw->findChild<QObject *>("beqt/dock_widget_tabify_helper");
    if ((enabled && o) || (!enabled && !o))
        return;
    if (enabled) {
        BDockWidgetTabifyHelper *helper = new BDockWidgetTabifyHelper(mw);
        helper->setObjectName("beqt/dock_widget_tabify_helper");
        mw->installEventFilter(helper);
    } else {
        delete o;
    }
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

void tabifyDockWidgets(QMainWindow *mw)
{
    typedef QMap<int, Qt::DockWidgetArea> DWAMap;
    init_once(DWAMap, dwaMap, DWAMap()) {
        dwaMap.insert(1, Qt::LeftDockWidgetArea);
        dwaMap.insert(2, Qt::RightDockWidgetArea);
        dwaMap.insert(3, Qt::TopDockWidgetArea);
        dwaMap.insert(4, Qt::BottomDockWidgetArea);
    }
    if (!mw)
        return;
    QMap< int, QList<QDockWidget *> > map;
    foreach (QDockWidget *dwgt, mw->findChildren<QDockWidget *>()) {
        Qt::DockWidgetArea dwa = mw->dockWidgetArea(dwgt);
        map[dwaMap.key(dwa)] << dwgt;
    }
    foreach (int i, bRangeD(1, 4)) {
        if (!map.contains(i))
            continue;
        const QList<QDockWidget *> &list = map.value(i);
        foreach (int j, bRangeD(0, list.size() - 2))
            mw->tabifyDockWidget(list.at(j), list.at(j + 1));
    }
}

QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action)
{
    if (!toolBar || !action)
        return 0;
    return static_cast<QToolButton *>(toolBar->widgetForAction(action));
}

}
