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

bool BLocalDocumentDriver::load(BCodeEditorDocument *doc, bool *finished)
{
    if (finished)
        *finished = true;
    QFile f( doc->fileName() );
    if ( !f.open(QFile::ReadOnly) )
        return false;
    QTextStream in(&f);
    in.setCodec( doc->codec() );
    QString text = in.readAll();
    f.close();
    emitLoadingFinished(doc, true, text);
    return true;
}

bool BLocalDocumentDriver::save(BCodeEditorDocument *doc, bool *finished)
{
    if (finished)
        *finished = true;
    QFile f( doc->fileName() );
    if ( !f.open(QFile::WriteOnly) )
        return false;
    QTextStream out(&f);
    out.setCodec( doc->codec() );
    out << doc->text();
    f.close();
    emitSavingFinished(doc, true);
    return true;
}
