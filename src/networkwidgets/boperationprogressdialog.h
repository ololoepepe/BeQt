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
