#ifndef BLOCALDOCUMENTDRIVER_P_H
#define BLOCALDOCUMENTDRIVER_P_H

#include "blocaldocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Local Document Driver Private
============================================================================*/

class B_CODEEDITOR_EXPORT BLocalDocumentDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocalDocumentDriver)
    Q_OBJECT
public:
    explicit BLocalDocumentDriverPrivate(BLocalDocumentDriver *q);
    ~BLocalDocumentDriverPrivate();
    //
    void init();
public slots:
    void newPendingLoadOperation();
    void newPendingSaveOperation();
private:
    Q_DISABLE_COPY(BLocalDocumentDriverPrivate)
};

#endif // BLOCALDOCUMENTDRIVER_P_H

