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

#ifndef BGUITOOLS_H
#define BGUITOOLS_H

class BTranslation;

class QAction;
class QComboBox;
class QFont;
class QMainWindow;
class QObject;
class QRect;
class QSize;
class QString;
class QToolBar;
class QToolButton;
class QVBoxLayout;
class QWidget;

#include "bapplication.h"

#include <BeQtCore/BDynamicTranslator>

#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QStack>

/*============================================================================
================================ BGuiTools ===================================
============================================================================*/

namespace BGuiTools
{

enum StandardAction
{
    AboutAction = 1,
    ContextualHelpAction,
    HelpContentsAction,
    HomepageAction,
    SettingsAction,
    WhatsThisAction
};

template <typename T> QFormLayout *formLayout(T *field)
{
    QWidget *pw = field ? field->parentWidget() : 0;
    if (!pw)
        return 0;
    QStack<QLayout *> s;
    s.push(pw->layout());
    while (!s.isEmpty()) {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        QFormLayout *flt = qobject_cast<QFormLayout *>(lt);
        if (flt) {
            QWidget *w = flt->labelForField(field);
            if (w)
                return flt;
        }
        foreach (int i, bRangeD(0, lt->count() - 1))
            s.push(lt->itemAt(i)->layout());
    }
    return 0;
}

template <typename T, typename U> T *labelForField(U *field)
{
    QWidget *pw = field ? field->parentWidget() : 0;
    if (!pw)
        return 0;
    QStack<QLayout *> s;
    s.push(pw->layout());
    while (!s.isEmpty()) {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        QFormLayout *flt = qobject_cast<QFormLayout *>(lt);
        if (flt) {
            QWidget *w = flt->labelForField(field);
            if (w)
                return qobject_cast<T *>(w);
        }
        foreach (int i, bRangeD(0, lt->count() - 1))
            s.push(lt->itemAt(i)->layout());
    }
    return 0;
}

template <typename T> void installTranslatorOnLabel(T *field, const BTranslation &translation)
{
    QLabel *lbl = labelForField<QLabel>(field);
    if (!lbl)
        return;
    new BDynamicTranslator(lbl, bApp, SIGNAL(languageChanged()), "text", translation);
}

B_WIDGETS_EXPORT void addRow(QVBoxLayout *vlt, const QString &label, QWidget *field);
B_WIDGETS_EXPORT void addRow(QVBoxLayout *vlt, const QString &label, QLayout *field);
B_WIDGETS_EXPORT QRect centerOnScreenGeometry(const QSize &preferredSize, int minimumLeftOffset = 0,
                                              int minimumTopOffset = 0);
B_WIDGETS_EXPORT QRect centerOnScreenGeometry(int preferredWidth, int preferredHeight, int minimumLeftOffset = 0,
                                              int minimumTopOffset = 0);
B_WIDGETS_EXPORT QFrame *createFrame(QFrame::Shape shape, QWidget *parent = 0);
B_WIDGETS_EXPORT QFrame *createFrame(QFrame::Shape shape, QFrame::Shadow shadow, QWidget *parent = 0);
B_WIDGETS_EXPORT QFont createMonospaceFont(int pointSize = 0);
B_WIDGETS_EXPORT QAction *createSeparator(QObject *parent = 0);
B_WIDGETS_EXPORT QAction *createStandardAction(StandardAction type, QObject *parent = 0);
B_WIDGETS_EXPORT bool dockWidgetsAutoTabificationEnabled(QMainWindow *mw);
B_WIDGETS_EXPORT void removeRow(QVBoxLayout *vlt, QWidget *field);
B_WIDGETS_EXPORT void removeRow(QVBoxLayout *vlt, QLayout *field);
B_WIDGETS_EXPORT void setDockWidgetsAutoTabificationEnabled(QMainWindow *mw, bool enabled);
B_WIDGETS_EXPORT void setItemEnabled(QComboBox *cmbox, int index, bool enabled);
B_WIDGETS_EXPORT void setRowEnabled(QWidget *field, bool enabled);
B_WIDGETS_EXPORT void setRowEnabled(QLayout *field, bool enabled);
B_WIDGETS_EXPORT void setRowVisible(QWidget *field, bool visible);
B_WIDGETS_EXPORT void setRowVisible(QLayout *field, bool visible);
B_WIDGETS_EXPORT void tabifyDockWidgets(QMainWindow *mw);
B_WIDGETS_EXPORT QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action);

}

#endif // BGUITOOLS_H
