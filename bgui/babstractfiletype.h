#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

#include "bsyntax.h"

#include <QObject>
#include <QString>
#include <QStringList>

class BAbstractFileType : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractFileType(QObject *parent = 0);
    explicit BAbstractFileType(const BSyntax &syntax, QObject *parent = 0);
    explicit BAbstractFileType(const QString &syntaxFileName, QObject *parent = 0);
    //
    void setSyntax(const BSyntax &syntax);
    bool loadSyntax(const QString &fileName);
    const BSyntax &syntax() const;
    bool matchesFileName(const QString &fileName) const;
    bool isValid() const;
    QString fileDialogFilter() const;
    virtual QString description() const = 0;
    virtual QStringList suffixes() const = 0;
    virtual BAbstractFileType *clone() const = 0;
private:
    BSyntax _m_syntax;
};

#endif // BABSTRACTFILETYPE_H
