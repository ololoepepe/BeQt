/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtWidgets module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "bterminalwidget.h"
#include "bterminalwidget_p.h"

#include "babstractterminaldriver.h"
#include "bapplication.h"
#include "bplaintextedit.h"

#include <BeQtCore/BTextTools>

#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QClipboard>
#include <QColor>
#include <QDebug>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMetaObject>
#include <QMimeData>
#include <QObject>
#include <QPoint>
#include <QScrollBar>
#include <QStringList>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QWidget>

/*============================================================================
================================ BTerminalWidgetPrivate ======================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalWidgetPrivate::BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode) :
    BBaseObjectPrivate(q), NormalMode(nmode)
{
    //
}

BTerminalWidgetPrivate::~BTerminalWidgetPrivate()
{
    delete mnu;
    if (driver) {
        driver->terminate();
        if (!driver->parent() || (driver->parent() == q_func()))
            delete driver;
    }
}

/*============================== Static public methods =====================*/

QTextCharFormat BTerminalWidgetPrivate::createStandardFormat(BTerminalWidget::StandardFormat format)
{
    QTextCharFormat fmt;
    switch (format) {
    case BTerminalWidget::MessageFormat:
        fmt.setForeground(QBrush(QColor("blue")));
        break;
    case BTerminalWidget::WarningFormat:
        fmt.setForeground(QBrush(QColor("darkorange")));
        break;
    case BTerminalWidget::CriticalFormat:
        fmt.setForeground(QBrush(QColor("red")));
        break;
    default:
        break;
    }
    return fmt;
}

/*============================== Public methods ============================*/

void BTerminalWidgetPrivate::appendLine(const QString &text, const QTextCharFormat &format)
{
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertBlock(tc.blockFormat(), format);
    tc.insertText(text, format);
    lastLineLength = ptedt->textCursor().block().length() - 1;
    scrollDown();
}

void BTerminalWidgetPrivate::appendText(const QString &text, const QTextCharFormat &format)
{
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(text, format);
    lastLineLength = ptedt->textCursor().block().length() - 1;
    scrollDown();
}

QString BTerminalWidgetPrivate::constructErrorString(const QString &error) const
{
    return tr("Error:", "text") + " " + (!error.isEmpty() ? error : tr("Unknown error", "text"));
}

bool BTerminalWidgetPrivate::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type())
    {
    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::Drop:
        e->ignore();
        return true;
    case QEvent::DragLeave:
        e->accept();
        return true;
    case QEvent::KeyPress:
        return handleKeyPress(static_cast<QKeyEvent *>(e));
    default:
        return QObject::eventFilter(o, e);
    }
}

bool BTerminalWidgetPrivate::handleKeyPress(QKeyEvent *e)
{
    int key = e->key();
    Qt::KeyboardModifiers modifiers = e->modifiers();
    if (key == terminatingKey && int(modifiers) == terminatingModifiers) {
        appendText(!terminatingSymbols.isEmpty() ? terminatingSymbols : QString("^D"));
        if (!driver || !driver->isActive())
            return true;
        driver->close();
        return true;
    }
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = tc.block();
    if (tb.next().isValid() || tc.positionInBlock() < lastLineLength
            || (tc.hasSelection() && tc.selectionStart() < tb.position() + lastLineLength)) {
        switch (key)
        {
        case Qt::Key_Down:
            handleUpDown(false);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        case Qt::Key_Up:
            handleUpDown(true);
            return true;
        default:
            return true;
        }
    } else {
        switch (key)
        {
        case Qt::Key_Backspace:
            return (tc.positionInBlock() <= lastLineLength);
        case Qt::Key_Down:
            handleUpDown(false);
            return true;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            handleReturn();
            return true;
        case Qt::Key_Up:
            handleUpDown(true);
            return true;
        default:
            return false;
        }
    }
}

void BTerminalWidgetPrivate::handleReturn()
{
    if (!driver)
        return;
    QTextBlock tb = ptedt->document()->lastBlock();
    QString command = tb.text().right(tb.length() - lastLineLength - 1);
    if (driver->isActive()) {
        currentProcessHistory = -1;
        if (processHistoryEnabled && !command.isEmpty())
            processHistory.prepend(command);
        q_func()->processCommand(command);
    }
    else if (NormalMode) {
        processHistory.clear();
        currentHistory = -1;
        currentProcessHistory = -1;
        if (historyEnabled && !command.isEmpty())
            history.prepend(command);
        q_func()->terminalCommand(command);
    }
}

