#ifndef BABSTRACTTERMINALDRIVER_H
#define BABSTRACTTERMINALDRIVER_H

class QString;
class QStringList;
class QVariant;

#include <BeQtCore/BeQtGlobal>

#include <QObject>

/*============================================================================
================================ BAbstractTerminalDriver =====================
============================================================================*/

class B_WIDGETS_EXPORT BAbstractTerminalDriver : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractTerminalDriver(QObject *parent = 0);
    ~BAbstractTerminalDriver();
public:
    virtual bool processCommand(const QString &command, const QStringList &arguments, QString &error) = 0;
    virtual bool isActive() const = 0;
    virtual QString read() = 0;
    virtual void close() = 0;
    virtual void terminate();
    virtual void kill();
    virtual QString prompt() const;
    virtual bool terminalCommand(const QString &command, const QStringList &arguments, QString &error);
    virtual bool terminalCommand(const QVariant &data, QString &error);
    virtual void setWorkingDirectory(const QString &path);
    virtual QString workingDirectory() const;
protected slots:
    void emitReadyRead();
    void emitFinished(int exitCode);
    void emitBlockTerminal();
    void emitUnblockTerminal();
signals:
    void readyRead();
    void finished(int exitCode);
    void blockTerminal();
    void unblockTerminal();
private:
    Q_DISABLE_COPY(BAbstractTerminalDriver)
};

#endif // BABSTRACTTERMINALDRIVER_H
