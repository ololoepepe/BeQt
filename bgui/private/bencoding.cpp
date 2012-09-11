#include "bencoding.h"

#include <QString>
#include <QObject>
#include <QMap>
#include <QList>

BEncoding::BEncoding()
{
}

//

QString BEncoding::descriptiveName() const
{
    return QString();
}

QString BEncoding::fullName() const
{
    QString dname = descriptiveName();
    return dname.isEmpty() ? codecName() : dname + " (" + codecName() + ")";
}
