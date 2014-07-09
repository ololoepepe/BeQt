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

#ifndef BTERMINALWIDGET_H
#define BTERMINALWIDGET_H

class BTerminalWidgetPrivate;

class BAbstractTerminalDriver;

class QStringList;
class QTextCodec;
class QVariant;

#include <BeQtCore/BBaseObject>

#include <QString>
#include <QTextCharFormat>
#include <QWidget>

/*============================================================================
================================ BTerminalWidget =============================
============================================================================*/

class B_WIDGETS_EXPORT BTerminalWidget : public QWidget, public BBaseObject
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalWidget)
public:
    enum StandardFormat
    {
        //This order reflects the importance of each level of messages
        MessageFormat = 1,
        WarningFormat,
        CriticalFormat
    };
    enum TerminalMode
    {
        NormalMode = 1,
        ProgrammaticMode
    };
public:
    explicit BTerminalWidget(TerminalMode mode, QWidget *parent = 0);
    explicit BTerminalWidget(TerminalMode mode, BAbstractTerminalDriver *driver, QWidget *parent = 0);
    ~BTerminalWidget();
protected:
    explicit BTerminalWidget(BTerminalWidgetPrivate &d, QWidget *parent = 0);
public:
    QTextCodec *codec() const;
    QString codecName() const;
    BAbstractTerminalDriver *driver() const;
    QStringList history() const;
    bool historyEnabled() const;
    bool isActive() const;
    bool isValid() const;
    TerminalMode mode() const;
    QStringList processHistory() const;
    bool processHistoryEnabled() const;
    void setCodec(QTextCodec *codec);
    void setCodec(const QString &codecName);
    void setDriver(BAbstractTerminalDriver *driver);
    void setHistory(const QStringList &list);
    void setHistoryEnabled(bool enabled);
    void setProcessHistory(const QStringList &list);
    void setProcessHistoryEnabled(bool enabled);
    void setTerminatingSequence(int key, int modifiers, const QString &displayedSymbols = "^D");
    void setWorkingDirectory(const QString &path);
    QString workingDirectory() const;
public Q_SLOTS:
    void appendLine(const QString &text, StandardFormat format);
    void appendLine(const QString &text = QString(), const QTextCharFormat &format = QTextCharFormat());
    void appendText(const QString &text, const QTextCharFormat &format = QTextCharFormat());
    void appendText(const QString &text, StandardFormat format);
    void clearEdit();
    void close();
    void emulateUserInput(const QString &command);
    void kill();
    void processCommand(const QString &command);
    void processCommand(const QString &command, const QStringList &arguments);
    void terminalCommand(const QString &command);
    void terminalCommand(const QString &command, const QStringList &arguments);
    void terminalCommand(const QVariant &data);
    void terminate();
Q_SIGNALS:
    void finished(int exitCode);
private:
    Q_DISABLE_COPY(BTerminalWidget)
};

#endif // BTERMINALWIDGET_H
