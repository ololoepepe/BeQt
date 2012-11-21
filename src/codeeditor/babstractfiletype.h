#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

class QStringList;
class QSyntaxHighlighter;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>

#include <QString>
#include <QList>
#include <QApplication>

/*============================================================================
================================ Abstract File Type
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileType
{
public:
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
    virtual QList<BCodeEdit::BracketPair> brackets() const = 0;
};

#endif // BABSTRACTFILETYPE_H
