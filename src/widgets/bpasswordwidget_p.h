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
#include <BeQtCore/BPassword>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QApplication>
#include <QList>
#include <QChar>

/*============================================================================
================================ BPasswordWidgetPrivate ======================
============================================================================*/

class B_WIDGETS_EXPORT BPasswordWidgetPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPasswordWidget)
public:
    explicit BPasswordWidgetPrivate(BPasswordWidget *q);
    ~BPasswordWidgetPrivate();
public:
    void init();
    bool eventFilter(QObject *o, QEvent *e);
    void updateEdit();
public Q_SLOTS:
    void retranslateUi();
    void resetSave();
    void resetShow();
    void passwordChanged(const QString &password);
public:
    bool wasEmpty;
    BPassword pwd;
    BPassword pwdBackup;
    bool save;
    bool show;
    BPasswordWidget::GeneratePasswordFunction generateFunction;
    int generatedLength;
    QHBoxLayout *hlt;
      QLineEdit *ledt;
      QToolButton *tbtnSave;
      QToolButton *tbtnShow;
      QToolButton *tbtnGenerate;
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivate)
};

#endif // BPASSWORDWIDGET_P_H
