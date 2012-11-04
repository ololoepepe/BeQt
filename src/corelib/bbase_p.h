#ifndef BBASE_P_H
#define BBASE_P_H

class BBase;

#include "bglobal.h"

#include <QtGlobal>

class B_CORE_EXPORT BBasePrivate
{
    B_DECLARE_PUBLIC(BBase)
public:
    explicit BBasePrivate(BBase *q);
    virtual ~BBasePrivate();
protected:
    BBase *const _m_q;
private:
    Q_DISABLE_COPY(BBasePrivate)
};

#endif // BBASE_P_H
