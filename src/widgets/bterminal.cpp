#include "bterminal.h"
#include "bterminal_p.h"
#include "babstractterminaldriver.h"

#include <BeQtCore/BeQtGlobal>

#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextBlock>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QMetaObject>

#include <QDebug>

BTerminalPrivateObject::BTerminalPrivateObject(BTerminalPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BTerminalPrivateObject::~BTerminalPrivateObject()
{
    //
}

//

bool BTerminalPrivateObject::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(object, event);
    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    return _m_p->handleKeyPress( ke->key(), ke->modifiers() );
}

//

void BTerminalPrivateObject::readyRead()
{
    _m_p->read();
}

void BTerminalPrivateObject::finished(int exitCode)
{
    _m_p->finished(exitCode);
}

//

BTerminalPrivate::BTerminalPrivate(BTerminal *q) :
    BBasePrivate(q), _m_o( new BTerminalPrivateObject(this) )
{
    driver = 0;
    len = 0;
    q->setContextMenuPolicy(Qt::NoContextMenu);
    q->installEventFilter(_m_o);
}

BTerminalPrivate::~BTerminalPrivate()
{
    if (driver)
        driver->deleteLater();
    _m_o->deleteLater();
}

//

void BTerminalPrivate::setDriver(BAbstractTerminalDriver *drv)
{
    if (driver)
    {
        if ( driver->isActive() )
            return;
        QObject::disconnect( driver, SIGNAL( readyRead() ), _m_o, SLOT( readyRead() ) );
        QObject::disconnect( driver, SIGNAL( finished(int) ), _m_o, SLOT( finished(int) ) );
    }
    driver = drv;
    if (!driver)
        return;
    QObject::connect( driver, SIGNAL( readyRead() ), _m_o, SLOT( readyRead() ) );
    QObject::connect( driver, SIGNAL( finished(int) ), _m_o, SLOT( finished(int) ) );
    appendText( driver->prompt() );
}

bool BTerminalPrivate::handleKeyPress(int key, int modifiers)
{
    B_Q(BTerminal);
    QTextCursor tc = q->textCursor();
    QTextBlock tb = tc.block();
    if (key >= Qt::Key_Left && key <= Qt::Key_Down)
        return false;
    if (Qt::NoModifier == modifiers || Qt::ShiftModifier == modifiers)
    {
        if ( tb.next().isValid() )
            return true;
        if (Qt::Key_Backspace == key && driver && !driver->isActive() && tc.positionInBlock() < len)
            return true;
        if ( Qt::NoModifier == modifiers && (Qt::Key_Enter == key || Qt::Key_Return == key) )
        {
            if ( !driver->applyCommand( tb.text().right(tb.length() - len) ) )
            {
                appendLine();
                appendText( driver->prompt() );
                return true;
            }
        }
        return false;
    }
    else if (Qt::ControlModifier == modifiers)
    {
        if (Qt::Key_X == key || Qt::Key_V == key)
            return true;
        if (Qt::Key_Z == key)
        {
#ifdef B_OS_WIN
            appendText("^Z");
            driver->close();
#endif
            return true;
        }
#ifdef B_OS_UNIX
        if (Qt::Key_D == key)
        {
            appendText("^D");
            driver->close();
            return true;
        }
#endif
    }
    else if ( (Qt::ControlModifier | Qt::ShiftModifier) == modifiers )
    {
        return true;
    }
    return false;
}

void BTerminalPrivate::scrollDown()
{
    B_Q(BTerminal);
    QScrollBar *sb = q->verticalScrollBar();
    if (sb)
        sb->setValue( sb->maximum() );
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    q->setTextCursor(tc);
}

void BTerminalPrivate::read()
{
    B_Q(BTerminal);
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText( driver->read() );
    tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    len = tc.block().length();
    scrollDown();
}

void BTerminalPrivate::finished(int exitCode)
{
    read();
    QMetaObject::invokeMethod( q_func(), "finished", Q_ARG(int, exitCode) );
    appendText( driver->prompt() );
}

void BTerminalPrivate::appendText(const QString &text)
{
    B_Q(BTerminal);
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text);
    //tc.setCharFormat( QTextCharFormat() );
    scrollDown();
    len = q->textCursor().block().length();
}

void BTerminalPrivate::appendLine(const QString &text)
{
    B_Q(BTerminal);
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text);
    //tc.setCharFormat( QTextCharFormat() );
    tc.insertBlock();
    scrollDown();
    len = q->textCursor().block().length();
}

//

BTerminal::BTerminal(QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BTerminalPrivate(this) )
{
    //
}

BTerminal::BTerminal(BAbstractTerminalDriver *driver, QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BTerminalPrivate(this) )
{
    setDriver(driver);
}

BTerminal::~BTerminal()
{
    //
}

//

void BTerminal::setDriver(BAbstractTerminalDriver *driver)
{
    d_func()->setDriver(driver);
}

BAbstractTerminalDriver *BTerminal::driver() const
{
    return d_func()->driver;
}

bool BTerminal::isValid() const
{
    return d_func()->driver;
}

bool BTerminal::isActive() const
{
    const B_D(BTerminal);
    return d->driver && d->driver->isActive();
}

//

void BTerminal::close()
{
    if ( !isActive() )
        return;
    d_func()->driver->close();
}

void BTerminal::terminate()
{
    if ( !isActive() )
        return;
    d_func()->driver->terminate();
}

//

BTerminal::BTerminal(BTerminalPrivate &d, QWidget *parent) :
    QPlainTextEdit(parent), BBase(d)
{
    //
}
