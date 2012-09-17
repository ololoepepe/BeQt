#ifndef BABSTRACTFILETYPE_H
#define BABSTRACTFILETYPE_H

#include "bsyntax.h"

#include <QObject>
#include <QString>
#include <QStringList>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BAbstractFileType : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractFileType(QObject *parent = 0);
    //
    void setSyntax(const BSyntax &syntax);
    void loadSyntax(const QString &fileName);
    const BSyntax &syntax() const;
    const QString &syntaxType() const;
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
