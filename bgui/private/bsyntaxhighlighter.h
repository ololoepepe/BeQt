#ifndef BSYNTAXHIGHLIGHTER_H
#define BSYNTAXHIGHLIGHTER_H

class QTextDocument;

#include "../bsyntax.h"

#include <QSyntaxHighlighter>
#include <QList>
#include <QString>

class BSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit BSyntaxHighlighter(QTextDocument *parent = 0);
    //
    void setSyntax(const BSyntax &syntax);
protected:
    void highlightBlock(const QString &text);
private:
    BSyntax _m_syntax;
};

#endif // BSYNTAXHIGHLIGHTER_H
