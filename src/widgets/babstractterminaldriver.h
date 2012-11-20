#ifndef BABSTRACTTERMINALDRIVER_H
#define BABSTRACTTERMINALDRIVER_H

class QString;
class QStringList;

#include <BeQtCore/BeQtGlobal>

#include <QObject>

/*============================================================================
================================ Abstract Terminal Driver
============================================================================*/

class B_WIDGETS_EXPORT BAbstractTerminalDriver : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractTerminalDriver(QObject *parent = 0);
    ~BAbstractTerminalDriver();
    //
    virtual bool processCommand(const QString &command, const QStringList &arguments, QString &error) = 0;
    virtual bool isActive() const = 0;
    virtual QString read() = 0;
    virtual void close() = 0;
    virtual void terminate() = 0;
    virtual void kill() = 0;
    virtual QString prompt() const;
    virtual bool terminalCommand(const QString &command, const QStringList &arguments, QString &error);
    virtual void setWorkingDirectory(const QString &path);
    virtual QString workingDirectory() const;
signals:
    void readyRead();
    void finished(int exitCode);
    void blockTerminal();
    void unblockTerminal();
protected slots:
    void emitReadyRead();
    void emitFinished(int exitCode);
    void emitBlockTerminal();
    void emitUnblockTerminal();
private:
    Q_DISABLE_COPY(BAbstractTerminalDriver)
};

#endif // BABSTRACTTERMINALDRIVER_H
