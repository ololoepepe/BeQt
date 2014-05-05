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
