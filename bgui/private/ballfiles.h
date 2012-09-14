#ifndef BALLFILES_H
#define BALLFILES_H

#include "../babstractfiletype.h"

#include <QObject>
#include <QString>
#include <QStringList>

class BAllFiles : public BAbstractFileType
{
    Q_OBJECT
public:
    BAllFiles(QObject *parent = 0);
    //
    QString description() const;
    QStringList suffixes() const;
    BAbstractFileType *clone() const;
};

#endif // BALLFILES_H
