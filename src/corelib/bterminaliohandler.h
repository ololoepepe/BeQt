#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

class BTerminalIOHandlerPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>
#include <QString>
#include <QStringList>

/*============================================================================
================================ Terminal IO Handler
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandler : public QObject, public BBase
{
    B_DECLARE_PRIVATE(BTerminalIOHandler)
    B_DECLARE_PRIVATE_S(BTerminalIOHandler)
    Q_OBJECT
    Q_DISABLE_COPY(BTerminalIOHandler)
public:
    static QStringList splitCommand(const QString &command);
    static BTerminalIOHandler *instance();
    static void start();
    static QString readLine();
    static void write(const QString &text);
    static void writeLine(const QString &text);
    static void setStdinEchoEnabled(bool enabled = true);
    //
    explicit BTerminalIOHandler(QObject *parent = 0);
    ~BTerminalIOHandler();
protected:
    BTerminalIOHandler(BTerminalIOHandlerPrivate &d, QObject *parent = 0);
signals:
    void commandEntered(const QString &command, const QStringList &arguments);
protected:
    static BTerminalIOHandler *_m_self;
};

#endif // BTERMINALIOHANDLER_H
