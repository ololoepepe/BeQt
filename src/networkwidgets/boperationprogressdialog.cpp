/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetworkWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "boperationprogressdialog.h"
#include "boperationprogressdialog_p.h"

#include <BeQtCore/BSignalDelayProxy>
#include <BeQtNetwork/BNetworkOperation>
#include <BeQtWidgets/BApplication>

#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

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
    proxy = new BSignalDelayProxy(250, 500);
    connect(proxy, SIGNAL(triggered()), this, SLOT(update()));
    canCancel = true;
    autoCloseInterval = -1;
    if (Operation) {
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
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(update()));
    update();
}

/*============================== Public slots ==============================*/

void BOperationProgressDialogPrivate::btnClicked()
{
    if (Operation && !Operation->isFinished() && !Operation->isError())
        Operation->cancel();
    else if (Operation && Operation->isFinished())
        q_func()->accept();
    else
        q_func()->reject();
}

void BOperationProgressDialogPrivate::update()
{
    if (Operation) {
        if (Operation->isFinished()) {
            lbl->setText(!successText.isEmpty() ? successText : tr("Operation successfully completed", "lbl text"));
            pbar->setMaximum(100);
            pbar->setValue(qMax(Operation->uploadProgress(), Operation->downloadProgress()));
            btn->setText(tr("Close", "btn text"));
            btn->setEnabled(true);
            if (autoCloseInterval > 0)
                QTimer::singleShot(autoCloseInterval, q_func(), SLOT(accept()));
            else
                q_func()->accept();
        } else if (Operation->isError()) {
            lbl->setText(!failureText.isEmpty() ? failureText : tr("Operation failed", "lbl text"));
            pbar->setMaximum(100);
            pbar->setValue(qMax(Operation->uploadProgress(), Operation->downloadProgress()));
            btn->setText(tr("Close", "btn text"));
            btn->setEnabled(true);
            if (autoCloseInterval > 0)
                QTimer::singleShot(autoCloseInterval, q_func(), SLOT(reject()));
                q_func()->reject();
        } else if (Operation->isRequest()) {
            if (Operation->downloadBytesTotal() > 0) {
                lbl->setText(!receivingReplyText.isEmpty() ? receivingReplyText :
                                                             tr("Receiving data...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->downloadProgress());
            } else if (Operation->uploadBytesTotal() == Operation->uploadBytesReady()) {
                lbl->setText(!waitingForReplyText.isEmpty() ? waitingForReplyText :
                                                              tr("Waiting for reply...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            } else if (Operation->uploadBytesTotal() > 0) {
                lbl->setText(!sendingRequestText.isEmpty() ? sendingRequestText :
                                                             tr("Sending data...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->uploadProgress());
            } else {
                lbl->setText(!waitingForStartText.isEmpty() ? waitingForStartText :
                                                              tr("Waiting for the operation start...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            }
            btn->setEnabled(canCancel);
            btn->setText(tr("Cancel", "btn text"));
        } else {
            if (Operation->uploadBytesTotal() > 0)
            {
                lbl->setText(!sendingReplyText.isEmpty() ? sendingReplyText : tr("Sending data...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->uploadProgress());
            } else if (Operation->downloadBytesTotal() == Operation->downloadBytesReady()) {
                lbl->setText(!processingRequestText.isEmpty() ? processingRequestText :
                                                                tr("Processing request...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            } else if (Operation->downloadBytesTotal() > 0) {
                lbl->setText(!receivingRequestText.isEmpty() ? receivingRequestText :
                                                               tr("Receiving data...", "lbl text"));
                pbar->setMaximum(100);
                pbar->setValue(Operation->downloadProgress());
            } else {
                lbl->setText(!waitingForStartText.isEmpty() ? waitingForStartText :
                                                              tr("Waiting for the operation start...", "lbl text"));
                pbar->setValue(0);
                pbar->setMaximum(0);
            }
            btn->setEnabled(canCancel);
            btn->setText(tr("Cancel", "btn text"));
        }
    } else {
        lbl->setText(tr("Invalid operation", "lbl text"));
        pbar->setMaximum(100);
        pbar->setValue(0);
        btn->setEnabled(true);
        btn->setText(tr("Close", "btn text"));
    }
    QApplication::processEvents();
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

int BOperationProgressDialog::autoCloseInterval() const
{
    return d_func()->autoCloseInterval;
}

bool BOperationProgressDialog::canCancel() const
{
    return d_func()->canCancel;
}

QString BOperationProgressDialog::failureText() const
{
    return d_func()->failureText;
}

bool BOperationProgressDialog::isValid() const
{
    return d_func()->Operation;
}

BNetworkOperation *BOperationProgressDialog::operation() const
{
    return d_func()->Operation;
}

QString BOperationProgressDialog::processingRequestText() const
{
    return d_func()->processingRequestText;
}

QString BOperationProgressDialog::receivingReplyText() const
{
    return d_func()->receivingReplyText;
}

QString BOperationProgressDialog::receivingRequestText() const
{
    return d_func()->receivingRequestText;
}

QString BOperationProgressDialog::sendingReplyText() const
{
    return d_func()->sendingReplyText;
}

QString BOperationProgressDialog::sendingRequestText() const
{
    return d_func()->sendingRequestText;
}

void BOperationProgressDialog::setAutoCloseInterval(int msecs)
{
    B_D(BOperationProgressDialog);
    bool b = d->autoCloseInterval;
    d->autoCloseInterval = msecs > 0 ? msecs : 0;
    if (b && msecs > 0) {
        if (d->Operation && d->Operation->isFinished())
            QTimer::singleShot(msecs, this, SLOT(accept()));
        else
            QTimer::singleShot(msecs, this, SLOT(reject()));
    }
}

void BOperationProgressDialog::setCanCancel(bool b)
{
    d_func()->canCancel = b;
    d_func()->update();
}

void BOperationProgressDialog::setFailureText(const QString &text)
{
    d_func()->failureText = text;
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

void BOperationProgressDialog::setReceivingRequestText(const QString &text)
{
    d_func()->receivingRequestText = text;
    d_func()->update();
}

void BOperationProgressDialog::setSendingReplyText(const QString &text)
{
    d_func()->sendingReplyText = text;
    d_func()->update();
}

void BOperationProgressDialog::setSendingRequestText(const QString &text)
{
    d_func()->sendingRequestText = text;
    d_func()->update();
}

void BOperationProgressDialog::setSuccessText(const QString &text)
{
    d_func()->successText = text;
    d_func()->update();
}

void BOperationProgressDialog::setUpdateInterval(int msecs)
{
    if (msecs < 2)
        return;
    d_func()->proxy->setMaximumDelay(msecs);
    d_func()->proxy->setIntermediateDelay(msecs / 2);
}

void BOperationProgressDialog::setWaitingForReplyText(const QString &text)
{
    d_func()->waitingForReplyText = text;
    d_func()->update();
}

void BOperationProgressDialog::setWaitingForStartText(const QString &text)
{
    d_func()->waitingForStartText = text;
    d_func()->update();
}

QString BOperationProgressDialog::successText() const
{
    return d_func()->successText;
}

int BOperationProgressDialog::updateInterval() const
{
    return d_func()->proxy->maximumDelay();
}

QString BOperationProgressDialog::waitingForReplyText() const
{
    return d_func()->waitingForReplyText;
}

QString BOperationProgressDialog::waitingForStartText() const
{
    return d_func()->waitingForStartText;
}
