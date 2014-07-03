#ifndef BGUITOOLS_H
#define BGUITOOLS_H

class QAction;
class QObject;
class QWidget;
class QFont;
class QToolButton;
class QToolBar;
class QVBoxLayout;
class QString;

#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>

#include <QFrame>
#include <QStack>
#include <QFormLayout>
#include <QLayout>

/*============================================================================
================================ BGuiTools ===================================
============================================================================*/

namespace BGuiTools
{

template <typename T, typename U> T *labelForField(U *field)
{
    QWidget *pw = field ? field->parentWidget() : 0;
    if (!pw)
        return 0;
    QStack<QLayout *> s;
    s.push(pw->layout());
    while (!s.isEmpty())
    {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        QFormLayout *flt = qobject_cast<QFormLayout *>(lt);
        if (flt)
        {
            QWidget *w = flt->labelForField(field);
            if (w)
                return qobject_cast<T *>(w);
        }
        foreach (int i, bRangeD(0, lt->count() - 1))
            s.push(lt->itemAt(i)->layout());
    }
    return 0;
}

template <typename T> QFormLayout *formLayout(T *field)
{
    QWidget *pw = field ? field->parentWidget() : 0;
    if (!pw)
        return 0;
    QStack<QLayout *> s;
    s.push(pw->layout());
    while (!s.isEmpty())
    {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        QFormLayout *flt = qobject_cast<QFormLayout *>(lt);
        if (flt)
        {
            QWidget *w = flt->labelForField(field);
            if (w)
                return flt;
        }
        foreach (int i, bRangeD(0, lt->count() - 1))
            s.push(lt->itemAt(i)->layout());
    }
    return 0;
}

B_WIDGETS_EXPORT QAction *createStandardAction(BApplication::StandardAction type, QObject *parent = 0);
B_WIDGETS_EXPORT QAction *createSeparator(QObject *parent = 0);
B_WIDGETS_EXPORT QFrame *createFrame(QFrame::Shape shape, QWidget *parent = 0);
B_WIDGETS_EXPORT QFrame *createFrame(QFrame::Shape shape, QFrame::Shadow shadow, QWidget *parent = 0);
B_WIDGETS_EXPORT QFont createMonospaceFont(int pointSize = 0);
B_WIDGETS_EXPORT QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action);
B_WIDGETS_EXPORT void addRow(QVBoxLayout *vlt, const QString &label, QWidget *field);
B_WIDGETS_EXPORT void addRow(QVBoxLayout *vlt, const QString &label, QLayout *field);
B_WIDGETS_EXPORT void removeRow(QVBoxLayout *vlt, QWidget *field);
B_WIDGETS_EXPORT void removeRow(QVBoxLayout *vlt, QLayout *field);
B_WIDGETS_EXPORT void setRowVisible(QWidget *field, bool visible);
B_WIDGETS_EXPORT void setRowVisible(QLayout *field, bool visible);
B_WIDGETS_EXPORT void setRowEnabled(QWidget *field, bool enabled);
B_WIDGETS_EXPORT void setRowEnabled(QLayout *field, bool enabled);

}

#endif // BGUITOOLS_H

