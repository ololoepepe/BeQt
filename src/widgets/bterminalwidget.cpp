#include "bterminalwidget.h"
#include "bterminalwidget_p.h"
#include "babstractterminaldriver.h"
#include "bplaintextedit.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BTerminalIOHandler>

#include <QWidget>
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include <QMetaObject>
#include <QApplication>
#include <QDir>
#include <QDragMoveEvent>
#include <QStringList>
#include <QVBoxLayout>

#include <QDebug>

/*============================================================================
================================ Terminal Widget Private Object
============================================================================*/

BTerminalWidgetPrivateObject::BTerminalWidgetPrivateObject(BTerminalWidgetPrivate *p) :
    BBasePrivateObject(p)
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
    return p_func()->handleKeyPress( ke->key(), ke->modifiers() );
}

//

void BTerminalWidgetPrivateObject::readyRead()
{
    p_func()->read();
}

void BTerminalWidgetPrivateObject::finished(int exitCode)
{
    p_func()->finished(exitCode);
}

void BTerminalWidgetPrivateObject::blockTerminal()
{
    p_func()->blockTerminal();
}

void BTerminalWidgetPrivateObject::unblockTerminal()
{
    p_func()->unblockTerminal();
}

/*============================================================================
================================ Terminal Widget Private
============================================================================*/

BTerminalWidgetPrivate::BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode) :
    BBasePrivate( *q, *new BTerminalWidgetPrivateObject(this) ), NormalMode(nmode)
{
    driver = 0;
    terminatingKey = Qt::Key_D;
    terminatingModifiers = Qt::ControlModifier;
    terminatingSymbols = "^D";
    len = 0;
    //
    vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      ptedt = new BPlainTextEdit(q);
        ptedt->setDragEnabled(false);
        ptedt->setAcceptDrops(false);
        ptedt->installEventFilter( o_func() );
        ptedt->setContextMenuPolicy(Qt::NoContextMenu);
      vlt->addWidget(ptedt);
}

BTerminalWidgetPrivate::~BTerminalWidgetPrivate()
{
    //
}

//

void BTerminalWidgetPrivate::setDriver(BAbstractTerminalDriver *drv)
{
    if (driver)
    {
        if ( driver->isActive() )
            return;
        QObject::disconnect( driver, SIGNAL( readyRead() ), o_func(), SLOT( readyRead() ) );
        QObject::disconnect( driver, SIGNAL( finished(int) ), o_func(), SLOT( finished(int) ) );
        QObject::disconnect( driver, SIGNAL( blockTerminal() ), o_func(), SLOT( blockTerminal() ) );
        QObject::disconnect( driver, SIGNAL( unblockTerminal() ), o_func(), SLOT( unblockTerminal() ) );
    }
    driver = drv;
    if (!driver)
        return;
    driver->setParent( q_func() );
    QObject::connect( driver, SIGNAL( readyRead() ), o_func(), SLOT( readyRead() ) );
    QObject::connect( driver, SIGNAL( finished(int) ), o_func(), SLOT( finished(int) ) );
    QObject::connect( driver, SIGNAL( blockTerminal() ), o_func(), SLOT( blockTerminal() ) );
    QObject::connect( driver, SIGNAL( unblockTerminal() ), o_func(), SLOT( unblockTerminal() ) );
    if (NormalMode)
    {
        QTextCursor tc = ptedt->textCursor();
        scrollDown();
        tc.movePosition(QTextCursor::End);
        ptedt->setTextCursor(tc);
        if ( tc.block().text().isEmpty() )
            appendText( driver->prompt() );
        else
            appendLine( driver->prompt() );
    }
}

bool BTerminalWidgetPrivate::handleKeyPress(int key, int modifiers)
{
    B_Q(BTerminalWidget);
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    if (key >= Qt::Key_Left && key <= Qt::Key_Down) //TODO: Implement history using arrow keys
        return false;
    if (key == terminatingKey && modifiers == terminatingModifiers)
    {
        if ( !driver || !driver->isActive() )
            return true;
        appendText( !terminatingSymbols.isEmpty() ? terminatingSymbols : QString("^D") );
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
    else if (modifiers & Qt::ControlModifier)
    {
        return true;
    }
    return false;
}

void BTerminalWidgetPrivate::scrollDown()
{
    QScrollBar *sb = ptedt->verticalScrollBar();
    if (sb)
        sb->setValue( sb->maximum() );
}

void BTerminalWidgetPrivate::appendText(const QString &text)
{
    scrollDown();
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text);
    ptedt->setTextCursor(tc);
    len = ptedt->textCursor().block().length();
}

void BTerminalWidgetPrivate::appendLine(const QString &text)
{
    scrollDown();
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertBlock();
    tc.insertText(text);
    len = ptedt->textCursor().block().length();
}

void BTerminalWidgetPrivate::blockTerminal()
{
    ptedt->setReadOnly(true);
}

void BTerminalWidgetPrivate::unblockTerminal()
{
    ptedt->setReadOnly(false);
}

void BTerminalWidgetPrivate::read()
{
    scrollDown();
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText( driver->read() );
    ptedt->setTextCursor(tc);
    len = tc.block().length();
}

void BTerminalWidgetPrivate::finished(int exitCode)
{
    read();
    driver->close();
    QMetaObject::invokeMethod( q_func(), "finished", Q_ARG(int, exitCode) );
    if (NormalMode)
    {
        QTextCursor tc = ptedt->textCursor();
        scrollDown();
        tc.movePosition(QTextCursor::End);
        ptedt->setTextCursor(tc);
        if ( tc.block().text().isEmpty() )
            appendText( driver->prompt() );
        else
            appendLine( driver->prompt() );
    }
}

//

BTerminalWidgetPrivate::BTerminalWidgetPrivate(BTerminalWidget &q, BTerminalWidgetPrivateObject &o, bool mode) :
    BBasePrivate(q, o), NormalMode(mode)
{
    //
}

/*============================================================================
================================ Terminal Widget
============================================================================*/

BTerminalWidget::BTerminalWidget(TerminalMode mode, QWidget *parent) :
    QWidget(parent), BBase( *new BTerminalWidgetPrivate(this, NormalMode == mode) )
{
    //
}

BTerminalWidget::BTerminalWidget(TerminalMode mode, BAbstractTerminalDriver *driver, QWidget *parent) :
    QWidget(parent), BBase( *new BTerminalWidgetPrivate(this, NormalMode == mode) )
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
    bool fin = false;
    int eCode = 0;
    QString ret = d->driver->terminalCommand(command, arguments, &fin, &eCode);
    if ( !ret.isEmpty() )
    {
        d->appendLine(tr("Error:", "text") + " " + ret);
        d->appendLine( d->driver->prompt() );
    }
    else
    {
        d->appendLine();
        if (fin)
            d->finished(eCode);
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
    QTextCursor tc = d->ptedt->textCursor();
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
    QWidget(parent), BBase(d)
{
    //
}
