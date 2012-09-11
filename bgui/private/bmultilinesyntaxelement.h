#ifndef BMULTILINESYNTAXELEMENT_H
#define BMULTILINESYNTAXELEMENT_H

#include "bsyntaxelement.h"

#include <QDomElement>
#include <QString>
#include <QChar>

class BMultilineSyntaxElement : public BSyntaxElement
{
public:
    explicit BMultilineSyntaxElement(const QDomElement &root);
    //
    bool isValid() const;
    bool isMultiline() const;
    const QString &opening() const;
    const QString &closing() const;
private:
    QString _m_opening;
    QString _m_closing;
};

#endif // BMULTILINESYNTAXELEMENT_H
