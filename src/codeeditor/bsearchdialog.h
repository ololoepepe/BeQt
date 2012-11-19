#ifndef BSEARCHDIALOG_H
#define BSEARCHDIALOG_H

class BSearchDialogPrivate;

class QWidget;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>
#include <QDialog>

/*============================================================================
================================ Search Dialog
============================================================================*/

class BSearchDialog : public QDialog, public BBase
{
    B_DECLARE_PRIVATE(BSearchDialog)
    Q_OBJECT
public:
    explicit BSearchDialog(QWidget *parent = 0);
    ~BSearchDialog();
    //
protected:
    BSearchDialog(BSearchDialogPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BSearchDialog)
};

#endif // BSEARCHDIALOG_H

