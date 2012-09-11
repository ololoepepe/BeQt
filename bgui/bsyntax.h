#ifndef BSYNTAX_H
#define BSYNTAX_H

class BSyntaxElement;

#include <QChar>
#include <QTextBlockUserData>
#include <QVector>
#include <QTextCharFormat>
#include <QString>
#include <QList>
#include <QTextCursor>
#include <QColor>
#include <QDomElement>
#include <QTextBlock>
#include <QMap>
#include <QRegExp>
#include <QSharedPointer>
#include <QStringList>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BSyntaxData : public QSharedData
{
public:
    QString type;
    QRegExp pattern;
    QList<QChar> opBr;
    QList<QChar> clBr;
    QChar bracketsEscape;
    bool bracketsSkipHighlighted;
    QList<BSyntaxElement *> elements;
    //
    BSyntaxData();
    ~BSyntaxData();
};

class BGUISHARED_EXPORT BSyntax
{
public:
    struct BracketInfo
    {
        QChar bracket;
        int position;
    };
    struct BlockData : public QTextBlockUserData
    {
        QVector<BracketInfo> brackets;
    };
    struct BlockProcessingResult
    {
        BlockData *blockData;
        int currentState;
        QVector<QTextCharFormat> formats;
    };
    struct CursorProcessingResult
    {
        QMap<int, bool> positions;
    };
    //
    static const QColor BracketsColor;
    static const QColor BracketsErrorColor;
    //
    static void removeSpaces(QString &text);
    //
    BSyntax();
    explicit BSyntax(const QString &fileName);
    BSyntax(const BSyntax &other);
    ~BSyntax();
    //
    void loadSyntax(const QString &fileName);
    bool isValid() const;
    bool matchesFileName(const QString &fileName) const;
    const QString &type() const;
    BlockProcessingResult processBlock(QString text, int previousState) const;
    CursorProcessingResult processCursor(const QTextCursor &cursor) const;
private:
    QSharedPointer<BSyntaxData> _m_d;
    //
    void _m_loadType(const QDomElement &root);
    void _m_loadPattern(const QDomElement &root);
    void _m_loadBrackets(const QDomElement &root);
    void _m_loadElements(const QDomElement &root);
    QVector<BracketInfo> _m_brackets(const QString &text) const;
    bool _m_testBracket(const QString &text, int pos) const;
    bool _m_testBracket(const QString &text, int pos, bool opening) const;
    BracketInfo _m_findBracketPair(const QTextBlock &block, int pos, bool opening) const;
    int _m_indexOf(const QString &text, QRegExp &rx, const QChar &prev, const QString &pattern, int from = 0) const;
};

#endif // BSYNTAX_H
