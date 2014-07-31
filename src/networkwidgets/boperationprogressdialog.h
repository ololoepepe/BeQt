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
    Q_PROPERTY(int autoCloseInterval READ autoCloseInterval WRITE setAutoCloseInterval)
    Q_PROPERTY(bool canCancel READ canCancel WRITE setCanCancel)
    Q_PROPERTY(QString failureText READ failureText WRITE setFailureText)
    Q_PROPERTY(QString processingRequestText READ processingRequestText WRITE setProcessingRequestText)
    Q_PROPERTY(QString receivingReplyText READ receivingReplyText WRITE setReceivingReplyText)
    Q_PROPERTY(QString receivingRequestText READ receivingRequestText WRITE setReceivingRequestText)
    Q_PROPERTY(QString sendingReplyText READ sendingReplyText WRITE setSendingReplyText)
    Q_PROPERTY(QString sendingRequestText READ sendingRequestText WRITE setSendingRequestText)
    Q_PROPERTY(QString successText READ successText WRITE setSuccessText)
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval)
    Q_PROPERTY(bool valid READ isValid STORED false)
    Q_PROPERTY(QString waitingForReplyText READ waitingForReplyText WRITE setWaitingForReplyText)
    Q_PROPERTY(QString waitingForStartText READ waitingForStartText WRITE setWaitingForStartText)
    B_DECLARE_PRIVATE(BOperationProgressDialog)
public:
    explicit BOperationProgressDialog(BNetworkOperation *op, QWidget *parent = 0);
    ~BOperationProgressDialog();
protected:
    explicit BOperationProgressDialog(BOperationProgressDialogPrivate &d, QWidget *parent = 0);
public:
    int autoCloseInterval() const;
    bool canCancel() const;
    QString failureText() const;
    bool isValid() const;
    BNetworkOperation *operation() const;
    QString processingRequestText() const;
    QString receivingReplyText() const;
    QString receivingRequestText() const;
    QString sendingReplyText() const;
    QString sendingRequestText() const;
    void setAutoCloseInterval(int msecs);
    void setCanCancel(bool b);
    void setFailureText(const QString &text);
    void setProcessingRequestText(const QString &text);
    void setReceivingReplyText(const QString &text);
    void setReceivingRequestText(const QString &text);
    void setSendingReplyText(const QString &text);
    void setSendingRequestText(const QString &text);
    void setSuccessText(const QString &text);
    void setUpdateInterval(int msecs);
    void setWaitingForReplyText(const QString &text);
    void setWaitingForStartText(const QString &text);
    QString successText() const;
    int updateInterval() const;
    QString waitingForReplyText() const;
    QString waitingForStartText() const;
private:
    Q_DISABLE_COPY(BOperationProgressDialog)
};

#endif // BOPERATIONPROGRESSTDIALOG_H
