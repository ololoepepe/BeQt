#ifndef BPASSWORDWIDGET_P_H
#define BPASSWORDWIDGET_P_H

class BPasswordWidgetPrivate;

class QByteArray;
class QHBoxLayout;
class QLineEdit;
class QToolButton;
class QEvent;

#include "bpasswordwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QApplication>
#include <QDataStream>

/*============================================================================
================================ BPasswordWidgetPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidgetPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPasswordWidget)
public:
    static const QDataStream::Version DSVersion;
public:
    explicit BPasswordWidgetPrivate(BPasswordWidget *q);
    ~BPasswordWidgetPrivate();
public:
    static BPasswordWidget::PasswordWidgetData createPasswordWidgetData();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
public slots:
    void retranslateUi();
    void resetSave();
    void resetShow();
public:
    QByteArray encPassword;
    bool save;
    bool show;
    int charCount;
    QHBoxLayout *hlt;
      QLineEdit *ledt;
      QToolButton *tbtnSave;
      QToolButton *tbtnShow;
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivate)
};

#endif // BPASSWORDWIDGET_P_H
