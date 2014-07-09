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

#ifndef BOPERATIONPROGRESSTDIALOG_P_H
#define BOPERATIONPROGRESSTDIALOG_P_H

class BNetworkOperation;
class BSignalDelayProxy;

class QLabel;
class QProgressBar;
class QPushButton;

#include "boperationprogressdialog.h"

#include <BeQtWidgets/private/bdialog_p.h>

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
    BNetworkOperation * const Operation;
public:
    int autoCloseInterval;
    QPushButton *btn;
    bool canCancel;
    QString failureText;
    QLabel *lbl;
    QProgressBar *pbar;
    QString processingRequestText;
    BSignalDelayProxy *proxy;
    QString receivingReplyText;
    QString receivingRequestText;
    QString sendingReplyText;
    QString sendingRequestText;
    QString successText;
    QString waitingForReplyText;
    QString waitingForStartText;
public:
    explicit BOperationProgressDialogPrivate(BOperationProgressDialog *q, BNetworkOperation *op);
    ~BOperationProgressDialogPrivate();
public:
    void init();
public Q_SLOTS:
    void btnClicked();
    void update();
private:
    Q_DISABLE_COPY(BOperationProgressDialogPrivate)
};

#endif // BOPERATIONPROGRESSTDIALOG_P_H
