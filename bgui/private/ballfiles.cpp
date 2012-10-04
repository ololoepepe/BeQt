#include "ballfiles.h"
#include "../babstractfiletype.h"

#include <QObject>
#include <QString>
#include <QStringList>

BAllFiles::BAllFiles(QObject *parent) :
    BAbstractFileType(parent)
{
}

//

QString BAllFiles::description() const
{
    return tr("All files", "fileType description");
}

QStringList BAllFiles::suffixes() const
{
    return QStringList();
}

BAbstractFileType *BAllFiles::clone() const
{
    return new BAllFiles( parent() );
}
