#include "boperationprogressdialog.h"
#include "boperationprogressdialog_p.h"

#include <BeQtNetwork/BNetworkOperation>
#include <BeQtCore/BCoreApplication>
#include <BeQtCore/BSignalDelayProxy>

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QDialogButtonBox>
#include <QTimer>
#include <QPushButton>

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
    q->setStretchEnabled(true);
    proxy = new BSignalDelayProxy(500, 1000);
    connect(proxy, SIGNAL(triggered()), this, SLOT(update()));
    canCancel = true;
    autoCloseInterval = -1;
    if (Operation)
    {
        connect(Operation, SIGNAL(finished()), this, SLOT(update()));
        connect(Operation, SIGNAL(error()), this, SLOT(update()));
        connect(Operation, SIGNAL(uploadProgress(qint64, qint64)), proxy, SLOT(trigger()));
        connect(Operation, SIGNAL(downloadProgress(qint64, qint64)), proxy, SLOT(trigger()));
    }
    QWidget *wgt = new QWidget;
      QVBoxLayout *vlt = new QVBoxLayout(wgt);
        lbl = new QLabel;
        vlt->addWidget(lbl);
        pbar = new QProgressBar;
          pbar->setMinimum(0);
        vlt->addWidget(pbar);
    q->setWidget(wgt);
    btn = q->addButton(" ", QDialogButtonBox::RejectRole, this, SLOT(btnClicked()));
    connect(BCoreApplication::instance(), SIGNAL(languageChanged()), this, SLOT(update()));
    update();
}

/*============================== Public slots ==============================*/

void BOperationProgressDialogPrivate::update()
{
    if (Operation)
    {
        if (Operation->isFinished())
        {
            lbl->setText(!successText.isEmpty() ? successText : tr("Operation successfully completed", "lbl text"));
            pbar->setMaximum(100);
            pbar->setValue(qMax(Operation->uploadProgress(), Operation->downloadProgress()));
            btn->setText(tr("Close", "btn text"));
            btn->setEnabled(true);
            if (autoCloseInterval > 0)
                QTimer::singleShot(autoCloseInterval, q_func(), SLOT(close()));
            else if (!autoCloseInterval)
                q_func()->close();
        }
        else if (Operation->isError())
        {
            lbl->setText(!failureText.isEmpty() ? failureText : tr("Operation failed", "lbl text"));
            pbar->setMaximum(100);
            pbar->setValue(qMax(Operation->uploadProgress(), Operation->downloadProgress()));
            btn->setText(tr("Close", "btn text"));
            btn->setEnabled(true);
            if (autoCloseInterval > 0)
                QTimer::singleShot(autoCloseInterval, q_func(), SLOT(close()));
            else if (!autoCloseInterval)
                q_func()->close();
        }
        else if (Operation->isRequest())
        {
            if (Operation->downloadBytesTotal() > 0)
            {
                lbl->setText(!receivingReplyText.isEmpty() ? receivingReplyText :
                                                             tr("Receiving reply...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->downloadProgress());
            }
            else if (Operation->uploadBytesTotal() == Operation->uploadBytesReady())
            {
                lbl->setText(!waitingForReplyText.isEmpty() ? waitingForReplyText :
                                                              tr("Waiting for reply...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            }
            else if (Operation->uploadBytesTotal() > 0)
            {
                lbl->setText(!sendingRequestText.isEmpty() ? sendingRequestText :
                                                             tr("Sending request...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->uploadProgress());
            }
            else
            {
                lbl->setText(!waitingForStartText.isEmpty() ? waitingForStartText :
                                                              tr("Waiting for the operation start...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            }
            btn->setEnabled(canCancel);
            btn->setText(tr("Cancel", "btn text"));
        }
        else
        {
            if (Operation->uploadBytesTotal() > 0)
            {
                lbl->setText(!sendingReplyText.isEmpty() ? sendingReplyText : tr("Sending reply...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->uploadProgress());
            }
            else if (Operation->downloadBytesTotal() == Operation->downloadBytesReady())
            {
                lbl->setText(!processingRequestText.isEmpty() ? processingRequestText :
                                                                tr("Processing request...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            }
            else if (Operation->downloadBytesTotal() > 0)
            {
                lbl->setText(!receivingRequestText.isEmpty() ? receivingRequestText :
                                                               tr("Receiving request...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->downloadProgress());
            }
            else
            {
                lbl->setText(!waitingForStartText.isEmpty() ? waitingForStartText :
                                                              tr("Waiting for the operation start...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            }
            btn->setEnabled(canCancel);
            btn->setText(tr("Cancel", "btn text"));
        }
    }
    else
    {
        lbl->setText(tr("Invalid operation", "lbl text"));
        pbar->setMaximum(100);
        pbar->setValue(0);
        btn->setEnabled(true);
        btn->setText(tr("Close", "btn text"));
    }
}

void BOperationProgressDialogPrivate::btnClicked()
{
    if (Operation && !Operation->isFinished() && !Operation->isError())
        Operation->cancel();
    else
        q_func()->close();
}

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
    d_func()->waitingForStartText = text;
    d_func()->update();
}

void BOperationProgressDialog::setSendingRequestText(const QString &text)
{
    d_func()->sendingRequestText = text;
    d_func()->update();
}

void BOperationProgressDialog::setReceivingRequestText(const QString &text)
{
    d_func()->receivingRequestText = text;
    d_func()->update();
}

void BOperationProgressDialog::setWaitingForReplyText(const QString &text)
{
    d_func()->waitingForReplyText = text;
    d_func()->update();
}

void BOperationProgressDialog::setProcessingRequestText(const QString &text)
{
    d_func()->processingRequestText = text;
    d_func()->update();
}

void BOperationProgressDialog::setReceivingReplyText(const QString &text)
{
    d_func()->receivingReplyText = text;
    d_func()->update();
}

void BOperationProgressDialog::setSendingReplyText(const QString &text)
{
    d_func()->sendingReplyText = text;
    d_func()->update();
}

void BOperationProgressDialog::setSuccessText(const QString &text)
{
    d_func()->successText = text;
    d_func()->update();
}

void BOperationProgressDialog::setFailureText(const QString &text)
{
    d_func()->failureText = text;
    d_func()->update();
}

void BOperationProgressDialog::setCanCancel(bool b)
{
    d_func()->canCancel = b;
    d_func()->update();
}

void BOperationProgressDialog::setUpdateInterval(int msecs)
{
    if (msecs < 2)
        return;
    d_func()->proxy->setMaximumDelay(msecs);
    d_func()->proxy->setIntermediateDelay(msecs / 2);
}

void BOperationProgressDialog::setAutoCloseInterval(int msecs)
{
    B_D(BOperationProgressDialog);
    bool b = d->autoCloseInterval;
    d->autoCloseInterval = msecs > 0 ? msecs : 0;
    if ((!d->Operation || d->Operation->isFinished() || d->Operation->isError()) && !b && msecs > 0)
        QTimer::singleShot(msecs, this, SLOT(close()));
}

BNetworkOperation *BOperationProgressDialog::operation() const
{
    return d_func()->Operation;
}

bool BOperationProgressDialog::canCancel() const
{
    return d_func()->canCancel;
}

int BOperationProgressDialog::updateInterval() const
{
    return d_func()->proxy->maximumDelay();
}

int BOperationProgressDialog::autoCloseInterval() const
{
    return d_func()->autoCloseInterval;
}

bool BOperationProgressDialog::isValid() const
{
    return d_func()->Operation;
}
