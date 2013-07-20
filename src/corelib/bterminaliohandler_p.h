#ifndef BTERMINALIOHANDLER_P_H
#define BTERMINALIOHANDLER_P_H

class BTerminalIOHandlerPrivate;
class BSettingsNode;

class QString;

#include "bterminaliohandler.h"
#include "bglobal.h"
#include "bbase_p.h"
#include "btranslation.h"

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
    explicit BTerminalIOHandlerThread();
    ~BTerminalIOHandlerThread();
protected:
    void run();
signals:
    void lineRead(const QString &text);
public:
    QTextStream readStream;
    QEventLoop readLoop;
    QString lastLine;
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
    static BTerminalIOHandlerThread *initThread(bool silent = false);
    static void removeThread();
public:
    void init();
public Q_SLOTS:
    void lineRead(const QString &text);
public:
    static QMutex echoMutex;
    static QMutex titleMutex;
    static QMutex readMutex;
    static QMutex writeMutex;
    static QMutex writeErrMutex;
    static QTextStream writeStream;
    static QTextStream writeErrStream;
    static BTerminalIOHandlerThread *readThread;
    static QMutex threadMutex;
public:
    QMap<QString, BTerminalIOHandler::InternalHandler> internalHandlers;
    QMap<QString, BTerminalIOHandler::ExternalHandler> externalHandlers;
    BSettingsNode *root;
    bool translations;
    BTranslation help;
    QMap<QString, BTerminalIOHandler::CommandHelpList> commandHelp;
private:
    Q_DISABLE_COPY(BTerminalIOHandlerPrivate)
    friend class BTerminalIOHandlerThread;
};

#endif // BTERMINALIOHANDLER_P_H
