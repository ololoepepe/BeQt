#ifndef BSYNTAXELEMENT_H
#define BSYNTAXELEMENT_H

#include <QString>
#include <QRegExp>
#include <QDomElement>
#include <QTextCharFormat>
#include <QChar>

class BSyntaxElement
{
public:
    static bool equals(const QString &sa, const QString &s2);
    static QRegExp::PatternSyntax syntax(const QDomElement &element);
    static int weight(const QDomElement &element);
    //
    explicit BSyntaxElement(const QDomElement &root);
    //
    const QString &pattern() const;
    Qt::CaseSensitivity caseSensitivity() const;
    QRegExp::PatternSyntax patternSyntax() const;
    const QChar &preventing() const;
    const QTextCharFormat &format() const;
    virtual bool isValid() const;
    virtual bool isMultiline() const;
private:
    QString _m_pattern;
    Qt::CaseSensitivity _m_cs;
    QRegExp::PatternSyntax _m_patternSyntax;
    QChar _m_preventing;
    QTextCharFormat _m_format;
};

#endif // BSYNTAXELEMENT_H
