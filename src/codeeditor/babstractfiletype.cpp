#include "babstractfiletype.h"
#include "bcodeedit.h"

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QSyntaxHighlighter>
#include <QList>

#include <QDebug>

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

class BDefaultFileType : public BAbstractFileType
{
    Q_DECLARE_TR_FUNCTIONS(BDefaultFileType)
public:
    BDefaultFileType();
    ~BDefaultFileType();
public:
    QString id() const;
    QString name() const;
    QString description() const;
    QStringList suffixes() const;
    bool matchesFileName(const QString &fileName) const;
    QSyntaxHighlighter *createHighlighter() const;
    QList<BCodeEdit::BracketPair> brackets() const;
};

/*============================================================================
================================ BDefaultFileType ============================
============================================================================*/

/*============================== Public constructors =======================*/

BDefaultFileType::BDefaultFileType()
{
    //
}

BDefaultFileType::~BDefaultFileType()
{
    //
}

/*============================== Public methods ============================*/

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

QList<BCodeEdit::BracketPair> BDefaultFileType::brackets() const
{
    //test
    QList<BCodeEdit::BracketPair> list;
    BCodeEdit::BracketPair br;
    br.opening = "(";
    br.closing = ")";
    br.escape = "\\";
    list << br;
    return list;
    //end test
    //return QList<BCodeEdit::BracketPair>();
}

/*============================================================================
================================ BAbstractFileType ===========================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractFileType::BAbstractFileType()
{
}

BAbstractFileType::~BAbstractFileType()
{
    //
}

/*============================== Static public methods =====================*/

BAbstractFileType *BAbstractFileType::defaultFileType()
{
    return new BDefaultFileType;
}

/*============================== Public methods ============================*/

QString BAbstractFileType::createFileDialogFilter() const
{
    QString desc = description();
    QStringList sl = suffixes();
    if ( desc.isEmpty() || sl.isEmpty() )
        return "";
    QString filter;
    filter += desc + " (";
    if ( sl.contains("*") )
        sl.clear();
    sl.removeDuplicates();
    if ( !sl.isEmpty() )
        for (int i = 0; i < sl.size(); ++i)
            filter += "*." + sl.at(i) + (i < sl.size() - 1 ? " " : "");
    else
        filter += "*";
    filter += ")";
    return filter;
}
