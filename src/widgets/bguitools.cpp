
class QObject;
class QWidget;
class QFont;
class QVBoxLayout;
class QString;
class QLayout;
class QFormLayout;

#include "bguitools.h"
#include "bapplication.h"

#include <BeQtCore/BeQtGlobal>

#include <QFrame>
#include <QAction>
#include <QToolButton>
#include <QToolBar>
#include <QHBoxLayout>
#include <QLabel>

namespace BGuiTools
{

QAction *createStandardAction(BApplication::StandardAction type, QObject *parent)
{
    return BApplication::createStandardAction(type, parent);
}

QAction *createSeparator(QObject *parent)
{
    QAction *act = new QAction(parent);
    act->setSeparator(true);
    return act;
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

QFont createMonospaceFont()
{
#if defined Q_OS_WIN
    //On Windows some weird fonts are selected, so we set the font explicitly
    QString f = "Courier New";
#else
    QString f = "monospace";
#endif
    //Using such a construct to guarantee that the font will be monospaced
    QFont fnt(QFontInfo(QFont(f)).family());
    fnt.setPointSize(QApplication::font().pointSize());
    return fnt;
}

QToolButton *toolButtonForAction(QToolBar *toolBar, QAction *action)
{
    if (!toolBar || !action)
        return 0;
    return static_cast<QToolButton *>(toolBar->widgetForAction(action));
}

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
    while (!s.isEmpty())
    {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        foreach (int i, bRangeD(0, lt->count() - 1))
        {
            s.push(lt->itemAt(i)->layout());
            QWidget *w = lt->itemAt(i)->widget();
            if (w)
                w->setVisible(visible);
        }
    }
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
    while (!s.isEmpty())
    {
        QLayout *lt = s.pop();
        if (!lt)
            continue;
        foreach (int i, bRangeD(0, lt->count() - 1))
        {
            s.push(lt->itemAt(i)->layout());
            QWidget *w = lt->itemAt(i)->widget();
            if (w)
                w->setEnabled(enabled);
        }
    }
}

}
