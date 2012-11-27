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

class B_CORE_EXPORT BPersonInfoProviderPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BPersonInfoProvider)
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
    void setFileName(const QString &fn);
    //
    QString fileName;
    QList<PersonInfoMap> infos;
};

#endif // BPERSONINFOPROVIDER_P_H

