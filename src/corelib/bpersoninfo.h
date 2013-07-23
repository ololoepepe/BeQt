#ifndef BPERSONINFO_H
#define BPERSONINFO_H

#include <QString>
#include <QList>

struct BPersonInfo
{
    QString name;
    QString role;
    QString site;
    QString mail;
    QString image;
};

typedef QList<BPersonInfo> BPersonInfoList;

#endif // BPERSONINFO_H
