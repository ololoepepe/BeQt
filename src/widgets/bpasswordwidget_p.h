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

class BPasswordWidgetPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BPasswordWidgetPrivateObject(BPasswordWidgetPrivate *p);
    ~BPasswordWidgetPrivateObject();
    //
    BPasswordWidgetPrivate *const _m_p;
public slots:
    void languageChanged();
    void tbtnSaveClicked();
    void tbtnShowClicked();
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivateObject)
};

class BPasswordWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPasswordWidget)
    Q_DECLARE_TR_FUNCTIONS(BPasswordWidget)
public:
    explicit BPasswordWidgetPrivate(BPasswordWidget *q);
    ~BPasswordWidgetPrivate();
    //
    void retranslateUi();
    void resetSave();
    void resetShow();
    //
    BPasswordWidgetPrivateObject *const _m_o;
    //
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
    //
    friend class BPasswordWidgetPrivateObject;
};

#endif // BPASSWORDWIDGET_P_H

