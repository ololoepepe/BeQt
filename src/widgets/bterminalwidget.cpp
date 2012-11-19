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
================================ Terminal Widget Private
============================================================================*/

BTerminalWidgetPrivate::BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode) :
    BBasePrivate(q), NormalMode(nmode)
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
        ptedt->installEventFilter(this);
        ptedt->setContextMenuPolicy(Qt::NoContextMenu);
      vlt->addWidget(ptedt);
}

BTerminalWidgetPrivate::~BTerminalWidgetPrivate()
{
    //
}

//

bool BTerminalWidgetPrivate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(object, event);
    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    return handleKeyPress( ke->key(), ke->modifiers() );
}

void BTerminalWidgetPrivate::setDriver(BAbstractTerminalDriver *drv)
{
    if (driver)
    {
        if ( driver->isActive() )
            return;
        disconnect( driver, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
        disconnect( driver, SIGNAL( finished(int) ), this, SLOT( finished(int) ) );
        disconnect( driver, SIGNAL( blockTerminal() ), this, SLOT( blockTerminal() ) );
        disconnect( driver, SIGNAL( unblockTerminal() ), this, SLOT( unblockTerminal() ) );
    }
    driver = drv;
    if (!driver)
        return;
    driver->setParent( q_func() );
    connect( driver, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
    connect( driver, SIGNAL( finished(int) ), this, SLOT( finished(int) ) );
    connect( driver, SIGNAL( blockTerminal() ), this, SLOT( blockTerminal() ) );
    connect( driver, SIGNAL( unblockTerminal() ), this, SLOT( unblockTerminal() ) );
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

void BTerminalWidgetPrivate::blockTerminal()
{
    ptedt->setReadOnly(true);
}

void BTerminalWidgetPrivate::unblockTerminal()
{
    ptedt->setReadOnly(false);
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
