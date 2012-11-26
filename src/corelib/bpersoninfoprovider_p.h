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
================================ Person Info Provider Private
============================================================================*/

class BPersonInfoProviderPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPersonInfoProvider)
    B_DECLARE_TR_FUNCTIONS(BPersonInfoProvider, q)
    Q_OBJECT
    Q_DISABLE_COPY(BPersonInfoProviderPrivate)
public:
    typedef QMap<QString, BPersonInfoProvider::PersonInfo> PersonInfoMap;
    //
    static BPersonInfoProvider::PersonInfo infoForLocale(const PersonInfoMap &map, const QString &localeName);
    static void tryAppendInfo(QList<PersonInfoMap> &where, PersonInfoMap what);
    //
    explicit BPersonInfoProviderPrivate(BPersonInfoProvider *q);
    ~BPersonInfoProviderPrivate();
    //
    void init();
    //
    QString fileName;
    QList<PersonInfoMap> infos;
};

#endif // BPERSONINFOPROVIDER_P_H

