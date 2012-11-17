#include "bcodeedit.h"
#include "bcodeedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QWidget>

BCodeEditPrivateObject::BCodeEditPrivateObject(BCodeEditPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BCodeEditPrivateObject::~BCodeEditPrivateObject()
{
    //
}

//

BCodeEditPrivate::BCodeEditPrivate(BCodeEdit *q) :
    BBasePrivate(q), _m_o( new BCodeEditPrivateObject(this) )
{
    //
}

BCodeEditPrivate::~BCodeEditPrivate()
{
    _m_o->deleteLater();
}

//

BCodeEdit::BCodeEdit(QWidget *parent) :
    QWidget(parent), BBase( *new BCodeEditPrivate(this) )
{
    //
}

BCodeEdit::~BCodeEdit()
{
    //
}

//

BCodeEdit::BCodeEdit(BCodeEditPrivate &d, QWidget *parent) :
    QWidget(parent), BBase(d)
{
    //
}

