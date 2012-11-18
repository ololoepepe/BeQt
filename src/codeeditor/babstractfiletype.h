#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

class QStringList;
class QSyntaxHighlighter;

#include <QString>
#include <QList>
#include <QApplication>

/*============================================================================
================================ Abstract File Type
============================================================================*/

class BAbstractFileType
{
public:
    struct BracketPair
    {
        QString opening;
        QString closing;
        QString escape;
        //
        BracketPair()
        {
            //
        }
        BracketPair(const QString &op, const QString &cl, const QString &esc)
        {
            opening = op;
            closing = cl;
            escape = esc;
        }
        bool operator=(const BracketPair &other) const
        {
            return opening == other.opening && closing == other.closing && escape == other.escape;
        }
    };
    //
    static BAbstractFileType *defaultFileType();
    //
    BAbstractFileType();
    virtual ~BAbstractFileType();
    //
    QString createFileDialogFilter() const;
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QStringList suffixes() const = 0;
    virtual bool matchesFileName(const QString &fileName) const = 0;
    virtual QSyntaxHighlighter *createHighlighter() const = 0;
    virtual QList<BracketPair> brackets() const = 0;
};

#endif // BABSTRACTFILETYPE_H
