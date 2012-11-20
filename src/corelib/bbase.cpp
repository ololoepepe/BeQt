#include "bbase.h"
#include "bbase_p.h"

/*============================================================================
================================ Base Private
============================================================================*/

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

void BBasePrivate::init()
{
    //
}

/*============================================================================
================================ Base
============================================================================*/

BBase::BBase() :
    _m_d( new BBasePrivate(this) )
{
    _m_d->init();
}

BBase::~BBase()
{
    delete _m_d;
}

//

BBase::BBase(BBasePrivate &d) :
    _m_d(&d)
{
    _m_d->init();
}
