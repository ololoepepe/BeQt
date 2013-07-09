#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

class BTerminalIOHandlerPrivate;

class QString;
class QStringLit;

#include "bglobal.h"
#include "bbase.h"

#include <QObject>

/*============================================================================
================================ BTerminalIOHandler ==========================
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandler : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalIOHandler)
    B_DECLARE_PRIVATE_S(BTerminalIOHandler)
public:
    typedef bool (BTerminalIOHandler::*InternalHandler)(const QString &, const QStringList &);
    typedef bool (*ExternalHandler)(const QString &, const QStringList &);
public:
    explicit BTerminalIOHandler(QObject *parent = 0);
    ~BTerminalIOHandler();
protected:
    explicit BTerminalIOHandler(BTerminalIOHandlerPrivate &d, QObject *parent = 0);
public:
    static BTerminalIOHandler *instance();
    static QStringList splitCommand(const QString &command);
    static QString mergeArguments(const QStringList &arguments);
    static QString readLine(const QString &text = QString());
    static void write(const QString &text);
    static void writeLine( const QString &text = QString() );
    static void writeErr(const QString &text);
    static void writeLineErr( const QString &text = QString() );
    static void setStdinEchoEnabled(bool enabled = true);
    static void installHandler(const QString &command, InternalHandler handler);
    static void installHandler(const QString &command, ExternalHandler handler);
protected:
    virtual bool handleCommand(const QString &command, const QStringList &arguments);
Q_SIGNALS:
    void commandEntered(const QString &command, const QStringList &arguments);
protected:
    static BTerminalIOHandler *_m_self;
private:
    Q_DISABLE_COPY(BTerminalIOHandler)
};

#endif // BTERMINALIOHANDLER_H
