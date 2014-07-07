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

#include "blocalterminaldriver.h"

#include <BeQtCore/BBase>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QApplication>
#include <QDir>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>

#include <QDebug>

/*============================================================================
================================ BLocalTerminalDriverPrivate =================
============================================================================*/

class BLocalTerminalDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocalTerminalDriver)
    Q_DECLARE_TR_FUNCTIONS(BLocalTerminalDriver)
public:
    QTextCodec *codec;
    QProcess *process;
    QString workingDirectory;
public:
    explicit BLocalTerminalDriverPrivate(BLocalTerminalDriver *q);
    ~BLocalTerminalDriverPrivate();
public:
    bool handleBuiltinCommand(const QString &command, const QStringList &args, QString &err, bool &ok);
    bool handleCd(const QStringList &args, QString &err);
    void init();
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivate)
};

/*============================================================================
================================ BLocalTerminalDriverPrivate =================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalTerminalDriverPrivate::BLocalTerminalDriverPrivate(BLocalTerminalDriver *q) :
    BBasePrivate(q)
{
    //
}

BLocalTerminalDriverPrivate::~BLocalTerminalDriverPrivate()
{
    //
}

/*============================== Public methods ============================*/

bool BLocalTerminalDriverPrivate::handleBuiltinCommand(const QString &command, const QStringList &args, QString &err,
                                                       bool &ok)
{
    typedef bool (BLocalTerminalDriverPrivate::*HandleFunction)(const QStringList &, QString &);
    typedef QMap<QString, HandleFunction> HandleFunctionMap;
    init_once(HandleFunctionMap, funcs, HandleFunctionMap()) {
        funcs.insert("cd", &BLocalTerminalDriverPrivate::handleCd);
    }
#if defined(Q_OS_WINDOWS)
    QString cmd = command.toLower();
#else
    QString cmd = command;
#endif
    if (!funcs.contains(cmd))
        return false;
    ok = (this->*funcs.value(cmd))(args, err);
    return true;
}

bool BLocalTerminalDriverPrivate::handleCd(const QStringList &args, QString &err)
{
    if (args.size() > 1) {
        err = tr("Too many arguments", "terminalCommand return");
        return false;
    }
    if (args.isEmpty() || args.first().isEmpty()) {
        workingDirectory = QDir::homePath();
        return true;
    }
    if (!QDir(args.first()).exists()) {
        err = tr("No such directory", "terminalCommand return");
        return false;
    }
    workingDirectory = args.first();
    return true;
}

void BLocalTerminalDriverPrivate::init()
{
    B_Q(BLocalTerminalDriver);
    process = new QProcess(q);
    workingDirectory = QDir::homePath();
    process->setProcessChannelMode(QProcess::MergedChannels);
    codec = QTextCodec::codecForLocale();
    QObject::connect(process, SIGNAL(finished(int)), q, SLOT(emitFinished(int)));
    QObject::connect(process, SIGNAL(readyRead()), q, SLOT(emitReadyRead()));
}

/*============================================================================
================================ BLocalTerminalDriver ========================
============================================================================*/

/*============================== Public constructors =======================*/

BLocalTerminalDriver::BLocalTerminalDriver(QObject *parent) :
    BAbstractTerminalDriver(parent), BBase(*new BLocalTerminalDriverPrivate(this))
{
    d_func()->init();
}

BLocalTerminalDriver::~BLocalTerminalDriver()
{
    //
}

/*============================== Protected constructors ====================*/

BLocalTerminalDriver::BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent) :
    BAbstractTerminalDriver(parent), BBase(d)
{
    d_func()->init();
}

/*============================== Public methods ============================*/

void BLocalTerminalDriver::close()
{
    d_func()->process->close();
}

QTextCodec *BLocalTerminalDriver::codec() const
{
    return d_func()->codec;
}

QString BLocalTerminalDriver::codecName() const
{
    return QString::fromLatin1(d_func()->codec->name());
}

bool BLocalTerminalDriver::isActive() const
{
    return d_func()->process->isOpen();
}

void BLocalTerminalDriver::kill()
{
    d_func()->process->kill();
}

QString BLocalTerminalDriver::read()
{
    QTextStream in(d_func()->process);
    if (d_func()->codec)
        in.setCodec(d_func()->codec);
    return in.readAll();
}

bool BLocalTerminalDriver::processCommand(const QString &command, const QStringList &arguments, QString &error)
{
    if (!isActive()) {
        error = tr("No process is running", "processCommand return");
        return false;
    }
    QTextStream out(d_func()->process);
    if (d_func()->codec)
        out.setCodec(d_func()->codec);
    QString cmd = command;
    foreach (const QString &arg, arguments)
        cmd += " " + (arg.contains(" ") ? ("\"" + arg + "\"") : arg);
    cmd += "\n";
    out << cmd;
    return true;
}

QString BLocalTerminalDriver::prompt() const
{
    return d_func()->workingDirectory + "$";
}

void BLocalTerminalDriver::setCodec(QTextCodec *codec)
{
    d_func()->codec = codec;
}

void BLocalTerminalDriver::setCodec(const QString &codecName)
{
    d_func()->codec = QTextCodec::codecForName(codecName.toLatin1());
}

void BLocalTerminalDriver::setWorkingDirectory(const QString &path)
{
    if (isActive())
        return;
    d_func()->workingDirectory = !path.isEmpty() ? path : QDir::homePath();
}

bool BLocalTerminalDriver::terminalCommand(const QString &command, const QStringList &arguments, QString &error)
{
    if (isActive()) {
        error = tr("Another process is running", "terminalCommand return");
        return false;
    }
    B_D(BLocalTerminalDriver);
    bool ok = false;
    if (d->handleBuiltinCommand(command, arguments, error, ok)) {
        emitFinished(0);
        return ok;
    }
    d->process->setWorkingDirectory(d->workingDirectory);
    BeQt::startProcess(d->process, command, arguments);
    if (!d->process->waitForStarted()) {
        d->process->close();
        error = tr("Could not find or start programm", "terminalCommand return");
        return false;
    }
    return true;
}

void BLocalTerminalDriver::terminate()
{
    d_func()->process->terminate();
}

QString BLocalTerminalDriver::workingDirectory() const
{
    return d_func()->workingDirectory;
}
