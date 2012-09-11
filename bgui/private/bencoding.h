#ifndef BENCODING_H
#define BENCODING_H

#include <QString>
#include <QList>
#include <QMap>

class BEncoding
{
public:
    BEncoding();
    //
    virtual QString codecName() const = 0;
    virtual QString descriptiveName() const;
    QString fullName() const;
};

#endif // BENCODING_H
