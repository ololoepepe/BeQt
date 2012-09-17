#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

#include <QThread>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QMutex>

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

class BCORESHARED_EXPORT BTerminalIOHandler : public QThread
{
    Q_OBJECT
public:
    static BTerminalIOHandler *instance();
    static QString readLine(bool *ok = 0);
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
