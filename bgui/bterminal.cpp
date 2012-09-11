#include "bterminal.h"

#include <QObject>
#include <QProcess>
#include <QPlainTextEdit>
#include <QString>
#include <QStringList>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QScrollBar>
#include <QTextBlock>
#include <QEvent>
#include <QKeyEvent>
#include <QTextStream>

BTerminal::BTerminal(QObject *parent) :
    QObject(parent)
{
    _m_proc = new QProcess(this);
    _m_proc->setProcessChannelMode(QProcess::MergedChannels);
    _m_edit = new QPlainTextEdit;
    _m_edit->setContextMenuPolicy(Qt::NoContextMenu);
    _m_edit->installEventFilter(this);
    _m_started = false;
    _m_len = 0;
    connect( _m_proc, SIGNAL( readyRead() ), this, SLOT( _m_readFromProcess() ) );
    connect( _m_proc, SIGNAL( finished(int) ), this, SLOT( _m_finished(int) ) );
}

//

bool BTerminal::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(object, event);
    QKeyEvent *ke = static_cast<QKeyEvent *>(event);
    int modifiers = ke->modifiers();
    int key = ke->key();
    QTextCursor tc = _m_edit->textCursor();
    QTextBlock tb = tc.block();
    if (key >= Qt::Key_Left && key <= Qt::Key_Down)
        return false;
    if (Qt::NoModifier == modifiers || Qt::ShiftModifier == modifiers)
    {
        if ( tb.next().isValid() )
            return true;
        if (Qt::Key_Backspace == key && tc.positionInBlock() < _m_len)
            return true;
        if ( Qt::NoModifier == modifiers && (Qt::Key_Enter == key || Qt::Key_Return == key) )
            _m_command( tb.text().right(tb.length() - _m_len) );
        return false;
    }
    else if (Qt::ControlModifier == modifiers)
    {
        if (Qt::Key_X == key || Qt::Key_V == key)
            return true;
        if (Qt::Key_Z == key)
        {
#ifdef Q_OS_WIN
            appendText("^Z");
            _m_endOfFile();
#endif
            return true; 
        }
#ifdef Q_OS_UNIX
        if (Qt::Key_D == key)
        {
            appendText("^D");
            _m_endOfFile();
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

bool BTerminal::start(const QString &program, const QStringList &arguments, const QString &workingDir, bool timeout)
{
    if ( _m_started || program.isEmpty() )
        return false;
    _m_prog = program;
    _m_args = arguments;
    _m_wdir = workingDir;
    if ( !_m_wdir.isEmpty() )
        _m_proc->setWorkingDirectory(_m_wdir);
    _m_proc->start(_m_prog, _m_args);
    return _m_proc->waitForStarted(timeout);
}

bool BTerminal::waitForFinished(int timeout)
{
    if (!_m_started)
        return false;
    return _m_proc->waitForFinished(timeout);
}

void BTerminal::terminate()
{
    _m_proc->terminate();
}

void BTerminal::kill()
{
    _m_proc->kill();
}

void BTerminal::appendText(const QString &text, const QTextCharFormat &format)
{
    QTextCursor tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text, format);
    tc.setCharFormat( QTextCharFormat() );
    _m_scrollDown();
    _m_len =  _m_edit->textCursor().block().length();
}

void BTerminal::appendLine(const QString &text, const QTextCharFormat &format)
{
    QTextCursor tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text, format);
    tc.setCharFormat( QTextCharFormat() );
    tc.insertBlock();
    _m_scrollDown();
    _m_len =  _m_edit->textCursor().block().length();
}

bool BTerminal::isStarted() const
{
    return _m_started;
}

const QString &BTerminal::program() const
{
    return _m_prog;
}

const QStringList &BTerminal::arguments() const
{
    return _m_args;
}

const QString &BTerminal::workingDir() const
{
    return _m_wdir;
}

QWidget *BTerminal::widget() const
{
    return _m_edit;
}

//

void BTerminal::clearEdit()
{
    _m_edit->clear();
}

//

void BTerminal::_m_scrollDown()
{
    QScrollBar *sb = _m_edit->verticalScrollBar();
    if (sb)
        sb->setValue( sb->maximum() );
    QTextCursor tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::End);
    _m_edit->setTextCursor(tc);
}

void BTerminal::_m_command(const QString &command)
{
    QTextStream out(_m_proc);
    out.setCodec("UTF-8");
    out << (command + "\n");
}

void BTerminal::_m_endOfFile()
{
    _m_proc->closeWriteChannel();
}

//

void BTerminal::_m_readFromProcess()
{
    QTextStream in(_m_proc);
    in.setCodec("UTF-8");
    QTextCursor tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText( in.readAll() );
    tc = _m_edit->textCursor();
    tc.movePosition(QTextCursor::End);
    _m_len =  tc.block().length();
    _m_scrollDown();
}

void BTerminal::_m_finished(int exitCode)
{
    _m_readFromProcess();
    _m_proc->close();
    _m_started = false;
    emit finished(exitCode);
}
