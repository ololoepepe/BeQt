#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

#include "bsyntax.h"

#include <QString>
#include <QStringList>

class BAbstractFileType
{
public:
    explicit BAbstractFileType( const BSyntax &syntax = BSyntax() );
    //
    void setSyntax(const BSyntax &syntax);
    bool loadSyntax(const QString &fileName);
    BSyntax &syntax() const;
    bool matchesFileName(const QString &fileName) const;
    bool isValid() const;
    virtual QString description() const = 0;
    virtual QStringList suffixes() const = 0;
private:
    BSyntax _m_syntax;
};

#endif // BABSTRACTFILETYPE_H
