#ifndef BABSTRACTDOCUMENTDRIVER_P_H
#define BABSTRACTDOCUMENTDRIVER_P_H

class BCodeEditor;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ Abstract Document Driver Private
============================================================================*/

class BAbstractDocumentDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractDocumentDriver)
    Q_OBJECT
public:
    explicit BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q);
    ~BAbstractDocumentDriverPrivate();
    //
    void init();
    void setEditor(BCodeEditor *edr);
    //
    BCodeEditor *editor;
private:
    Q_DISABLE_COPY(BAbstractDocumentDriverPrivate)
};

#endif // BABSTRACTDOCUMENTDRIVER_P_H

