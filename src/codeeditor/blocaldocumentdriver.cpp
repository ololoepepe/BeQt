#include "blocaldocumentdriver.h"
#include "babstractdocumentdriver.h"
#include "bcodeedit.h"

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

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

bool BLocalDocumentDriver::load(BCodeEdit *edit, const QString &fileName, const QString &codecName)
{
    if (!edit)
        return false;
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return false;
    QTextStream in(&f);
    in.setCodec( codecName.toLatin1().data() );
    QString text = in.readAll();
    f.close();
    emitLoadingFinished(edit, true, text);
    return true;
}

bool BLocalDocumentDriver::save(BCodeEdit *edit, const QString &fileName, const QString &text, const QString &codecName)
{
    if (!edit)
        return false;
    QFile f(fileName);
    if ( !f.open(QFile::WriteOnly) )
        return false;
    QTextStream out(&f);
    out.setCodec( codecName.toLatin1().data() );
    out << text;
    f.close();
    emitSavingFinished(edit, true);
    return true;
}
