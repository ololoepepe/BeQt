#include "bsyntaxelement.h"

#include <QString>
#include <QRegExp>
#include <QDomElement>
#include <QFont>
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>
#include <QChar>

bool BSyntaxElement::equals(const QString &s1, const QString &s2)
{
    return !QString::compare(s1, s2, Qt::CaseInsensitive);
}

QRegExp::PatternSyntax BSyntaxElement::syntax(const QDomElement &element)
{
    if ( element.isNull() )
        return QRegExp::RegExp;
    QString text = element.attribute("syntax");
    if ( text.isEmpty() )
        return QRegExp::RegExp;
    if ( BSyntaxElement::equals(text, "RegExp") )
        return QRegExp::RegExp;
    else if ( BSyntaxElement::equals(text, "RegExp2") )
        return QRegExp::RegExp2;
    else if ( BSyntaxElement::equals(text, "Wildcard") )
        return QRegExp::Wildcard;
    else if ( BSyntaxElement::equals(text, "WildcardUnix") )
        return QRegExp::WildcardUnix;
    else if ( BSyntaxElement::equals(text, "FixedString") )
        return QRegExp::FixedString;
    else if ( BSyntaxElement::equals(text, "W3CXmlSchema11") )
        return QRegExp::W3CXmlSchema11;
    else
        return QRegExp::RegExp;
}

int BSyntaxElement::weight(const QDomElement &element)
{
    if ( element.isNull() )
        return QFont::Normal;
    QString text = element.attribute("weight");
    if ( text.isEmpty() )
        return QFont::Normal;
    if ( BSyntaxElement::equals(text, "Light") )
        return QFont::Light;
    else if ( BSyntaxElement::equals(text, "Normal") )
        return QFont::Normal;
    else if ( BSyntaxElement::equals(text, "DemiBold") )
        return QFont::DemiBold;
    else if ( BSyntaxElement::equals(text, "Bold") )
        return QFont::Bold;
    else if ( BSyntaxElement::equals(text, "Black") )
        return QFont::Black;
    else
        return QFont::Normal;
}

//

BSyntaxElement::BSyntaxElement(const QDomElement &root)
{
    _m_cs = Qt::CaseInsensitive;
    _m_patternSyntax = QRegExp::RegExp;
    if ( root.isNull() )
        return;
    //pattern
    QDomElement el = root.firstChildElement("pattern");
    if ( el.isNull() )
        return;
    _m_pattern = el.text();
    if ( _m_pattern.isEmpty() )
        return;
    //sensitive
    if ( equals(el.attribute("sensitive"), "true") )
        _m_cs = Qt::CaseSensitive;
    //pattern_syntax
    _m_patternSyntax = syntax(el);
    //preventing
    el = root.firstChildElement("preventing");
    if ( !el.isNull() )
    {
        QString text = el.text();
        if (text.size() == 1)
            _m_preventing = text.at(0);
    }
    //format
    el = root.firstChildElement("format");
    if ( !el.isNull() )
    {
        _m_format.setForeground( QBrush( QColor( el.text() ) ) );
        _m_format.setFontWeight( weight(el) );
    }
}

//

const QString &BSyntaxElement::pattern() const
{
    return _m_pattern;
}

Qt::CaseSensitivity BSyntaxElement::caseSensitivity() const
{
    return _m_cs;
}

QRegExp::PatternSyntax BSyntaxElement::patternSyntax() const
{
    return _m_patternSyntax;
}


const QChar &BSyntaxElement::preventing() const
{
    return _m_preventing;
}

const QTextCharFormat &BSyntaxElement::format() const
{
    return _m_format;
}

bool BSyntaxElement::isValid() const
{
    return !_m_pattern.isEmpty() && _m_format.isValid();
}

bool BSyntaxElement::isMultiline() const
{
    return false;
}
