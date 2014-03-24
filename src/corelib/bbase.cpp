#include "bbase.h"
#include "bbase_p.h"

/*============================================================================
================================ BBasePrivate ================================
============================================================================*/

/*============================== Public constructors =======================*/

BBasePrivate::BBasePrivate(BBase *q) :
    _m_q(q)
{
    //
}

BBasePrivate::~BBasePrivate()
{
    //
}

/*============================== Public methods ============================*/

void BBasePrivate::init()
{
    //
}

/*============================================================================
================================ BBase =======================================
============================================================================*/

/*!
    \class BBase
    \brief The BBase class is the base class of all BeQt classes implementing
    D-pointer approach.
*/

/*============================== Public constructors =======================*/

/*!
    Constructs an object and the associated data object.
*/

BBase::BBase() :
    _m_d(new BBasePrivate(this))
{
    _m_d->init();
}

/*!
    Destroys the object, deleting the associated data object.
*/

BBase::~BBase()
{
    delete _m_d;
}

/*============================== Protected constructors ====================*/

/*!
    \overload BBase()

    Constructs an object and associates the given data object \a d with it.
 */

BBase::BBase(BBasePrivate &d) :
    _m_d(&d)
{
    _m_d->init();
}
