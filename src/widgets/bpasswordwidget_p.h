#ifndef BPASSWORDWIDGET_P_H
#define BPASSWORDWIDGET_P_H

class BPasswordWidgetPrivate;

class QByteArray;
class QHBoxLayout;
class QLineEdit;
class QToolButton;

#include "bpasswordwidget.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QApplication>
#include <QDataStream>

/*============================================================================
================================ Password Widget Private
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPasswordWidget)
    Q_OBJECT
public:
    static const QDataStream::Version DSVersion;
    //
    explicit BPasswordWidgetPrivate(BPasswordWidget *q);
    ~BPasswordWidgetPrivate();
    //
    QByteArray encPassword;
    bool save;
    bool show;
    int charCount;
    QHBoxLayout *hlt;
      QLineEdit *ledt;
      QToolButton *tbtnSave;
      QToolButton *tbtnShow;
public slots:
    void retranslateUi();
    void resetSave();
    void resetShow();
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivate)
};

#endif // BPASSWORDWIDGET_P_H

