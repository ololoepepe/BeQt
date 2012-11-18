#ifndef BBASE_H
#define BBASE_H

class BBasePrivate;

#include "bglobal.h"

#include <QtGlobal>

/*============================================================================
================================ Base
============================================================================*/

class B_CORE_EXPORT BBase
{
    B_DECLARE_PRIVATE(BBase)
public:
    BBase();
    virtual ~BBase();
protected:
    BBase(BBasePrivate &d);
    //
    BBasePrivate *const _m_d;
private:
    Q_DISABLE_COPY(BBase)
};

#endif // BBASE_H
