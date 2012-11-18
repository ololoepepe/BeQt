#ifndef BBASE_P_H
#define BBASE_P_H

class BBase;
class BBasePrivate;

#include "bglobal.h"

#include <QtGlobal>
#include <QObject>
#include <QPointer>

/*============================================================================
================================ Base Private Object
============================================================================*/

class B_CORE_EXPORT BBasePrivateObject : public QObject
{
    B_DECLARE_PRIVATE_O(BBase)
    Q_OBJECT
public:
    explicit BBasePrivateObject(BBasePrivate *p);
    ~BBasePrivateObject();
protected:
    BBasePrivate *const _m_p;
private:
    Q_DISABLE_COPY(BBasePrivateObject)
};

/*============================================================================
================================ Base Private
============================================================================*/

class B_CORE_EXPORT BBasePrivate
{
    B_DECLARE_PUBLIC(BBase)
    B_DECLARE_OBJECT(BBase)
public:
    explicit BBasePrivate(BBase *q);
    virtual ~BBasePrivate();
protected:
    BBasePrivate(BBase &q, BBasePrivateObject &o);
    //
    BBase *const _m_q;
    BBasePrivateObject *const _m_o;
private:
    Q_DISABLE_COPY(BBasePrivate)
};

#endif // BBASE_P_H
