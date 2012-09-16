#ifndef BTERMINALIOHANDLER_H
#define BTERMINALIOHANDLER_H

#include <QThread>
#include <QString>
#include <QTextStream>
#include <QStringList>

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
    static void write(const QString &text);
    static void writeLine(const QString &text);
    static void setStdinEchoEnabled(bool enabled = true);
protected:
    void run();
private:
    BTerminalIOHandler();
    BTerminalIOHandler(const BTerminalIOHandler &other);
    ~BTerminalIOHandler();
    //
    BTerminalIOHandler &operator=(const BTerminalIOHandler &other);
signals:
    void commandEntered(const QString &command, const QStringList &arguments);
};

#endif // BTERMINALIOHANDLER_H
