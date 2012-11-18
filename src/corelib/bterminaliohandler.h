#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

class BTerminalIOHandlerPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QThread>
#include <QString>
#include <QStringList>

/*============================================================================
================================ Terminal IO Handler
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandler : public QThread, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalIOHandler)
    B_DECLARE_PRIVATE_S(BTerminalIOHandler)
public:
    static QStringList splitCommand(const QString &command);
    static BTerminalIOHandler *instance();
    static QString readLine();
    static void write(const QString &text);
    static void writeLine(const QString &text);
    static void setStdinEchoEnabled(bool enabled = true);
signals:
    void commandEntered(const QString &command, const QStringList &arguments);
protected:
    void run();
    //
    static BTerminalIOHandler *_m_self;
private:
    Q_DISABLE_COPY(BTerminalIOHandler)
    //
    BTerminalIOHandler();
    ~BTerminalIOHandler();
};

#endif // BTERMINALIOHANDLER_H
