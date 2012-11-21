#include "bplaintextedit.h"
#include "bplaintextedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPlainTextEdit>
#include <QWidget>
#include <QMimeData>

#include <QDebug>

/*============================================================================
================================ Plain Text Edit Private
============================================================================*/

BPlainTextEditPrivate::BPlainTextEditPrivate(BPlainTextEdit *q) :
    BBasePrivate(q)
{
    //
}

BPlainTextEditPrivate::~BPlainTextEditPrivate()
{
    //
}

//

void BPlainTextEditPrivate::init()
{
    drag = true;
}

/*============================================================================
================================ Plain Text Edit
============================================================================*/

BPlainTextEdit::BPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BPlainTextEditPrivate(this) )
{
    d_func()->init();
}

BPlainTextEdit::~BPlainTextEdit()
{
    //
}

//

void BPlainTextEdit::setDragEnabled(bool b)
{
    d_func()->drag = b;
}

bool BPlainTextEdit::dragEnabled() const
{
    return d_func()->drag;
}

//

BPlainTextEdit::BPlainTextEdit(BPlainTextEditPrivate &d, QWidget *parent) :
    QPlainTextEdit(parent), BBase(d)
{
    d_func()->init();
}

//

QMimeData *BPlainTextEdit::createMimeDataFromSelection() const
{
    return d_func()->drag ? QPlainTextEdit::createMimeDataFromSelection() : 0;
}
