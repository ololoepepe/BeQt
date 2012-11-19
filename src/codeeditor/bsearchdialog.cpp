#include "bsearchdialog.h"
#include "bsearchdialog_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QDialog>

/*============================================================================
================================ BSearchDialog Private
============================================================================*/

BSearchDialogPrivate::BSearchDialogPrivate(BSearchDialog *q) :
    BBasePrivate(q)
{
    //
}

BSearchDialogPrivate::~BSearchDialogPrivate()
{
    //
}

/*============================================================================
================================ BSearchDialog
============================================================================*/

BSearchDialog::BSearchDialog(QWidget *parent) :
    QDialog(parent), BBase( *new BSearchDialogPrivate(this) )
{
    //
}

BSearchDialog::~BSearchDialog()
{
    //
}

//

BSearchDialog::BSearchDialog(BSearchDialogPrivate &d, QWidget *parent) :
    QDialog(parent), BBase(d)
{
    //
}

