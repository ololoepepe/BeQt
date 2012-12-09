#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

class QStringList;
class QSyntaxHighlighter;
class QObject;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>

#include <QString>
#include <QList>
#include <QApplication>

/*============================================================================
================================ BAbstractFileType ===========================
============================================================================*/

class B_CODEEDITOR_EXPORT BAbstractFileType
{
public:
    BAbstractFileType();
    virtual ~BAbstractFileType();
public:
    static BAbstractFileType *defaultFileType();
public:
    QString createFileDialogFilter() const;
    virtual QString id() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QStringList suffixes() const = 0;
    virtual bool matchesFileName(const QString &fileName) const = 0;
    virtual QSyntaxHighlighter *createHighlighter(QObject *parent) const = 0;
    virtual QList<BCodeEdit::BracketPair> brackets() const = 0;
protected:
    static BCodeEdit::BracketPair createBracketPair( const QString &op, const QString &cl,
                                                     const QString &esc = QString() );
};

#endif // BABSTRACTFILETYPE_H
