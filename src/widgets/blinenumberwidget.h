#ifndef BLINENUMBERWIDGET_H
#define BLINENUMBERWIDGET_H

class BLineNumberWidgetPrivate;

class QPaintEvent;
class QSize;
class QPlainTextEdit;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QObject>

/*============================================================================
================================ BLineNumberWidget ===========================
============================================================================*/

class B_WIDGETS_EXPORT BLineNumberWidget : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLineNumberWidget)
public:
    explicit BLineNumberWidget(QPlainTextEdit *edit);
    ~BLineNumberWidget();
public:
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *e);
private:
    Q_DISABLE_COPY(BLineNumberWidget)
};

#endif // BLINENUMBERWIDGET_H
