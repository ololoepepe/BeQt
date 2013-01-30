class BSplittedLinesDialog;

#include "bcodeeditordocument.h"
#include "bcodeeditordocument_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeedit.h"
#include "bcodeedit_p.h"
#include "babstractfiletype.h"
#include "bcodeeditor.h"

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
================================ BCodeEditorDocumentPrivate ==================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditorDocumentPrivate::BCodeEditorDocumentPrivate(BCodeEditorDocument *q) :
    BCodeEditPrivate(q)
{
    //
}

BCodeEditorDocumentPrivate::~BCodeEditorDocumentPrivate()
{
    //
}

/*============================== Public methods ============================*/

void BCodeEditorDocumentPrivate::init()
{
    codec = QTextCodec::codecForName("UTF-8");
    asyncMin = 100 * BeQt::Kilobyte;
    fileType = 0;
    sld = 0;
    q_func()->setFileType( BAbstractFileType::defaultFileType() );
}

void BCodeEditorDocumentPrivate::setFileName(const QString &fn)
{
    bool b = (fn != fileName);
    fileName = fn;
    if (b)
        QMetaObject::invokeMethod( q_func(), "fileNameChanged", Q_ARG(QString, fn) );
}

void BCodeEditorDocumentPrivate::setCodec(QTextCodec *c)
{
    if (!c)
        return;
    bool b = (c != codec);
    codec = c;
    if (b)
        QMetaObject::invokeMethod( q_func(), "codecChanged", Q_ARG( QString, BCodeEditor::codecName(c) ) );
}

/*============================== Public slots ==============================*/

void BCodeEditorDocumentPrivate::loadingFinished(const BAbstractDocumentDriver::Operation &operation,
                                                 bool success, const QString &text)
{
    if ( operation.document != q_func() )
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>( sender() );
    disconnect( driver, SIGNAL( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ),
                this, SLOT( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ) );
    if (success)
    {
        if ( !operation.fileName.isEmpty() )
            setFileName(operation.fileName);
        if (operation.codec)
            setCodec(operation.codec);
        setText(text, asyncMin);
        ptedt->document()->setModified(false);
        ptedt->document()->clearUndoRedoStacks();
    }
    QMetaObject::invokeMethod( q_func(), "loadingFinished", Q_ARG(bool, success) );
    setBuisy(false);
}

void BCodeEditorDocumentPrivate::savingFinished(const BAbstractDocumentDriver::Operation &operation, bool success)
{
    if ( operation.document != q_func() )
        return;
    BAbstractDocumentDriver *driver = static_cast<BAbstractDocumentDriver *>( sender() );
    disconnect( driver, SIGNAL( savingFinished(BAbstractDocumentDriver::Operation, bool) ),
                this, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool) ) );
    if (success)
    {
        if ( !operation.fileName.isEmpty() )
            setFileName(operation.fileName);
        if (operation.codec)
            setCodec(operation.codec);
        ptedt->document()->setModified(false);
    }
    QMetaObject::invokeMethod( q_func(), "savingFinished", Q_ARG(bool, success) );
    setBuisy(false);
}

/*============================================================================
================================ BCodeEditorDocument =========================
============================================================================*/

/*============================== Public constructors =======================*/

BCodeEditorDocument::BCodeEditorDocument(QWidget *parent) :
    BCodeEdit(*new BCodeEditorDocumentPrivate(this), parent)
{
    d_func()->init();
}

BCodeEditorDocument::~BCodeEditorDocument()
{
    //
}

/*============================== Protected constructors ====================*/

BCodeEditorDocument::BCodeEditorDocument(BCodeEditorDocumentPrivate &d, QWidget *parent) :
    BCodeEdit(d, parent)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BCodeEditorDocument::setFileName(const QString &fn)
{
    if ( isBuisy() )
        return;
    d_func()->setFileName(fn);
}

void BCodeEditorDocument::setCodec(QTextCodec *codec)
{
    if ( isBuisy() )
        return;
    d_func()->setCodec(codec);
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

void BCodeEditorDocument::setFileType(BAbstractFileType *ft)
{
    B_D(BCodeEditorDocument);
    if (ft == d->fileType)
        return;
    if ( ft && d->fileType && ft->id() == d->fileType->id() )
        return;
    if (!ft && d->fileType && d->fileType->id() == "Text")
        return;
    d->fileType = ft ? ft : BAbstractFileType::defaultFileType();
    setHighlighter( ft->createHighlighter(this) );
    setRecognizedBrackets( ft->brackets() );
    emit fileTypeChanged(ft);
}

void BCodeEditorDocument::setSplittedLinesDialog(BSplittedLinesDialog *dlg)
{
    d_func()->sld = dlg;
}

bool BCodeEditorDocument::load(BAbstractDocumentDriver *driver, const QString &fileName)
{
    return load(driver, 0, fileName);
}

bool BCodeEditorDocument::load(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName)
{
    if ( !driver || isBuisy() )
        return false;
    B_D(BCodeEditorDocument);
    d->setBuisy(true);
    connect( driver, SIGNAL( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ),
             d, SLOT( loadingFinished(BAbstractDocumentDriver::Operation, bool, QString) ) );
    if ( !driver->load(this, codec, fileName) )
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
    return save(driver, 0, fileName);
}

bool BCodeEditorDocument::save(BAbstractDocumentDriver *driver, QTextCodec *codec, const QString &fileName)
{
    if ( !driver || isBuisy() )
        return false;
    B_D(BCodeEditorDocument);
    d->setBuisy(true);
    connect( driver, SIGNAL( savingFinished(BAbstractDocumentDriver::Operation, bool) ),
             d, SLOT( savingFinished(BAbstractDocumentDriver::Operation, bool) ) );
    if ( !driver->save(this, codec, fileName) )
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

QString BCodeEditorDocument::codecName() const
{
    return d_func()->codec ? QString::fromLatin1( d_func()->codec->name().data() ) : QString();
}

int BCodeEditorDocument::asyncProcessingMinimumLength() const
{
    return d_func()->asyncMin;
}

BAbstractFileType *BCodeEditorDocument::fileType() const
{
    return d_func()->fileType;
}

BSplittedLinesDialog *BCodeEditorDocument::splittedLinesDialog() const
{
    return d_func()->sld;
}
