#ifndef BSEARCHDIALOG_P_H
#define BSEARCHDIALOG_P_H

#include "bsearchdialog.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Search Dialog Private
============================================================================*/

class BSearchDialogPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSearchDialog)
    Q_OBJECT
public:
    explicit BSearchDialogPrivate(BSearchDialog *q);
    ~BSearchDialogPrivate();
    //
private:
    Q_DISABLE_COPY(BSearchDialogPrivate)
};

#endif // BSEARCHDIALOG_P_H

