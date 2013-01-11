#ifndef BABSTRACTDOCUMENTDRIVER_P_H
#define BABSTRACTDOCUMENTDRIVER_P_H

class BCodeEditor;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>

/*============================================================================
================================ BAbstractDocumentDriverPrivate ==============
============================================================================*/

class BAbstractDocumentDriverPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BAbstractDocumentDriver)
public:
    explicit BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q);
    ~BAbstractDocumentDriverPrivate();
public:
    void init();
    void setEditor(BCodeEditor *edr);
public:
    BCodeEditor *editor;
private:
    Q_DISABLE_COPY(BAbstractDocumentDriverPrivate)
};

#endif // BABSTRACTDOCUMENTDRIVER_P_H
