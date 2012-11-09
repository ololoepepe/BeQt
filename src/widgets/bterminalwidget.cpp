#include "bterminalwidget.h"
#include "bterminalwidget_p.h"
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
#include <QApplication>

#include <QDebug>

BTerminalWidgetPrivateObject::BTerminalWidgetPrivateObject(BTerminalWidgetPrivate *p) :
    QObject(0), _m_p(p)
{
    //
}

BTerminalWidgetPrivateObject::~BTerminalWidgetPrivateObject()
{
    //
}

//

bool BTerminalWidgetPrivateObject::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(object, event);
    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    return _m_p->handleKeyPress( ke->key(), ke->modifiers() );
}

//

void BTerminalWidgetPrivateObject::readyRead()
{
    _m_p->read();
}

void BTerminalWidgetPrivateObject::finished(int exitCode)
{
    _m_p->finished(exitCode);
}

//

BTerminalWidgetPrivate::BTerminalWidgetPrivate(BTerminalWidget *q) :
    BBasePrivate(q), _m_o( new BTerminalWidgetPrivateObject(this) )
{
    driver = 0;
    terminatingKey = Qt::Key_D;
    terminatingModifiers = Qt::ControlModifier;
    terminatingSymbols = "^D";
    len = 0;
    q->setContextMenuPolicy(Qt::NoContextMenu);
    q->installEventFilter(_m_o);
}

BTerminalWidgetPrivate::~BTerminalWidgetPrivate()
{
    if (driver)
        driver->deleteLater();
    _m_o->deleteLater();
}

//

void BTerminalWidgetPrivate::setDriver(BAbstractTerminalDriver *drv)
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

bool BTerminalWidgetPrivate::handleKeyPress(int key, int modifiers)
{
    B_Q(BTerminalWidget);
    QTextCursor tc = q->textCursor();
    QTextBlock tb = tc.block();
    if (key >= Qt::Key_Left && key <= Qt::Key_Down)
        return false;
    if (key == terminatingKey && modifiers == terminatingModifiers)
    {
        appendLine( !terminatingSymbols.isEmpty() ? terminatingSymbols : QString("^D") );
        driver->close();
        return true;
    }
    else if (Qt::NoModifier == modifiers || Qt::ShiftModifier == modifiers)
    {
        if ( tb.next().isValid() )
            return true;
        if ( Qt::Key_Backspace == key && ( (driver && !driver->isActive() && tc.positionInBlock() < len) ||
                                           tb.text().isEmpty() ) )
            return true;
        if ( Qt::NoModifier == modifiers && (Qt::Key_Enter == key || Qt::Key_Return == key) )
        {
            if ( !driver->applyCommand( tb.text().right(tb.length() - len) ) )
            {
                driver->close();
                appendLine();
                appendLine( driver->invalidCommandMessage() );
                appendText( driver->prompt() );
                return true;
            }
        }
        return false;
    }
    else if ( (Qt::ControlModifier | Qt::ShiftModifier) == modifiers )
    {
        return true;
    }
    return false;
}

void BTerminalWidgetPrivate::scrollDown()
{
    B_Q(BTerminalWidget);
    QScrollBar *sb = q->verticalScrollBar();
    if (sb)
        sb->setValue( sb->maximum() );
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    q->setTextCursor(tc);
}

void BTerminalWidgetPrivate::read()
{
    B_Q(BTerminalWidget);
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText( driver->read() );
    tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    len = tc.block().length();
    scrollDown();
}

void BTerminalWidgetPrivate::finished(int exitCode)
{
    read();
    driver->close();
    QMetaObject::invokeMethod( q_func(), "finished", Q_ARG(int, exitCode) );
    appendText( driver->prompt() );
}

void BTerminalWidgetPrivate::appendText(const QString &text)
{
    B_Q(BTerminalWidget);
    scrollDown();
    QTextCursor tc = q->textCursor();
    tc.insertText(text);
    len = q->textCursor().block().length();
}

void BTerminalWidgetPrivate::appendLine(const QString &text)
{
    B_Q(BTerminalWidget);
    scrollDown();
    QTextCursor tc = q->textCursor();
    tc.insertText(text);
    tc.insertBlock();
    len = q->textCursor().block().length();
}

//

BTerminalWidget::BTerminalWidget(QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BTerminalWidgetPrivate(this) )
{
    //
}

BTerminalWidget::BTerminalWidget(BAbstractTerminalDriver *driver, QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BTerminalWidgetPrivate(this) )
{
    setDriver(driver);
}

BTerminalWidget::~BTerminalWidget()
{
    //
}

//

void BTerminalWidget::setDriver(BAbstractTerminalDriver *driver)
{
    d_func()->setDriver(driver);
}

void BTerminalWidget::setTerminatingSequence(int key, int modifiers, const QString &displayedSymbols)
{
    B_D(BTerminalWidget);
    d->terminatingKey = key;
    d->terminatingModifiers = modifiers;
    d->terminatingSymbols = displayedSymbols;
}

BAbstractTerminalDriver *BTerminalWidget::driver() const
{
    return d_func()->driver;
}

bool BTerminalWidget::isValid() const
{
    return d_func()->driver;
}

bool BTerminalWidget::isActive() const
{
    const B_D(BTerminalWidget);
    return d->driver && d->driver->isActive();
}

//

void BTerminalWidget::close()
{
    if ( !isActive() )
        return;
    d_func()->driver->close();
}

void BTerminalWidget::terminate()
{
    if ( !isActive() )
        return;
    d_func()->driver->terminate();
}

void BTerminalWidget::emulateCommand(const QString &command)
{
    B_D(BTerminalWidget);
    d->scrollDown();
    QTextCursor tc = textCursor();
    tc.insertText(command);
    QKeyEvent e(QKeyEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(this, &e);
}

//

BTerminalWidget::BTerminalWidget(BTerminalWidgetPrivate &d, QWidget *parent) :
    QPlainTextEdit(parent), BBase(d)
{
    //
}
