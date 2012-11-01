#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

#include <BeQt>

#include <QThread>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>

class B_CORE_EXPORT BTerminalIOHandler : public QThread
{
    Q_OBJECT
public:
    static BTerminalIOHandler *instance();
    static QString readLine();
    static void write(const QString &text);
    static void writeLine(const QString &text);
    static void setStdinEchoEnabled(bool enabled = true);
protected:
    void run();
private:
    static BTerminalIOHandler *_m_inst;
    static QMutex _m_instMutex;
    static QMutex _m_stdinMutex;
    static QMutex _m_stdoutMutex;
    static QMutex _m_readLineMutex;
    static bool _m_prefereReadLine;
    static QString _m_lastLine;
    //
    static QStringList _m_splitCommand(const QString &command);
    //
    BTerminalIOHandler();
    BTerminalIOHandler(const BTerminalIOHandler &other);
    ~BTerminalIOHandler();
    //
    BTerminalIOHandler &operator=(const BTerminalIOHandler &other);
signals:
    void commandEntered(const QString &command, const QStringList &arguments);
};

#endif // BTERMINALIOHANDLER_H