void BTerminalWidgetPrivate::handleUpDown(bool up)
{
    if (!driver)
        return;
    int k = up ? 1 : -1;
    QTextCursor tc = ptedt->textCursor();
    if (driver->isActive() && processHistoryEnabled) {
        int current = currentProcessHistory + k;
        if ((current >= -1) && (current < processHistory.size())) {
            currentProcessHistory = current;
            tc.movePosition(QTextCursor::End);
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText((current >= 0) ? processHistory.at(currentProcessHistory) : QString());
        }
    } else if (NormalMode && historyEnabled) {
        int current = currentHistory + k;
        if ((current >= -1) && (current < history.size())) {
            currentHistory = current;
            tc.movePosition(QTextCursor::End);
            tc.movePosition(QTextCursor::StartOfBlock);
            tc.setPosition(tc.position() + driver->prompt().length() + 1);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            tc.insertText((current >= 0) ? history.at(currentHistory) : QString());
        }
    }
}

void BTerminalWidgetPrivate::init()
{
    B_Q(BTerminalWidget);
    driver = 0;
    terminatingKey = Qt::Key_D;
    terminatingModifiers = Qt::ControlModifier;
    terminatingSymbols = "^D";
    lastLineLength = 0;
    historyEnabled = true;
    processHistoryEnabled = true;
    currentHistory = -1;
    currentProcessHistory = -1;
    codec = QTextCodec::codecForLocale();
    //
    QVBoxLayout *vlt = new QVBoxLayout(q);
      vlt->setContentsMargins(0, 0, 0, 0);
      ptedt = new BPlainTextEdit(q);
        ptedt->setDragEnabled(false);
        ptedt->setAcceptDrops(false);
        ptedt->installEventFilter(this);
        ptedt->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ptedt, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
      vlt->addWidget(ptedt);
    //
    mnu = new QMenu;
    actCopy = mnu->addAction("", this, SLOT(actCopyTriggered()));
    actPaste = mnu->addAction("", this, SLOT(actPasteTriggered()));
    //
    retranslateUi();
    connect(bApp, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

bool BTerminalWidgetPrivate::pasteAvailable() const
{
    const QMimeData *md = QApplication::clipboard()->mimeData();
    if (!md || !md->hasText())
        return false;
    QTextCursor tc = ptedt->textCursor();
    QTextBlock tb = ptedt->document()->lastBlock();
    int pos = tb.position() + lastLineLength;
    return (tc.position() >= pos) && (!tc.hasSelection() || (tc.selectionStart() >= pos && tc.selectionEnd() >= pos));
}

void BTerminalWidgetPrivate::scrollDown()
{
    QScrollBar *sb = ptedt->verticalScrollBar();
    if (sb)
        sb->setValue(sb->maximum());
}

void BTerminalWidgetPrivate::setDriver(BAbstractTerminalDriver *drv)
{
    if (driver) {
        if (driver->isActive())
            return;
        disconnect(driver, SIGNAL(readyRead()), this, SLOT(read()));
        disconnect(driver, SIGNAL(finished(int)), this, SLOT(finished(int)));
        disconnect(driver, SIGNAL(blockTerminal()), this, SLOT(blockTerminal()));
        disconnect(driver, SIGNAL(unblockTerminal()), this, SLOT(unblockTerminal()));
        if (!driver->parent() || (driver->parent() == q_func()))
            delete driver;
    }
    driver = drv;
    if (!driver)
        return;
    if (!driver->parent())
        driver->setParent(q_func());
    connect(driver, SIGNAL(readyRead()), this, SLOT(read()));
    connect(driver, SIGNAL(finished(int)), this, SLOT(finished(int)));
    connect(driver, SIGNAL(blockTerminal()), this, SLOT(blockTerminal()));
    connect(driver, SIGNAL(unblockTerminal()), this, SLOT(unblockTerminal()));
    if (NormalMode) {
        QTextCursor tc = ptedt->textCursor();
        scrollDown();
        tc.movePosition(QTextCursor::End);
        ptedt->setTextCursor(tc);
        if (tc.block().text().isEmpty())
            appendText(driver->prompt() + " ");
        else
            appendLine(driver->prompt() + " ");
    }
}

/*============================== Public slots ==============================*/

void BTerminalWidgetPrivate::actCopyTriggered()
{
    QApplication::clipboard()->setText(ptedt->textCursor().selectedText());
}

void BTerminalWidgetPrivate::actPasteTriggered()
{
    if (!pasteAvailable())
        return;
    QString text = BTextTools::withoutUnsuppottedSymbols(QApplication::clipboard()->text());
    ptedt->textCursor().insertText(text.split(QRegExp("\\n+")).first());
}

void BTerminalWidgetPrivate::blockTerminal()
{
    ptedt->setReadOnly(true);
}

void BTerminalWidgetPrivate::customContextMenuRequested(const QPoint &pos)
{
    actCopy->setEnabled(ptedt->textCursor().hasSelection());
    actPaste->setEnabled(pasteAvailable());
    mnu->exec(ptedt->mapToGlobal(pos));
}

void BTerminalWidgetPrivate::finished(int exitCode)
{
    read();
    driver->close();
    QMetaObject::invokeMethod(q_func(), "finished", Q_ARG(int, exitCode));
    if (NormalMode) {
        QTextCursor tc = ptedt->textCursor();
        scrollDown();
        tc.movePosition(QTextCursor::End);
        ptedt->setTextCursor(tc);
        if (tc.block().text().isEmpty())
            appendText(driver->prompt() + " ");
        else
            appendLine(driver->prompt() + " ");
    }
}

void BTerminalWidgetPrivate::read()
{
    scrollDown();
    QTextCursor tc = ptedt->textCursor();
    tc.movePosition(QTextCursor::End);
    tc.insertText(driver->read(codec));
    ptedt->setTextCursor(tc);
    lastLineLength = tc.block().length() - 1;
}

void BTerminalWidgetPrivate::retranslateUi()
{
    actCopy->setText(tr("Copy", "act text"));
    actPaste->setText(tr("Paste", "act text"));
}

void BTerminalWidgetPrivate::unblockTerminal()
{
    ptedt->setReadOnly(false);
}

/*============================================================================
================================ BTerminalWidget =============================
============================================================================*/

/*============================== Public constructors =======================*/

BTerminalWidget::BTerminalWidget(TerminalMode mode, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BTerminalWidgetPrivate(this, NormalMode == mode))
{
    d_func()->init();
}

BTerminalWidget::BTerminalWidget(TerminalMode mode, BAbstractTerminalDriver *driver, QWidget *parent) :
    QWidget(parent), BBaseObject(*new BTerminalWidgetPrivate(this, NormalMode == mode))
{
    d_func()->init();
    setDriver(driver);
}

BTerminalWidget::~BTerminalWidget()
{
    //
}

/*============================== Protected constructors ====================*/

BTerminalWidget::BTerminalWidget(BTerminalWidgetPrivate &d, QWidget *parent) :
    QWidget(parent), BBaseObject(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

QTextCodec *BTerminalWidget::codec() const
{
    return d_func()->codec;
}

QString BTerminalWidget::codecName() const
{
    return QString::fromLatin1(d_func()->codec->name());
}

BAbstractTerminalDriver *BTerminalWidget::driver() const
{
    return d_func()->driver;
}

QStringList BTerminalWidget::history() const
{
    return d_func()->history;
}

bool BTerminalWidget::historyEnabled() const
{
    return d_func()->historyEnabled;
}

bool BTerminalWidget::isActive() const
{
    const B_D(BTerminalWidget);
    return d->driver && d->driver->isActive();
}

bool BTerminalWidget::isValid() const
{
    return d_func()->driver;
}

BTerminalWidget::TerminalMode BTerminalWidget::mode() const
{
    return d_func()->NormalMode ? NormalMode : ProgrammaticMode;
}

QStringList BTerminalWidget::processHistory() const
{
    return d_func()->processHistory;
}

bool BTerminalWidget::processHistoryEnabled() const
{
    return d_func()->processHistoryEnabled;
}

void BTerminalWidget::setCodec(QTextCodec *codec)
{
    d_func()->codec = codec ? codec : QTextCodec::codecForLocale();
}

void BTerminalWidget::setCodec(const QString &codecName)
{
    setCodec(QTextCodec::codecForName(codecName.toLatin1()));
}

void BTerminalWidget::setDriver(BAbstractTerminalDriver *driver)
{
    d_func()->setDriver(driver);
}

void BTerminalWidget::setHistory(const QStringList &list)
{
    d_func()->history = list;
}

void BTerminalWidget::setHistoryEnabled(bool enabled)
{
    d_func()->historyEnabled = enabled;
}

void BTerminalWidget::setProcessHistory(const QStringList &list)
{
    if (!isActive())
        return;
    d_func()->processHistory = list;
}

void BTerminalWidget::setProcessHistoryEnabled(bool enabled)
{
    d_func()->processHistoryEnabled = enabled;
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

QString BTerminalWidget::workingDirectory() const
{
    const B_D(BTerminalWidget);
    return d->driver ? d->driver->workingDirectory() : QString("");
}

/*============================== Public slots ==============================*/

void BTerminalWidget::appendLine(const QString &text, const QTextCharFormat &format)
{
    d_func()->appendLine(text, format);
}

void BTerminalWidget::appendLine(const QString &text, StandardFormat format)
{
    appendLine(text, BTerminalWidgetPrivate::createStandardFormat(format));
}

void BTerminalWidget::appendText(const QString &text, const QTextCharFormat &format)
{
    d_func()->appendText(text, format);
}

void BTerminalWidget::appendText(const QString &text, StandardFormat format)
{
    appendText(text, BTerminalWidgetPrivate::createStandardFormat(format));
}

void BTerminalWidget::clearEdit()
{
    B_D(BTerminalWidget);
    d->ptedt->clear();
    if (d->NormalMode)
        d->appendText(d->driver->prompt() + " ");
    else
        d->lastLineLength = 0;
}

void BTerminalWidget::close()
{
    if (!isActive())
        return;
    d_func()->driver->close();
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

void BTerminalWidget::kill()
{
    if (!isActive())
        return;
    d_func()->driver->kill();
}

void BTerminalWidget::processCommand(const QString &command)
{
    QStringList args = BTextTools::splitCommand(command);
    QString cmd = !args.isEmpty() ? args.takeFirst() : QString();
    processCommand(cmd, args);
}

void BTerminalWidget::processCommand(const QString &command, const QStringList &arguments)
{
    B_D(BTerminalWidget);
    if (!d->driver || !d->driver->isActive())
        return;
    d->appendLine();
    if (command.isEmpty())
        return ;
    QString error;
    if (!d->driver->processCommand(command, arguments, error, d_func()->codec))
        d->appendText(d->constructErrorString(error));

}

void BTerminalWidget::terminalCommand(const QString &command)
{
    QStringList args = BTextTools::splitCommand(command);
    QString cmd = !args.isEmpty() ? args.takeFirst() : QString();
    terminalCommand(cmd, args);
}

void BTerminalWidget::terminalCommand(const QString &command, const QStringList &arguments)
{
    B_D(BTerminalWidget);
    if (!d->driver || d->driver->isActive())
        return;
    if (command.isEmpty()) {
        if (d->NormalMode)
            d->appendLine(d->driver->prompt() + " ");
        return;
    }
    d->appendLine();
    QString error;
    if (!d->driver->terminalCommand(command, arguments, error, d_func()->codec)) {
        d->appendText(d->constructErrorString(error));
        if (d->NormalMode)
            d->appendLine(d->driver->prompt() + " ");
        return;
    }
}

void BTerminalWidget::terminalCommand(const QVariant &data)
{
    B_D(BTerminalWidget);
    if (!d->driver || d->driver->isActive() || !data.isValid())
        return;
    d->appendLine();
    QString error;
    if (!d->driver->terminalCommand(data, error, d_func()->codec)) {
        d->appendText(d->constructErrorString(error));
        if (d->NormalMode)
            d->appendLine(d->driver->prompt() + " ");
        return;
    }
}

void BTerminalWidget::terminate()
{
    if (!isActive())
        return;
    d_func()->driver->terminate();
}
