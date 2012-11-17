#ifndef BCODEEDIT_P_H
#define BCODEEDIT_P_H

class BCodeEditPrivate;

#include "bcodeedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

class BCodeEditPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BCodeEditPrivateObject(BCodeEditPrivate *p);
    ~BCodeEditPrivateObject();
    //
    BCodeEditPrivate *const _m_p;
public slots:
    //
private:
    Q_DISABLE_COPY(BCodeEditPrivateObject)
};

class BCodeEditPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BCodeEdit)
    Q_DECLARE_TR_FUNCTIONS(BCodeEdit)
public:
    explicit BCodeEditPrivate(BCodeEdit *q);
    ~BCodeEditPrivate();
    //
    BCodeEditPrivateObject *const _m_o;
    //
private:
    Q_DISABLE_COPY(BCodeEditPrivate)
    //
    friend class BCodeEditPrivateObject;
};

#endif // BCODEEDIT_P_H

