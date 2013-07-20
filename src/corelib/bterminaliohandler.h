#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

class BTerminalIOHandlerPrivate;
class BSettingsNode;

class QString;
class QStringLit;

#include "bglobal.h"
#include "bbase.h"
#include "btranslation.h"

#include <QObject>

#define bReadLine BTerminalIOHandler::readLine
#define bReadLineSecure BTerminalIOHandler::readLineSecure
#define bWrite BTerminalIOHandler::write
#define bWriteLine BTerminalIOHandler::writeLine
#define bWriteErr BTerminalIOHandler::writeErr
#define bWriteLineErr BTerminalIOHandler::writeLineErr

/*============================================================================
================================ BTerminalIOHandler ==========================
============================================================================*/

class B_CORE_EXPORT BTerminalIOHandler : public QObject, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalIOHandler)
    B_DECLARE_PRIVATE_S(BTerminalIOHandler)
public:
    enum StandardCommand
    {
        QuitCommand,
        SetCommand,
        HelpCommand
    };
public:
    struct CommandHelp
    {
        QString usage;
        BTranslation description;
    };
public:
    typedef QList<CommandHelp> CommandHelpList;
    typedef bool (BTerminalIOHandler::*InternalHandler)(const QString &, const QStringList &);
    typedef bool (*ExternalHandler)(BTerminalIOHandler *, const QString &, const QStringList &);
public:
    explicit BTerminalIOHandler(QObject *parent = 0);
    ~BTerminalIOHandler();
protected:
    explicit BTerminalIOHandler(BTerminalIOHandlerPrivate &d, QObject *parent = 0);
public:
    static QString command(StandardCommand cmd);
    static QStringList commands(StandardCommand cmd);
    static CommandHelp commandHelp(StandardCommand cmd);
    static CommandHelpList commandHelpList(StandardCommand cmd);
    static InternalHandler handler(StandardCommand cmd);
    static BTerminalIOHandler *instance();
    static QStringList splitCommand(const QString &command);
    static QString mergeArguments(const QStringList &arguments);
    static QString readLine(const QString &text = QString());
    static QString readLineSecure(const QString &text = QString());
    static void write(const QString &text);
    static void writeLine( const QString &text = QString() );
    static void writeErr(const QString &text);
    static void writeLineErr( const QString &text = QString() );
    static void writeHelpLine(const QString &usage, const QString &description);
    static void writeHelpLine(const QString &usage, const BTranslation &description, bool translate = true);
    static void writeHelpLines(const CommandHelpList &list, bool translate = true);
    static void setStdinEchoEnabled(bool enabled = true);
    static void setTerminalTitle(const QString &title);
    static void installHandler(const QString &command, InternalHandler handler);
    static void installHandler(const QString &command, ExternalHandler handler);
    static void installHandler(StandardCommand cmd);
    static void installHandler(StandardCommand cmd, InternalHandler handler);
    static void installHandler(StandardCommand cmd, ExternalHandler handler);
    static void setRootSettingsNode(BSettingsNode *root);
    static BSettingsNode *createBeQtSettingsNode(BSettingsNode *parent = rootSettingsNode());
    static void setTranslationsEnabled(bool enabled);
    static void setHelpDescription(const BTranslation &t);
    static void setCommandHelp(const QString &command, const CommandHelp &help);
    static void setCommandHelp(const QString &command, const CommandHelpList &list);
    static BSettingsNode *rootSettingsNode();
    static bool translationsEnabled();
protected:
    virtual bool handleCommand(const QString &command, const QStringList &arguments);
    bool handleQuit(const QString &command, const QStringList &arguments);
    bool handleSet(const QString &command, const QStringList &arguments);
    bool handleHelp(const QString &command, const QStringList &arguments);
Q_SIGNALS:
    void commandEntered(const QString &command, const QStringList &arguments);
protected:
    static BTerminalIOHandler *_m_self;
private:
    Q_DISABLE_COPY(BTerminalIOHandler)
};

#endif // BTERMINALIOHANDLER_H
