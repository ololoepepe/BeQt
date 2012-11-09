#include "bterminalwidget.h"
#include "bterminalwidget_p.h"
#include "babstractterminaldriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BTerminalIOHandler>

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
#include <QDir>
#include <QDragMoveEvent>
#include <QStringList>

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

BTerminalWidgetPrivate::BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode) :
    BBasePrivate(q), _m_o( new BTerminalWidgetPrivateObject(this) ), NormalMode(nmode)
{
    driver = 0;
    terminatingKey = Qt::Key_D;
    terminatingModifiers = Qt::ControlModifier;
    terminatingSymbols = "^D";
    len = 0;
    q->setContextMenuPolicy(Qt::NoContextMenu);
    q->installEventFilter(_m_o);
    Qt::TextInteractionFlags tif = q->textInteractionFlags() &= ~(Qt::TextSelectableByMouse |
                                                                  Qt::TextSelectableByKeyboard);
    q->setTextInteractionFlags(tif);
}

BTerminalWidgetPrivate::~BTerminalWidgetPrivate()
{
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
    B_Q(BTerminalWidget);
    driver->setParent(q);
    QObject::connect( driver, SIGNAL( readyRead() ), _m_o, SLOT( readyRead() ) );
    QObject::connect( driver, SIGNAL( finished(int) ), _m_o, SLOT( finished(int) ) );
    if (NormalMode)
    {
        QTextCursor tc = q->textCursor();
        scrollDown();
        tc.movePosition(QTextCursor::End);
        q->setTextCursor(tc);
        if ( tc.block().text().isEmpty() )
            appendText( driver->prompt() );
        else
            appendLine( driver->prompt() );
    }
}

bool BTerminalWidgetPrivate::handleKeyPress(int key, int modifiers)
{
    B_Q(BTerminalWidget);
    QTextCursor tc = q->textCursor();
    QTextBlock tb = tc.block();
    if (key >= Qt::Key_Left && key <= Qt::Key_Down) //TODO: Implement history using arrow keys
        return false;
    if (key == terminatingKey && modifiers == terminatingModifiers)
    {
        appendText( !terminatingSymbols.isEmpty() ? terminatingSymbols : QString("^D") );
        appendLine();
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
            if (!driver)
                return false;
            QString command = tb.text().right(tb.length() - len);
            if ( driver->isActive() )
                q->processCommand(command);
            else if (NormalMode)
                q->terminalCommand(command);
            return true;
        }
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
}

void BTerminalWidgetPrivate::read()
{
    B_Q(BTerminalWidget);
    scrollDown();
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText( driver->read() );
    q->setTextCursor(tc);
    len = tc.block().length();
}

void BTerminalWidgetPrivate::finished(int exitCode)
{
    read();
    driver->close();
    QMetaObject::invokeMethod( q_func(), "finished", Q_ARG(int, exitCode) );
    if (NormalMode)
    {
        B_Q(BTerminalWidget);
        QTextCursor tc = q->textCursor();
        scrollDown();
        tc.movePosition(QTextCursor::End);
        q->setTextCursor(tc);
        if ( tc.block().text().isEmpty() )
            appendText( driver->prompt() );
        else
            appendLine( driver->prompt() );
    }
}

void BTerminalWidgetPrivate::appendText(const QString &text)
{
    B_Q(BTerminalWidget);
    scrollDown();
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text);
    q->setTextCursor(tc);
    len = q->textCursor().block().length();
}

void BTerminalWidgetPrivate::appendLine(const QString &text)
{
    B_Q(BTerminalWidget);
    scrollDown();
    QTextCursor tc = q->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertBlock();
    tc.insertText(text);
    len = q->textCursor().block().length();
}

//

BTerminalWidget::BTerminalWidget(TerminalMode mode, QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BTerminalWidgetPrivate(this, NormalMode == mode) )
{
    //
}

BTerminalWidget::BTerminalWidget(TerminalMode mode, BAbstractTerminalDriver *driver, QWidget *parent) :
    QPlainTextEdit(parent), BBase( *new BTerminalWidgetPrivate(this, NormalMode == mode) )
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

void BTerminalWidget::setWorkingDirectory(const QString &path)
{
    B_D(BTerminalWidget);
    if (d->NormalMode || !d->driver)
        return;
    d->driver->setWorkingDirectory(path);
}

BTerminalWidget::TerminalMode BTerminalWidget::mode() const
{
    return d_func()->NormalMode ? NormalMode : ProgrammaticMode;
}

BAbstractTerminalDriver *BTerminalWidget::driver() const
{
    return d_func()->driver;
}

QString BTerminalWidget::workingDirectory() const
{
    const B_D(BTerminalWidget);
    return d->driver ? d->driver->workingDirectory() : QString("");
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

void BTerminalWidget::terminalCommand(const QString &command)
{
    QStringList args = BTerminalIOHandler::splitCommand(command);
    QString cmd = !args.isEmpty() ? args.takeFirst() : QString();
    terminalCommand(cmd, args);
}

void BTerminalWidget::terminalCommand(const QString &command, const QStringList &arguments)
{
    B_D(BTerminalWidget);
    if ( !d->NormalMode || !d->driver || d->driver->isActive() )
        return;
    if ( command.isEmpty() )
        return d->appendLine( d->driver->prompt() );
    QString ret = d->driver->terminalCommand(command, arguments);
    if ( !ret.isEmpty() )
    {
        d->appendLine(tr("Error:", "text") + " " + ret);
        d->appendLine( d->driver->prompt() );
    }
    else
    {
        d->appendLine();
    }
}

void BTerminalWidget::processCommand(const QString &command)
{
    QStringList args = BTerminalIOHandler::splitCommand(command);
    QString cmd = !args.isEmpty() ? args.takeFirst() : QString();
    processCommand(cmd, args);
}

void BTerminalWidget::processCommand(const QString &command, const QStringList &arguments)
{
    B_D(BTerminalWidget);
    if ( !d->driver || !d->driver->isActive() )
        return;
    if ( command.isEmpty() )
        return d->appendLine();
    QString ret = d->driver->processCommand(command, arguments);
    if ( !ret.isEmpty() )
        d->appendLine(tr("Error:", "text") + " " + ret);
    d->appendLine();
}

void BTerminalWidget::emulateUserInput(const QString &command)
{
    B_D(BTerminalWidget);
    d->scrollDown();
    QTextCursor tc = textCursor();
    tc.insertText(command);
    QKeyEvent e(QKeyEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QApplication::sendEvent(this, &e);
}

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

void BTerminalWidget::kill()
{
    if ( !isActive() )
        return;
    d_func()->driver->kill();
}

//

BTerminalWidget::BTerminalWidget(BTerminalWidgetPrivate &d, QWidget *parent) :
    QPlainTextEdit(parent), BBase(d)
{
    //
}
