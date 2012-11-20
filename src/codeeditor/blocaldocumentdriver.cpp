#include "blocaldocumentdriver.h"
#include "babstractdocumentdriver.h"
#include "bcodeeditordocument.h"

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

/*============================================================================
================================ Local Document Driver
============================================================================*/

BLocalDocumentDriver::BLocalDocumentDriver(QObject *parent) :
    BAbstractDocumentDriver(parent)
{
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

bool BLocalDocumentDriver::load(BCodeEditorDocument *doc, const QString &fileName)
{
    if ( !doc || isDocumentInList(doc) )
        return false;
    Operation op(doc, fileName);
    QFile f( !fileName.isEmpty() ? fileName : doc->fileName() );
    if ( !f.open(QFile::ReadOnly) )
    {
        emitLoadingFinished(op, false);
        return true;
    }
    QTextStream in(&f);
    in.setCodec( doc->codec() );
    QString text = in.readAll();
    f.close();
    emitLoadingFinished(op, true, text);
    return true;
}

bool BLocalDocumentDriver::save(BCodeEditorDocument *doc, const QString &fileName)
{
    if ( !doc || isDocumentInList(doc) )
        return false;
    Operation op(doc, fileName);
    QFile f( !fileName.isEmpty() ? fileName : doc->fileName() );
    if ( !f.open(QFile::WriteOnly) )
    {
        emitSavingFinished(op, false);
        return true;
    }
    QTextStream out(&f);
    out.setCodec( doc->codec() );
    out << doc->text();
    f.close();
    return true;
}
