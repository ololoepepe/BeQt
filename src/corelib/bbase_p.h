#ifndef BBASE_P_H
#define BBASE_P_H

class BBase;
class BBasePrivate;

#include "bglobal.h"

#include <QtGlobal>
#include <QObject>

/*============================================================================
================================ BBasePrivate ================================
============================================================================*/

class B_CORE_EXPORT BBasePrivate : public QObject
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BBase)
public:
    explicit BBasePrivate(BBase *q);
    virtual ~BBasePrivate();
public:
    void init();
protected:
    BBase *const _m_q;
private:
    Q_DISABLE_COPY(BBasePrivate)
};

#endif // BBASE_P_H
