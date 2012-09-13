#include "babstractfiletype.h"

#include "bsyntax.h"

#include <QObject>
#include <QString>
#include <QStringList>

BAbstractFileType::BAbstractFileType(QObject *parent) :
    QObject(parent)
{
}

BAbstractFileType::BAbstractFileType(const BSyntax &syntax, QObject *parent) :
    QObject(parent)
{
    setSyntax(syntax);
}

BAbstractFileType::BAbstractFileType(const QString &syntaxFileName, QObject *parent) :
    QObject(parent)
{
    loadSyntax(syntaxFileName);
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

const QString &BAbstractFileType::syntaxType() const
{
    return _m_syntax.type();
}

bool BAbstractFileType::matchesFileName(const QString &fileName) const
{
    return _m_syntax.matchesFileName(fileName);
}

bool BAbstractFileType::isValid() const
{
    QStringList sl = suffixes();
    return description().length() <= 32 && ( sl.isEmpty() || ( !sl.contains("") && !sl.contains("*") ) );
}

QString BAbstractFileType::fileDialogFilter() const
{
    QString filter;
    if ( !isValid() )
        return filter;
    filter += description() + " (";
    QStringList sl = suffixes();
    if ( !sl.isEmpty() )
    {
        sl.removeDuplicates();
        for (int i = 0; i < sl.size(); ++i)
            filter += "*." + sl.at(i) + (i < sl.size() - 1 ? " " : "");
    }
    else
    {
        filter += "*";
    }
    filter += ")";
    return filter;
}
