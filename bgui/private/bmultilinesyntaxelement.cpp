#include "bmultilinesyntaxelement.h"
#include "bsyntaxelement.h"

#include <QString>
#include <QDomElement>
#include <QChar>

BMultilineSyntaxElement::BMultilineSyntaxElement(const QDomElement &root) :
    BSyntaxElement(root)
{
    if ( !BSyntaxElement::isValid() || root.isNull() ||
         !equals(root.attribute("multiline"), "true") )
        return;
    //opening
    QDomElement el = root.firstChildElement("opening");
    if ( !el.isNull() )
        _m_opening = el.text();
    //closing
    el = root.firstChildElement("closing");
    if ( !el.isNull() )
        _m_closing = el.text();
}

//

bool BMultilineSyntaxElement::isValid() const
{
    return BSyntaxElement::isValid();
}

bool BMultilineSyntaxElement::isMultiline() const
{
    return true;
}

const QString &BMultilineSyntaxElement::opening() const
{
    return _m_opening;
}

const QString &BMultilineSyntaxElement::closing() const
{
    return _m_closing;
}
