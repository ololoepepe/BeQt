#ifndef BBASE_H
#define BBASE_H

class BBasePrivate;

#include "bglobal.h"

#include <QtGlobal>

/*============================================================================
================================ BBase =======================================
============================================================================*/

class B_CORE_EXPORT BBase
{
    B_DECLARE_PRIVATE(BBase)
public:
    explicit BBase();
    virtual ~BBase();
protected:
    explicit BBase(BBasePrivate &d);
protected:
    BBasePrivate *const _m_d;
private:
    Q_DISABLE_COPY(BBase)
};

#endif // BBASE_H
