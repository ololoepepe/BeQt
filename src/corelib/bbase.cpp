#include "bbase.h"

#include "bbase_p.h"

BBasePrivate::BBasePrivate(BBase *q) :
    _m_q(q)
{
    //
}

BBasePrivate::~BBasePrivate()
{
    //
}

//

BBase::BBase() :
    _m_d( new BBasePrivate(this) )
{
    //
}

BBase::~BBase()
{
    delete _m_d;
}

//

BBase::BBase(BBasePrivate &d) :
    _m_d(&d)
{
    //
}
