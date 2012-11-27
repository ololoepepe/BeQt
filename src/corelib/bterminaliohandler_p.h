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

/*============================================================================
================================ Terminal IO Handler Thread
============================================================================*/

class BTerminalIOHandlerThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(BTerminalIOHandlerThread)
public:
    explicit BTerminalIOHandlerThread(BTerminalIOHandlerPrivate *p);
    ~BTerminalIOHandlerThread();
protected:
    void run();
public:
    static QMutex readMutex;
    //
    BTerminalIOHandlerPrivate *const _m_p;
    //
    QTextStream readStream;
};

/*============================================================================
================================ Terminal IO Handler Private
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandlerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminalIOHandler)
    B_DECLARE_PUBLIC_S(BTerminalIOHandler)
    Q_OBJECT
    Q_DISABLE_COPY(BTerminalIOHandlerPrivate)
public:
    explicit BTerminalIOHandlerPrivate(BTerminalIOHandler *q);
    ~BTerminalIOHandlerPrivate();
    //
    static bool testInit(const char *where = 0);
    static bool testUnique();
    //
    void init();
    void lineRead(const QString &text);
    //
    static QMutex echoMutex;
    static QMutex readMutex;
    static QMutex writeMutex;
    static QTextStream writeStream;
    //
    BTerminalIOHandlerThread *const Thread;
    //
    QMutex loopMutex;
    QEventLoop readEventLoop;
    QString lastLine;
private:
    friend class BTerminalIOHandlerThread;
};

#endif // BTERMINALIOHANDLER_P_H
