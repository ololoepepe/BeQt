#include "bcodeeditordocument.h"
#include "bcodeeditordocument_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeedit.h"
#include "bcodeedit_p.h"
#include "bplaintextedit.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>
#include <QList>
#include <QString>
#include <QTextCodec>
#include <QPointer>
#include <QByteArray>
#include <QMap>
#include <QTextDocument>

#include <QDebug>

/*============================================================================
================================ Code Editor Document Private
============================================================================*/

BCodeEditorDocumentPrivate::BCodeEditorDocumentPrivate(BCodeEditorDocument *q) :
    BCodeEditPrivate(q)
{
    //
}

BCodeEditorDocumentPrivate::~BCodeEditorDocumentPrivate()
{
    //
}

//

void BCodeEditorDocumentPrivate::init()
{
    codec = QTextCodec::codecForName("UTF-8");
    asyncMin = 100 * BeQt::Kilobyte;
}

//

void BCodeEditorDocumentPrivate::loadingFinished(const BAbstractDocumentDriver::Operation &operation,
                                                 bool success, const QString &text)
{
    if ( operation.document != q_func() )
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>( sender() );
    disconnect( driver, SIGNAL( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ),
                this, SLOT( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ) );
    B_Q(BCodeEditorDocument);
    if (success)
    {
        if ( !operation.fileName.isEmpty() )
            q->setFileName(operation.fileName);
        q->setText(text, asyncMin);
        q->innerEdit()->document()->setModified(false);
    }
    QMetaObject::invokeMethod( q, "loadingFinished", Q_ARG(bool, success) );
    buisy = false;
    QMetaObject::invokeMethod( q, "buisyChanged", Q_ARG(bool, false) );
}

void BCodeEditorDocumentPrivate::savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success)
{
    if ( operation.document != q_func() )
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>( sender() );
    disconnect( driver, SIGNAL( savingFinished(BAbstractDocumentDriver::Operation, bool) ),
                this, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool) ) );
    B_Q(BCodeEditorDocument);
    if (success)
    {
        if ( !operation.fileName.isEmpty() )
            q->setFileName(operation.fileName);
        q->innerEdit()->document()->setModified(false);
    }
    QMetaObject::invokeMethod( q, "savingFinished", Q_ARG(bool, success) );
    buisy = false;
    QMetaObject::invokeMethod( q, "buisyChanged", Q_ARG(bool, false) );
}

/*============================================================================
================================ Code Editor Document
============================================================================*/

BCodeEditorDocument::BCodeEditorDocument(QWidget *parent) :
    BCodeEdit(*new BCodeEditorDocumentPrivate(this), parent)
{
    d_func()->init();
}

BCodeEditorDocument::~BCodeEditorDocument()
{
    //
}

void BCodeEditorDocument::setFileName(const QString &fn)
{
    if ( isBuisy() )
        return;
    B_D(BCodeEditorDocument);
    bool b = (fn == d->fileName);
    d->fileName = fn;
    if (b)
        emit fileNameChanged(fn);
}

void BCodeEditorDocument::setCodec(QTextCodec *codec)
{
    if ( isBuisy() )
        return;
    if (!codec)
        return;
    B_D(BCodeEditorDocument);
    bool b = (codec == d->codec);
    d->codec = codec;
    if (b)
        emit codecChanged( QString::fromLatin1( codec->name() ) );
}

void BCodeEditorDocument::setCodec(const char *codecName)
{
    setCodec( QTextCodec::codecForName(codecName) );
}

void BCodeEditorDocument::setAsyncProcessingMinimumLength(int length)
{
    if (length < 0)
        length = 0;
    d_func()->asyncMin = length;
}

bool BCodeEditorDocument::load(BAbstractDocumentDriver *driver, const QString &fileName)
{
    if ( !driver || isBuisy() )
        return false;
    B_D(BCodeEditorDocument);
    d->setBuisy(true);
    connect( driver, SIGNAL( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ),
             d, SLOT( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ) );
    if ( !driver->load(this, fileName) )
    {
        disconnect( driver, SIGNAL( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ),
                    d, SLOT( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ) );
        d->setBuisy(false);
        return false;
    }
    return true;
}

bool BCodeEditorDocument::save(BAbstractDocumentDriver *driver, const QString &fileName)
{
    if ( !driver || isBuisy() )
        return false;
    B_D(BCodeEditorDocument);
    d->setBuisy(true);
    connect( driver, SIGNAL( savingFinished(BAbstractDocumentDriver::Operation, bool) ),
             d, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool) ) );
    if ( !driver->save(this, fileName) )
    {
        disconnect( driver, SIGNAL( savingFinished(BAbstractDocumentDriver::Operation, bool) ),
                    d, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool) ) );
        d->setBuisy(false);
        return false;
    }
    return true;
}

QString BCodeEditorDocument::fileName() const
{
    return d_func()->fileName;
}

QTextCodec *BCodeEditorDocument::codec() const
{
    return d_func()->codec;
}

int BCodeEditorDocument::asyncProcessingMinimumLength() const
{
    return d_func()->asyncMin;
}

//

BCodeEditorDocument::BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent) :
    BCodeEdit(d, parent)
{
    d_func()->init();
}

