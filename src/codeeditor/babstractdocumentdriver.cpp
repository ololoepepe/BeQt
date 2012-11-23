class BCodeEdit;

class QTextCodec;

#include "babstractdocumentdriver.h"
#include "babstractdocumentdriver_p.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QQueue>
#include <QList>

#include <QDebug>

/*============================================================================
================================ Abstract Document Driver Private
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
    //
}

/*============================================================================
================================ Abstract Document Driver
============================================================================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(QObject *parent) :
    QObject(parent)
{
    d_func()->init();
}

BAbstractDocumentDriver::~BAbstractDocumentDriver()
{
    //
}

//

bool BAbstractDocumentDriver::load(BCodeEditorDocument *doc, const QString &fileName)
{
    return load(doc, 0, fileName);
}

bool BAbstractDocumentDriver::load(BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName)
{
    if ( !doc || isDocumentInList(doc) )
        return false;
    d_func()->docs << doc;
    Operation op;
    op.document = doc;
    op.fileName = fileName;
    op.codec = codec;
    d_func()->loadOps.enqueue(op);
    emit newPendingLoadOperation();
    return true;
}

bool BAbstractDocumentDriver::save(BCodeEditorDocument *doc, const QString &fileName)
{
    return save(doc, 0, fileName);
}

bool BAbstractDocumentDriver::save(BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName)
{
    if ( !doc || isDocumentInList(doc) )
        return false;
    d_func()->docs << doc;
    Operation op;
    op.document = doc;
    op.fileName = fileName;
    op.codec = codec;
    d_func()->saveOps.enqueue(op);
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

BAbstractDocumentDriver::BAbstractDocumentDriver(BAbstractDocumentDriverPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

BAbstractDocumentDriver::Operation BAbstractDocumentDriver::nextPendingLoadOperation()
{
    B_D(BAbstractDocumentDriver);
    if ( d->loadOps.isEmpty() )
    {
        Operation op;
        op.document = 0;
        op.codec = 0;
        return op;
    }
    return d->loadOps.dequeue();
}

BAbstractDocumentDriver::Operation BAbstractDocumentDriver::nextPendingSaveOperation()
{
    B_D(BAbstractDocumentDriver);
    if ( d->saveOps.isEmpty() )
    {
        Operation op;
        op.document = 0;
        op.codec = 0;
        return op;
    }
    return d->saveOps.dequeue();
}

void BAbstractDocumentDriver::emitLoadingFinished(const Operation &operation, bool success, const QString &text)
{
    B_D(BAbstractDocumentDriver);
    if ( !operation.document || !d->docs.contains(operation.document) )
        return;
    d->docs.removeAll(operation.document);
    emit loadingFinished(operation, success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(const Operation &operation, bool success)
{
    B_D(BAbstractDocumentDriver);
    if ( !operation.document || !d->docs.contains(operation.document) )
        return;
    d->docs.removeAll(operation.document);
    emit savingFinished(operation, success);
}
