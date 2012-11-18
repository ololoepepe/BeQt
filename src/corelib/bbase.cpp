#include "bbase.h"
#include "bbase_p.h"

/*============================================================================
================================ Base Private Object
============================================================================*/

BBasePrivateObject::BBasePrivateObject(BBasePrivate *p) :
    _m_p(p)
{
    //
}

BBasePrivateObject::~BBasePrivateObject()
{
    //
}

/*============================================================================
================================ Base Private
============================================================================*/

BBasePrivate::BBasePrivate(BBase *q) :
    _m_q(q), _m_o( new BBasePrivateObject(this) )
{
    //
}

BBasePrivate::~BBasePrivate()
{
    if (_m_o)
        _m_o->deleteLater();
}

//

BBasePrivate::BBasePrivate(BBase &q, BBasePrivateObject &o) :
    _m_q(&q), _m_o(&o)
{
    //
}

/*============================================================================
================================ Base
============================================================================*/

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
