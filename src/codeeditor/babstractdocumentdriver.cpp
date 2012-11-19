class BCodeEdit;

#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QQueue>

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
    QQueue<BCodeEditorDocument *> loadOps;
    QQueue<BCodeEditorDocument *> saveOps;
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

bool BAbstractDocumentDriver::load(BCodeEditorDocument *doc, bool *finished, bool *success, QString *text)
{
    if (finished)
        *finished = true;
    if (success)
        *success = true;
    if (!doc)
        return false;
    B_D(BAbstractDocumentDriver);
    if ( d->loadOps.contains(doc) || d->saveOps.contains(doc) )
    {
        if (success)
            *success = false;
        return false;
    }
    d_func()->loadOps.enqueue(doc);
    emit newPendingLoadOperation();
    return true;
}

bool BAbstractDocumentDriver::save(BCodeEditorDocument *doc, bool *finished, bool *success)
{
    if (finished)
        *finished = true;
    if (success)
        *success = true;
    if (!doc)
        return false;
    B_D(BAbstractDocumentDriver);
    if ( d->saveOps.contains(doc) || d->saveOps.contains(doc) )
    {
        if (success)
            *success = false;
        return false;
    }
    d_func()->saveOps.enqueue(doc);
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

//

BCodeEditorDocument *BAbstractDocumentDriver::nextPendingLoadOperation()
{
    B_D(BAbstractDocumentDriver);
    return !d->loadOps.isEmpty() ? d->loadOps.dequeue() : 0;
}

BCodeEditorDocument *BAbstractDocumentDriver::nextPendingSaveOperation()
{
    B_D(BAbstractDocumentDriver);
    return !d->saveOps.isEmpty() ? d->saveOps.dequeue() : 0;
}

void BAbstractDocumentDriver::emitLoadingFinished(BCodeEditorDocument *doc, bool success, const QString &text)
{
    emit loadingFinished(doc, success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(BCodeEditorDocument *doc, bool success)
{
    emit savingFinished(doc, success);
}
