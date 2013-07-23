#ifndef BOPERATIONPROGRESSTDIALOG_P_H
#define BOPERATIONPROGRESSTDIALOG_P_H

class BNetworkOperation;
class BSignalDelayProxy;

class QLabel;
class QProgressBar;
class QPushButton;

#include "boperationprogressdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtWidgets/private/bdialog_p.h>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QString>

/*============================================================================
================================ BOperationProgressDialogPrivate =============
============================================================================*/

class B_NETWORKWIDGETS_EXPORT BOperationProgressDialogPrivate : public BDialogPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BOperationProgressDialog)
public:
    explicit BOperationProgressDialogPrivate(BOperationProgressDialog *q, BNetworkOperation *op);
    ~BOperationProgressDialogPrivate();
public:
    void init();
public Q_SLOTS:
    void update();
    void btnClicked();
public:
    BNetworkOperation *const Operation;
public:
    QString waitingForStartText;
    QString sendingRequestText;
    QString receivingRequestText;
    QString waitingForReplyText;
    QString processingRequestText;
    QString receivingReplyText;
    QString sendingReplyText;
    QString successText;
    QString failureText;
    BSignalDelayProxy *proxy;
    bool canCancel;
    int autoCloseInterval;
    QLabel *lbl;
    QProgressBar *pbar;
    QPushButton *btn;
private:
    Q_DISABLE_COPY(BOperationProgressDialogPrivate)
};

#endif // BOPERATIONPROGRESSTDIALOG_P_H
