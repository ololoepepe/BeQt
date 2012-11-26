#ifndef BTERMINALIOHANDLER_P_H
#define BTERMINALIOHANDLER_P_H

class QMutex;
class QString;

#include "bterminaliohandler.h"

#include "bglobal.h"
#include "bbase_p.h"

#include <QObject>
#include <QThread>

/*============================================================================
================================ Terminal IO Handler Thread
============================================================================*/

class BTerminalIOHandlerThread : public QThread
{
    Q_OBJECT
    Q_DISABLE_COPY(BTerminalIOHandlerThread)
public:
    explicit BTerminalIOHandlerThread(QObject *parent = 0);
    ~BTerminalIOHandlerThread();
protected:
    void run();
};

/*============================================================================
================================ Terminal IO Handler Private
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandlerPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminalIOHandler)
    B_DECLARE_TR_FUNCTIONS(BTerminalIOHandler, q)
    Q_OBJECT
    Q_DISABLE_COPY(BTerminalIOHandlerPrivate)
public:
    explicit BTerminalIOHandlerPrivate(BTerminalIOHandler *q);
    ~BTerminalIOHandlerPrivate();
    //
    void init();
    //
    static QMutex instMutex;
    static QMutex stdinMutex;
    static QMutex stdoutMutex;
    static QMutex readLineMutex;
    static QMutex echoMutex;
    static bool prefereReadLine;
    static QString lastLine;
};

#endif // BTERMINALIOHANDLER_P_H
