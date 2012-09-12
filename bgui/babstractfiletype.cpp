#include "babstractfiletype.h"

#include "bsyntax.h"

#include <QString>
#include <QStringList>

BAbstractFileType::BAbstractFileType(const BSyntax &syntax)
{
    _m_syntax = syntax;
}

//

void BAbstractFileType::setSyntax(const BSyntax &syntax)
{
    _m_syntax = syntax;
}

const BSyntax &BAbstractFileType::syntax() const
{
    return _m_syntax;
}

bool BAbstractFileType::matchesFileName(const QString &fileName) const
{
    return _m_syntax.matchesFileName(fileName);
}

bool BAbstractFileType::isValid() const
{
    return description().length() <= 32;
}
