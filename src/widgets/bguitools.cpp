class QAction;
class QObject;
class QWidget;
class QFont;
class QToolButton;
class QToolBar;
class QVBoxLayout;
class QString;
class QLayout;
class QFormLayout;

#include "bguitools.h"
#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>

#include <QFrame>

namespace BGuiTools
{

QAction *createStandardAction(BApplication::StandardAction type, QObject *parent)
{
    return BApplication::createStandardAction(type, parent);
}

QAction *createSeparator(QObject *parent)
{
    return BApplication::createSeparator(parent);
}

QFrame *createFrame(QFrame::Shape shape, QWidget *parent)
{
    return BApplication::createFrame(shape, parent);
}

QFrame *createFrame(QFrame::Shape shape, QFrame::Shadow shadow, QWidget *parent)
{
    return BApplication::createFrame(shape, shadow, parent);
}

QFont createMonospaceFont()
{
    return BApplication::createMonospaceFont();
}

QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action)
{
    return BApplication::toolButtonForAction(toolBar, action);
}

void addRow(QVBoxLayout *vlt, const QString &label, QWidget *field)
{
    return BApplication::addRow(vlt, label, field);
}

void addRow(QVBoxLayout *vlt, const QString &label, QLayout *field)
{
    return BApplication::addRow(vlt, label, field);
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

QFormLayout *formLayout(QWidget *field)
{
    return BApplication::formLayout(field);
}

QFormLayout *formLayout(QLayout *field)
{
    return BApplication::formLayout(field);
}

void setRowVisible(QWidget *field, bool visible)
{
    return BApplication::setRowVisible(field, visible);
}

void setRowVisible(QLayout *field, bool visible)
{
    return BApplication::setRowVisible(field, visible);
}

void setRowEnabled(QWidget *field, bool enabled)
{
    return BApplication::setRowEnabled(field, enabled);
}

void setRowEnabled(QLayout *field, bool enabled)
{
    return BApplication::setRowEnabled(field, enabled);
}

}
