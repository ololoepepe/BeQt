#include "blocaldocumentdriver.h"
#include "babstractdocumentdriver.h"

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

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

bool BLocalDocumentDriver::load(const QString &fileName, QTextCodec *codec)
{
    QFile f(fileName);
    if ( !f.open(QFile::ReadOnly) )
        return false;
    QTextStream in(&f);
    in.setCodec(codec);
    QString text = in.readAll();
    f.close();
    emitLoadingFinished(true, text);
    return true;
}

bool BLocalDocumentDriver::save(const QString &fileName, const QString &text, QTextCodec *codec)
{
    QFile f(fileName);
    if ( !f.open(QFile::WriteOnly) )
        return false;
    QTextStream out(&f);
    out.setCodec(codec);
    out << text;
    f.close();
    emitSavingFinished(true);
    return true;
}
