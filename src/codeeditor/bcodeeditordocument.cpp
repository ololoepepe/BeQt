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

/*============================================================================
================================ Code Editor Document Private
============================================================================*/

BCodeEditorDocumentPrivate::BCodeEditorDocumentPrivate(BCodeEditorDocument *q) :
    BCodeEditPrivate(q)
{
    codec = QTextCodec::codecForName("UTF-8");
    asyncMin = 100 * BeQt::Kilobyte;
    buisy = false;
}

BCodeEditorDocumentPrivate::~BCodeEditorDocumentPrivate()
{
    //
}

//

void BCodeEditorDocumentPrivate::loadingFinished(BCodeEditorDocument *doc, bool success, const QString &text)
{
    if ( doc != q_func() )
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>( sender() );
    disconnect( driver, SIGNAL( loadingFinished(BCodeEditorDocument *, bool, QString) ),
                this, SLOT( loadingFinished(BCodeEditorDocument *, bool, QString) ) );
    B_Q(BCodeEditorDocument);
    if (success)
    {
        q->setText(text, asyncMin);
        q->innerEdit()->document()->setModified(false);
    }
    QMetaObject::invokeMethod( q, "loadingFinished", Q_ARG(bool, success) );
    buisy = false;
    QMetaObject::invokeMethod( q, "buisyChanged", Q_ARG(bool, false) );
}

void BCodeEditorDocumentPrivate::savingFinished(BCodeEditorDocument *doc, bool success)
{
    if ( doc != q_func() )
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>( sender() );
    disconnect( driver, SIGNAL( savingFinished(BCodeEditorDocument *, bool, QString) ),
                this, SLOT( savingFinished(BCodeEditorDocument *, bool, QString) ) );
    B_Q(BCodeEditorDocument);
    if (success)
        q->innerEdit()->document()->setModified(false);
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
    //
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

bool BCodeEditorDocument::load(BAbstractDocumentDriver *driver)
{
    if ( !driver || isBuisy() )
        return false;
    B_D(BCodeEditorDocument);
    bool fin = false;
    bool success = false;
    QString txt;
    if ( !driver->load(this, &fin, &success, &txt) )
        return false;
    if (fin)
    {
        d->loadingFinished(this, success, txt);
    }
    else
    {
        connect( driver, SIGNAL( loadingFinished(BCodeEditorDocument *, bool, QString) ),
                 d, SLOT( loadingFinished(BCodeEditorDocument *, bool, QString) ) );
        d->buisy = true;
        emit buisyChanged(true);
    }
    return true;
}

bool BCodeEditorDocument::save(BAbstractDocumentDriver *driver)
{
    if ( !driver || isBuisy() )
        return false;
    B_D(BCodeEditorDocument);
    bool fin = false;
    bool success = false;
    if ( !driver->save(this, &fin, &success) )
        return false;
    if (fin)
    {
        d->savingFinished(this, success);
    }
    else
    {
        connect( driver, SIGNAL( savingFinished(BCodeEditorDocument *, bool, QString) ),
                 d, SLOT( savingFinished(BCodeEditorDocument *, bool, QString) ) );
        d->buisy = true;
        emit buisyChanged(true);
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

bool BCodeEditorDocument::isBuisy() const
{
    return d_func()->buisy;
}

//

BCodeEditorDocument::BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent) :
    BCodeEdit(d, parent)
{
    //
}

