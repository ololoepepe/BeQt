#include "boperationprogressdialog.h"
#include "boperationprogressdialog_p.h"

#include <BeQtNetwork/BNetworkOperation>

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>

#include <QDebug>

/*============================================================================
================================ BOperationProgressDialogPrivate =============
============================================================================*/

/*============================== Public constructors =======================*/

BOperationProgressDialogPrivate::BOperationProgressDialogPrivate(BOperationProgressDialog *q, BNetworkOperation *op) :
    BDialogPrivate(q), Operation(op)
{
    //
}

BOperationProgressDialogPrivate::~BOperationProgressDialogPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BOperationProgressDialogPrivate::init()
{
    B_Q(BOperationProgressDialog);
    q->setWaitingForStartText("");
    q->setSendingRequestText("");
    q->setReceivingRequestText("");
    q->setWaitingForReplyText("");
    q->setProcessingRequestText("");
    q->setReceivingReplyText("");
    q->setSendingReplyText("");
    q->setSuccessText("");
    q->setFailureText("");
    q->setStretchEnabled(true);
    QWidget *wgt = new QWidget;
      QVBoxLayout *vlt = new QVBoxLayout(wgt);
        lbl = new QLabel;
        vlt->addWidget(lbl);
        pbar = new QProgressBar;
        vlt->addWidget(pbar);
    q->setWidget(wgt);
}

/*============================== Public slots ==============================*/

//

/*============================================================================
================================ BOperationProgressDialog ====================
============================================================================*/

/*============================== Public constructors =======================*/


BOperationProgressDialog::BOperationProgressDialog(BNetworkOperation *op, QWidget *parent) :
    BDialog(*new BOperationProgressDialogPrivate(this, op), parent)
{
    d_func()->init();
}

BOperationProgressDialog::~BOperationProgressDialog()
{
    //
}

/*============================== Protected constructors ====================*/

BOperationProgressDialog::BOperationProgressDialog(BOperationProgressDialogPrivate &d, QWidget *parent) :
    BDialog(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BOperationProgressDialog::setWaitingForStartText(const QString &text)
{
    d_func()->waitingForStartText = !text.isEmpty() ? text : tr("Waiting for the operation start...");
}

void BOperationProgressDialog::setSendingRequestText(const QString &text)
{
    d_func()->sendingRequestText = !text.isEmpty() ? text : tr("Sending request...");
}

void BOperationProgressDialog::setReceivingRequestText(const QString &text)
{
    d_func()->receivingRequestText = !text.isEmpty() ? text : tr("Receiving request...");
}

void BOperationProgressDialog::setWaitingForReplyText(const QString &text)
{
    d_func()->waitingForReplyText = !text.isEmpty() ? text : tr("Waiting for reply...");
}

void BOperationProgressDialog::setProcessingRequestText(const QString &text)
{
    d_func()->processingRequestText = !text.isEmpty() ? text : tr("Processing request...");
}

void BOperationProgressDialog::setReceivingReplyText(const QString &text)
{
    d_func()->receivingReplyText = !text.isEmpty() ? text : tr("Receiving reply...");
}

void BOperationProgressDialog::setSendingReplyText(const QString &text)
{
    d_func()->sendingReplyText = !text.isEmpty() ? text : tr("Sending reply...");
}

void BOperationProgressDialog::setSuccessText(const QString &text)
{
    d_func()->successText = !text.isEmpty() ? text : tr("Operation successfully completed");
}

void BOperationProgressDialog::setFailureText(const QString &text)
{
    d_func()->failureText = !text.isEmpty() ? text : tr("Operation failed");
}

BNetworkOperation *BOperationProgressDialog::operation() const
{
    return d_func()->Operation;
}

bool BOperationProgressDialog::isValid() const
{
    return d_func()->Operation;
}

/*============================== Public slots ==============================*/

//
