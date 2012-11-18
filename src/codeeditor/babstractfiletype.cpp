#include "babstractfiletype.h"

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QList>

/*========== Default File Type (declaration) ==========*/

class BDefaultFileType : public BAbstractFileType
{
    Q_DECLARE_TR_FUNCTIONS(BDefaultFileType)
public:
    BDefaultFileType();
    ~BDefaultFileType();
    //
    QString id() const;
    QString name() const;
    QString description() const;
    QStringList suffixes() const;
    bool matchesFileName(const QString &fileName) const;
    QSyntaxHighlighter *createHighlighter() const;
    QList<BracketPair> brackets() const;
};

/*========== Default File Type (definition) ==========*/

BDefaultFileType::BDefaultFileType()
{
    //
}

BDefaultFileType::~BDefaultFileType()
{
    //
}

//

QString BDefaultFileType::id() const
{
    return "Text";
}

QString BDefaultFileType::name() const
{
    return tr("Text", "name");
}

QString BDefaultFileType::description() const
{
    return tr("All files", "description");
}

QStringList BDefaultFileType::suffixes() const
{
    return QStringList() << "*";
}

bool BDefaultFileType::matchesFileName(const QString &fileName) const
{
    return true;
}

QSyntaxHighlighter *BDefaultFileType::createHighlighter() const
{
    return 0;
}

QList<BAbstractFileType::BracketPair> BDefaultFileType::brackets() const
{
    return QList<BracketPair>();
}

/*========== Abstract File Type ==========*/

BAbstractFileType *BAbstractFileType::defaultFileType()
{
    return new BDefaultFileType;
}

//

BAbstractFileType::BAbstractFileType()
{
}

BAbstractFileType::~BAbstractFileType()
{
    //
}

//

QString BAbstractFileType::createFileDialogFilter() const
{
    QString desc = description();
    QStringList sl = suffixes();
    if ( !desc.isEmpty() || suffixes().isEmpty() )
        return "";
    QString filter;
    filter += desc + " (";
    if ( !sl.isEmpty() )
    {
        sl.removeDuplicates();
        for (int i = 0; i < sl.size(); ++i)
            filter += "*." + sl.at(i) + (i < sl.size() - 1 ? " " : "");
    }
    else
    {
        filter += "*";
    }
    filter += ")";
    return filter;
}
