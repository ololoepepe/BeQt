#ifndef BOPERATIONPROGRESSTDIALOG_P_H
#define BOPERATIONPROGRESSTDIALOG_P_H

class BNetworkOperation;

class QLabel;
class QProgressBar;

#include "boperationprogressdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtWidgets/private/bdialog_p.h>
#include <BeQtCore/BPersonInfoProvider>

#include <QObject>
#include <QString>

/*============================================================================
================================ BOperationProgressDialogPrivate =============
============================================================================*/

class B_WIDGETS_EXPORT BOperationProgressDialogPrivate : public BDialogPrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BOperationProgressDialog)
public:
    explicit BOperationProgressDialogPrivate(BOperationProgressDialog *q, BNetworkOperation *op);
    ~BOperationProgressDialogPrivate();
public:
    void init();
    //
public Q_SLOTS:
    //
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
    QLabel *lbl;
    QProgressBar *pbar;
private:
    Q_DISABLE_COPY(BOperationProgressDialogPrivate)
};

#endif // BOPERATIONPROGRESSTDIALOG_P_H
