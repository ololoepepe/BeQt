class BCodeEditor;

class QTextCodec;

#include "babstractdocumentdriver.h"
#include "babstractdocumentdriver_p.h"
#include "bcodeeditordocument.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QByteArray>

#include <QDebug>

/*============================================================================
================================ BAbstractDocumentDriverPrivate ==============
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractDocumentDriverPrivate::BAbstractDocumentDriverPrivate(BAbstractDocumentDriver *q) :
    BBasePrivate(q)
{
    //
}

BAbstractDocumentDriverPrivate::~BAbstractDocumentDriverPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BAbstractDocumentDriverPrivate::init()
{
    editor = 0;
}

void BAbstractDocumentDriverPrivate::setEditor(BCodeEditor *edr)
{
    editor = edr;
}

/*============================================================================
================================ BAbstractDocumentDriver =====================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(QObject *parent) :
    QObject(parent)
{
    d_func()->init();
}

BAbstractDocumentDriver::~BAbstractDocumentDriver()
{
    //
}

/*============================== Protected constructors ====================*/

BAbstractDocumentDriver::BAbstractDocumentDriver(BAbstractDocumentDriverPrivate &d, QObject *parent) :
    QObject(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QByteArray BAbstractDocumentDriver::saveState() const
{
    return QByteArray();
}

void BAbstractDocumentDriver::restoreState(const QByteArray &)
{
    //
}

bool BAbstractDocumentDriver::load(BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName)
{
    if (!doc)
        return false;
    Operation op;
    op.document = doc;
    op.fileName = fileName;
    op.codec = codec;
    return handleLoadOperation(op);
}

bool BAbstractDocumentDriver::save(BCodeEditorDocument *doc, QTextCodec *codec, const QString &fileName)
{
    if (!doc)
        return false;
    Operation op;
    op.document = doc;
    op.fileName = fileName;
    op.codec = codec;
    return handleSaveOperation(op);
}

bool BAbstractDocumentDriver::load(BCodeEditorDocument *doc, const QString &fileName)
{
    return load(doc, 0, fileName);
}

bool BAbstractDocumentDriver::save(BCodeEditorDocument *doc, const QString &fileName)
{
    return save(doc, 0, fileName);
}

BCodeEditor *BAbstractDocumentDriver::editor() const
{
    return d_func()->editor;
}

/*============================== Protected methods =========================*/

void BAbstractDocumentDriver::emitLoadingFinished(const Operation &operation, bool success, const QString &text)
{
    Q_EMIT loadingFinished(operation, success, text);
}

void BAbstractDocumentDriver::emitSavingFinished(const Operation &operation, bool success)
{
    Q_EMIT savingFinished(operation, success);
}
