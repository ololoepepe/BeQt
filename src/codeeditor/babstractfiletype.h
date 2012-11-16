#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

class QString;
class QStringList;

class BAbstractFileType
{
public:
    BAbstractFileType();
    virtual ~BAbstractFileType();
    //
    virtual QString name() const = 0;
    virtual QStringList suffixes() const = 0;
};

#endif // BABSTRACTFILETYPE_H
