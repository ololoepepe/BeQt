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

bool BLocalDocumentDriver::load(BCodeEditorDocument *doc, bool *finished, bool *success, QString *text)
{
    if (finished)
        *finished = true;
    if (success)
        *success = true;
    QFile f( doc->fileName() );
    if ( !f.open(QFile::ReadOnly) )
    {
        if (success)
            *success = false;
        return false;
    }
    QTextStream in(&f);
    in.setCodec( doc->codec() );
    QString txt = in.readAll();
    if (text)
        *text = txt;
    f.close();
    return true;
}

bool BLocalDocumentDriver::save(BCodeEditorDocument *doc, bool *finished, bool *success)
{
    if (finished)
        *finished = true;
    if (success)
        *success = true;
    QFile f( doc->fileName() );
    if ( !f.open(QFile::WriteOnly) )
    {
        if (success)
            *success = false;
        return false;
    }
    QTextStream out(&f);
    out.setCodec( doc->codec() );
    out << doc->text();
    f.close();
    return true;
}
