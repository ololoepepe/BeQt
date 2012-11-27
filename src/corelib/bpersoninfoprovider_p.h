#ifndef BPERSONINFOPROVIDER_P_H
#define BPERSONINFOPROVIDER_P_H

class QString;

#include "bpersoninfoprovider.h"

#include "bglobal.h"
#include "bbase_p.h"

#include <QObject>
#include <QList>
#include <QMap>

/*============================================================================
================================ BPersonInfoProviderPrivate ==================
============================================================================*/

class B_CORE_EXPORT BPersonInfoProviderPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BPersonInfoProvider)
public:
    typedef QMap<QString, BPersonInfoProvider::PersonInfo> PersonInfoMap;
public:
    explicit BPersonInfoProviderPrivate(BPersonInfoProvider *q);
    ~BPersonInfoProviderPrivate();
public:
    static BPersonInfoProvider::PersonInfo infoForLocale(const PersonInfoMap &map, const QString &localeName);
    static void tryAppendInfo(QList<PersonInfoMap> &where, PersonInfoMap what);
public:
    void init();
    void setFileName(const QString &fn);
private:
    QString fileName;
    QList<PersonInfoMap> infos;
private:
    Q_DISABLE_COPY(BPersonInfoProviderPrivate)
};

#endif // BPERSONINFOPROVIDER_P_H

