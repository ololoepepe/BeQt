#include "bplaintextedit.h"
#include "bplaintextedit_p.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QPlainTextEdit>
#include <QWidget>
#include <QMimeData>
#include <QTextCursor>
#include <QTextDocument>

#include <QDebug>

BPlainTextEditPrivateObject::BPlainTextEditPrivateObject(BPlainTextEditPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BPlainTextEditPrivateObject::~BPlainTextEditPrivateObject()
{
    //
}

//

void BPlainTextEditPrivateObject::cursorPositionChanged(const QTextCursor &cursor)
{
    _m_p->cursorPositionChanged(cursor);
}

void BPlainTextEditPrivateObject::contentsChange(int position, int charsRemoved, int charsAdded)
{
    _m_p->contentsChange(position, charsRemoved, charsAdded);
}

//

BPlainTextEditPrivate::BPlainTextEditPrivate(BPlainTextEdit *q) :
    BBasePrivate(q), _m_o( new BPlainTextEditPrivateObject(this) )
{
    drag = true;
    QObject::connect( q->document(), SIGNAL( cursorPositionChanged(QTextCursor) ),
                      _m_o, SLOT( cursorPositionChanged(QTextCursor) ) );
    QObject::connect( q->document(), SIGNAL( contentsChange(int, int, int) ),
                      _m_o, SLOT( contentsChange(int, int, int) ) );
}

BPlainTextEditPrivate::~BPlainTextEditPrivate()
{
    _m_o->deleteLater();
}

//

void BPlainTextEditPrivate::cursorPositionChanged(const QTextCursor &cursor)
{
    //
}

void BPlainTextEditPrivate::contentsChange(int position, int charsRemoved, int charsAdded)
{
    //
}

//

BPlainTextEdit::BPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BPlainTextEditPrivate(this) )
{
    //
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
    //
}

//

bool BPlainTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QPlainTextEdit::canInsertFromMimeData(source);
}

QMimeData *BPlainTextEdit::createMimeDataFromSelection() const
{
    return d_func()->drag ? QPlainTextEdit::createMimeDataFromSelection() : 0;
}

void BPlainTextEdit::insertFromMimeData(const QMimeData *source)
{
    QPlainTextEdit::insertFromMimeData(source);
}
