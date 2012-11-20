class BCodeEdit;

#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QQueue>
#include <QList>

/*============================================================================
================================ Abstract Document Driver Private (declaration)
============================================================================*/

class BAbstractDocumentDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BAbstractDocumentDriver)
public:
    explicit BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q);
    ~BAbstractDocumentDriverPrivate();
    //
    void init();
    //
    QQueue<BAbstractDocumentDriver::Operation> loadOps;
    QQueue<BAbstractDocumentDriver::Operation> saveOps;
    QList<BCodeEditorDocument *> docs;
private:
    Q_DISABLE_COPY(BAbstractDocumentDriverPrivate)
};

/*============================================================================
================================ Abstract Document Driver Private (definition)
============================================================================*/

BAbstractDocumentDriverPrivate::BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q) :
    BBasePrivate(q)
{
    //
}

BAbstractDocumentDriverPrivate::~BAbstractDocumentDriverPrivate()
{
    //
}

//

void BAbstractDocumentDriverPrivate::init()
{
    BBasePrivate::init();
}

/*============================================================================
================================ Abstract Document Driver
============================================================================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(QObject *parent) :
    QObject(parent)
{
    //
}

BAbstractDocumentDriver::~BAbstractDocumentDriver()
{
    //
}

//

bool BAbstractDocumentDriver::load(BCodeEditorDocument *doc, const QString &fileName)
{
    if ( !doc || isDocumentInList(doc) )
        return false;
    d_func()->loadOps.enqueue( Operation(doc, fileName) );
    emit newPendingLoadOperation();
    return true;
}

bool BAbstractDocumentDriver::save(BCodeEditorDocument *doc, const QString &fileName)
{
    if ( !doc || isDocumentInList(doc) )
        return false;
    d_func()->saveOps.enqueue( Operation(doc, fileName) );
    emit newPendingSaveOperation();
    return true;
}

bool BAbstractDocumentDriver::hasPendingLoadOperations() const
{
    return !d_func()->loadOps.isEmpty();
}

bool BAbstractDocumentDriver::hasPendingSaveOperations() const
{
    return !d_func()->saveOps.isEmpty();
}

bool BAbstractDocumentDriver::isDocumentInList(BCodeEditorDocument *doc) const
{
    return doc && d_func()->docs.contains(doc);
}

//

BAbstractDocumentDriver::Operation BAbstractDocumentDriver::nextPendingLoadOperation()
{
    B_D(BAbstractDocumentDriver);
    return !d->loadOps.isEmpty() ? d->loadOps.dequeue() : Operation();
}

BAbstractDocumentDriver::Operation BAbstractDocumentDriver::nextPendingSaveOperation()
{
    B_D(BAbstractDocumentDriver);
    return !d->saveOps.isEmpty() ? d->saveOps.dequeue() : Operation();
}

void BAbstractDocumentDriver::emitLoadingFinished(const Operation &operation, bool success, const QString &text)
{
    if (!operation.document)
        return;
    d_func()->docs.removeAll(operation.document);
    emit loadingFinished(operation, success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(const Operation &operation, bool success)
{
    if (!operation.document)
        return;
    d_func()->docs.removeAll(operation.document);
    emit savingFinished(operation, success);
}
