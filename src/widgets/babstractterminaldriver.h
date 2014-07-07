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

#ifndef BABSTRACTTERMINALDRIVER_H
#define BABSTRACTTERMINALDRIVER_H

class QString;
class QStringList;
class QVariant;

#include <BeQtCore/BeQtGlobal>

#include <QObject>

/*============================================================================
================================ BAbstractTerminalDriver =====================
============================================================================*/

class B_WIDGETS_EXPORT BAbstractTerminalDriver : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractTerminalDriver(QObject *parent = 0);
    ~BAbstractTerminalDriver();
public:
    virtual void close() = 0;
    virtual bool isActive() const = 0;
    virtual void kill();
    virtual bool processCommand(const QString &command, const QStringList &arguments, QString &error);
    virtual QString prompt() const;
    virtual QString read() = 0;
    virtual void setWorkingDirectory(const QString &path);
    virtual bool terminalCommand(const QString &command, const QStringList &arguments, QString &error);
    virtual bool terminalCommand(const QVariant &data, QString &error);
    virtual void terminate();
    virtual QString workingDirectory() const;
protected Q_SLOTS:
    void emitBlockTerminal();
    void emitFinished(int exitCode);
    void emitReadyRead();
    void emitUnblockTerminal();
Q_SIGNALS:
    void blockTerminal();
    void finished(int exitCode);
    void readyRead();
    void unblockTerminal();
private:
    Q_DISABLE_COPY(BAbstractTerminalDriver)
};

#endif // BABSTRACTTERMINALDRIVER_H
