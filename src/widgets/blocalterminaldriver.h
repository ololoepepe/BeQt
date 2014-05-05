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

#ifndef BLOCALTERMINALDRIVER_H
#define BLOCALTERMINALDRIVER_H

class BLocalTerminalDriverPrivate;

class QString;
class QTextCodec;

#include "babstractterminaldriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ BLocalTerminalDriver ========================
============================================================================*/

class B_WIDGETS_EXPORT BLocalTerminalDriver : public BAbstractTerminalDriver, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLocalTerminalDriver)
public:
    explicit BLocalTerminalDriver(QObject *parent = 0);
    ~BLocalTerminalDriver();
protected:
    explicit BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent = 0);
public:
    bool isActive() const;
    QString read();
    void close();
    void terminate();
    void kill();
    QString prompt() const;
    bool terminalCommand(const QString &command, const QStringList &arguments, QString &error);
    bool processCommand(const QString &command, const QStringList &arguments, QString &error);
    void setWorkingDirectory(const QString &path);
    QString workingDirectory() const;
    void setCodec(QTextCodec *codec);
    void setCodec(const QString &codecName);
    QTextCodec *codec() const;
    QString codecName() const;
private:
    Q_DISABLE_COPY(BLocalTerminalDriver)
};

#endif // BLOCALTERMINALDRIVER_H
