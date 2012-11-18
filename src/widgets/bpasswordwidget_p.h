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

class B_WIDGETS_EXPORT BPasswordWidgetPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BPasswordWidget)
    Q_OBJECT
public:
    explicit BPasswordWidgetPrivateObject(BPasswordWidgetPrivate *p);
    ~BPasswordWidgetPrivateObject();
public slots:
    void languageChanged();
    void tbtnSaveClicked();
    void tbtnShowClicked();
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivateObject)
};

class B_WIDGETS_EXPORT BPasswordWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPasswordWidget)
    B_DECLARE_OBJECT(BPasswordWidget)
    Q_DECLARE_TR_FUNCTIONS(BPasswordWidget)
public:
    static const QDataStream::Version DSVersion;
    //
    explicit BPasswordWidgetPrivate(BPasswordWidget *q);
    ~BPasswordWidgetPrivate();
    //
    void retranslateUi();
    void resetSave();
    void resetShow();
    //
    QByteArray encPassword;
    bool save;
    bool show;
    int charCount;
    QHBoxLayout *hlt;
      QLineEdit *ledt;
      QToolButton *tbtnSave;
      QToolButton *tbtnShow;
protected:
    BPasswordWidgetPrivate(BPasswordWidget &q, BPasswordWidgetPrivateObject &o);
private:
    Q_DISABLE_COPY(BPasswordWidgetPrivate)
};

#endif // BPASSWORDWIDGET_P_H

