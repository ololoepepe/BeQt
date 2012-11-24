#ifndef BABSTRACTDOCUMENTDRIVER_P_H
#define BABSTRACTDOCUMENTDRIVER_P_H

class BCodeEditorDocument;
class BCodeEditor;

#include "babstractdocumentdriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QQueue>
#include <QList>

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
    QQueue<BAbstractDocumentDriver::Operation> loadOps;
    QQueue<BAbstractDocumentDriver::Operation> saveOps;
    QList<BCodeEditorDocument *> docs;
private:
    Q_DISABLE_COPY(BAbstractDocumentDriverPrivate)
};

#endif // BABSTRACTDOCUMENTDRIVER_P_H

