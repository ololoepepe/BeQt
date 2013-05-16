#ifndef BTERMINALIOHANDLER_P_H
#define BTERMINALIOHANDLER_P_H

class BTerminalIOHandlerPrivate;

class QString;

#include "bterminaliohandler.h"

#include "bglobal.h"
#include "bbase_p.h"

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTextStream>
#include <QFile>
#include <QEventLoop>
#include <QMap>

/*============================================================================
================================ BTerminalIOHandlerThread ====================
============================================================================*/

class BTerminalIOHandlerThread : public QThread
{
    Q_OBJECT
public:
    explicit BTerminalIOHandlerThread(BTerminalIOHandlerPrivate *p);
    ~BTerminalIOHandlerThread();
protected:
    void run();
public:
    BTerminalIOHandlerPrivate *const _m_p;
public:
    QTextStream readStream;
private:
    Q_DISABLE_COPY(BTerminalIOHandlerThread)
};

/*============================================================================
================================ BTerminalIOHandlerPrivate ===================
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandlerPrivate : public BBasePrivate
{
    Q_OBJECT
    B_DECLARE_PUBLIC(BTerminalIOHandler)
    B_DECLARE_PUBLIC_S(BTerminalIOHandler)
public:
    explicit BTerminalIOHandlerPrivate(BTerminalIOHandler *q);
    ~BTerminalIOHandlerPrivate();
public:
    static bool testInit(const char *where = 0);
    static bool testUnique();
public:
    void init();
    void lineRead(const QString &text);
public slots:
    void executeInternalHandler(const QString &cmd, const QStringList &args);
    void executeExternalHandler(const QString &cmd, const QStringList &args);
    void executeHandleCommand(const QString &cmd, const QStringList &args);
public:
    static QMutex echoMutex;
    static QMutex readMutex;
    static QMutex writeMutex;
    static QMutex writeErrMutex;
    static QTextStream writeStream;
    static QTextStream writeErrStream;
public:
    BTerminalIOHandlerThread *const Thread;
public:
    QMutex loopMutex;
    QEventLoop readEventLoop;
    QString lastLine;
    QMap<QString, BTerminalIOHandler::InternalHandler> internalHandlers;
    QMap<QString, BTerminalIOHandler::ExternalHandler> externalHandlers;
private:
    Q_DISABLE_COPY(BTerminalIOHandlerPrivate)
    friend class BTerminalIOHandlerThread;
};

#endif // BTERMINALIOHANDLER_P_H
