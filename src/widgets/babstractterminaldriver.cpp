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

#include "babstractterminaldriver.h"

#include <QObject>
#include <QString>

/*============================================================================
================================ BAbstractTerminalDriver =====================
============================================================================*/

/*============================== Public constructors =======================*/

BAbstractTerminalDriver::BAbstractTerminalDriver(QObject *parent) :
    QObject(parent)
{
    //
}

BAbstractTerminalDriver::~BAbstractTerminalDriver()
{
    //
}

/*============================== Public methods ============================*/

void BAbstractTerminalDriver::kill()
{
    terminate();
}

bool BAbstractTerminalDriver::processCommand(const QString &, const QStringList &, QString &, QTextCodec *)
{
    return true;
}

QString BAbstractTerminalDriver::prompt() const
{
    return "$";
}

void BAbstractTerminalDriver::setWorkingDirectory(const QString &)
{
    //
}

bool BAbstractTerminalDriver::terminalCommand(const QString &, const QStringList &, QString &, QTextCodec *)
{
    emitFinished(0);
    return true;
}

bool BAbstractTerminalDriver::terminalCommand(const QVariant &, QString &, QTextCodec *)
{
    emitFinished(0);
    return true;
}

void BAbstractTerminalDriver::terminate()
{
    close();
}

QString BAbstractTerminalDriver::workingDirectory() const
{
    return "";
}

/*============================== Protected slots ===========================*/

void BAbstractTerminalDriver::emitBlockTerminal()
{
    Q_EMIT blockTerminal();
}

void BAbstractTerminalDriver::emitFinished(int exitCode)
{
    Q_EMIT finished(exitCode);
}

void BAbstractTerminalDriver::emitReadyRead()
{
    Q_EMIT readyRead();
}

void BAbstractTerminalDriver::emitUnblockTerminal()
{
    Q_EMIT unblockTerminal();
}
