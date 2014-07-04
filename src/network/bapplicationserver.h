/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtNetwork module of the BeQt library.
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

#ifndef BAPPLICATIONSERVER_H
#define BAPPLICATIONSERVER_H

class BApplicationServerPrivate;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBaseObject>
#include <BeQtCore/BeQt>

#include <QString>
#include <QStringList>

/*============================================================================
================================ BApplicationServer ==========================
============================================================================*/

class B_NETWORK_EXPORT BApplicationServer : public BBaseObject
{
    B_DECLARE_PRIVATE(BApplicationServer)
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    explicit BApplicationServer(quint16 port, int operationTimeout = 5 * BeQt::Second);
#else
    explicit BApplicationServer(const QString &serverName, int operationTimeout = 5 * BeQt::Second);
#endif
    explicit BApplicationServer(quint16 port, const QString &serverName, int operationTimeout = 5 * BeQt::Second);
    ~BApplicationServer();
protected:
    explicit BApplicationServer(BApplicationServerPrivate &d);
public:
    bool isValid() const;
    bool testServer() const;
    bool listen();
    bool sendMessage(int &argc, char **argv);
    bool sendMessage(const QStringList &arguments = QStringList());
protected:
    virtual void handleMessage(const QStringList &arguments);
private:
    Q_DISABLE_COPY(BApplicationServer)
};

#endif // BAPPLICATIONSERVER_H
