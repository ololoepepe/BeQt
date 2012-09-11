class QTextDocument;

#include "bsyntaxhighlighter.h"
#include "../bsyntax.h"

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextCharFormat>
#include <QTextBlockUserData>

#include <QDebug>

BSyntaxHighlighter::BSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    //
}

//

void BSyntaxHighlighter::setSyntax(const BSyntax &syntax)
{
    _m_syntax = syntax;
}

//

void BSyntaxHighlighter::highlightBlock(const QString &text)
{
    BSyntax::BlockProcessingResult r = _m_syntax.processBlock( text, previousBlockState() );
    for (int i = 0; i < r.formats.size(); ++i)
        setFormat( i, 1, r.formats.at(i) );
    setCurrentBlockState(r.currentState);
    setCurrentBlockUserData(r.blockData);
}
