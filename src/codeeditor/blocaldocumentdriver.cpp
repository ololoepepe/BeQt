#include "blocaldocumentdriver.h"
#include "blocaldocumentdriver_p.h"
#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>

#include <QDebug>

/*============================================================================
================================ Local Document Driver Private
============================================================================*/

BLocalDocumentDriverPrivate::BLocalDocumentDriverPrivate(BLocalDocumentDriver *q) :
    BAbstractDocumentDriverPrivate(q)
{
    //
}

BLocalDocumentDriverPrivate::~BLocalDocumentDriverPrivate()
{
    //
}

//

void BLocalDocumentDriverPrivate::init()
{
    B_Q(BLocalDocumentDriver);
    connect( q, SIGNAL( newPendingLoadOperation() ), this, SLOT( newPendingLoadOperation() ) );
    connect( q, SIGNAL( newPendingSaveOperation() ), this, SLOT( newPendingSaveOperation() ) );
}

//

void BLocalDocumentDriverPrivate::newPendingLoadOperation()
{
    B_Q(BLocalDocumentDriver);
    BLocalDocumentDriver::Operation op = q->nextPendingLoadOperation();
    QFile f( !op.fileName.isEmpty() ? op.fileName : op.document->fileName() );
    if ( !f.open(QFile::ReadOnly) )
        return q->emitLoadingFinished(op, false);
    QTextStream in(&f);
    in.setCodec( op.codec ? op.codec : op.document->codec() );
    QString text = in.readAll();
    f.close();
    q->emitLoadingFinished(op, true, text);
}

void BLocalDocumentDriverPrivate::newPendingSaveOperation()
{
    B_Q(BLocalDocumentDriver);
    BLocalDocumentDriver::Operation op = q->nextPendingSaveOperation();
    QFile f( !op.fileName.isEmpty() ? op.fileName : op.document->fileName() );
    if ( !f.open(QFile::WriteOnly) )
        return q->emitSavingFinished(op, false);
    QTextStream out(&f);
    out.setCodec( op.codec ? op.codec : op.document->codec() );
    out << op.document->text();
    f.close();
    q->emitSavingFinished(op, true);
}

/*============================================================================
================================ Local Document Driver
============================================================================*/

BLocalDocumentDriver::BLocalDocumentDriver(QObject *parent) :
    BAbstractDocumentDriver(*new BLocalDocumentDriverPrivate(this), parent)
{
    d_func()->init();
}

BLocalDocumentDriver::~BLocalDocumentDriver()
{
    //
}

//

QString BLocalDocumentDriver::id() const
{
    return "beqt/local";
}

bool BLocalDocumentDriver::shouldSaveAs(const QString &fileName)
{
    return !QFileInfo(fileName).isFile();
}

//

BLocalDocumentDriver::BLocalDocumentDriver(BLocalDocumentDriverPrivate &d, QObject *parent) :
    BAbstractDocumentDriver(d, parent)
{
    d_func()->init();
}
