#ifndef BLINENUMBERWIDGET_P_H
#define BLINENUMBERWIDGET_P_H

class QPlainTextEdit;
class QPaintEvent;
class QRect;
class QEvent;

#include "blinenumberwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BLineNumberWidgetPrivate ====================
============================================================================*/

class B_WIDGETS_EXPORT BLineNumberWidgetPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BLineNumberWidget)
public:
    explicit BLineNumberWidgetPrivate(BLineNumberWidget *q, QPlainTextEdit *edit);
    ~BLineNumberWidgetPrivate();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    int getWidth() const;
    bool paintEvent(QPaintEvent *e);
public Q_SLOTS:
    void update(const QRect &rect, int dy);
    void resize();
public:
    QPlainTextEdit *const Edit;
private:
    Q_DISABLE_COPY(BLineNumberWidgetPrivate)
};

#endif // BLINENUMBERWIDGET_P_H
