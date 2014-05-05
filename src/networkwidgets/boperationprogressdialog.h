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

#ifndef BOPERATIONPROGRESSTDIALOG_H
#define BOPERATIONPROGRESSTDIALOG_H

class BOperationProgressDialogPrivate;
class BNetworkOperation;

class QWidget;
class QString;

#include <BeQtWidgets/BDialog>

#include <QObject>

/*============================================================================
================================ BOperationProgressDialog ====================
============================================================================*/

class B_NETWORKWIDGETS_EXPORT BOperationProgressDialog : public BDialog
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BOperationProgressDialog)
public:
    explicit BOperationProgressDialog(BNetworkOperation *op, QWidget *parent = 0);
    ~BOperationProgressDialog();
protected:
    explicit BOperationProgressDialog(BOperationProgressDialogPrivate &d, QWidget *parent = 0);
public:
    void setWaitingForStartText(const QString &text);
    void setSendingRequestText(const QString &text);
    void setReceivingRequestText(const QString &text);
    void setWaitingForReplyText(const QString &text);
    void setProcessingRequestText(const QString &text);
    void setReceivingReplyText(const QString &text);
    void setSendingReplyText(const QString &text);
    void setSuccessText(const QString &text);
    void setFailureText(const QString &text);
    void setCanCancel(bool b);
    void setUpdateInterval(int msecs);
    void setAutoCloseInterval(int msecs);
    BNetworkOperation *operation() const;
    bool canCancel() const;
    int updateInterval() const;
    int autoCloseInterval() const;
    bool isValid() const;
private:
    Q_DISABLE_COPY(BOperationProgressDialog)
};

#endif // BOPERATIONPROGRESSTDIALOG_H
