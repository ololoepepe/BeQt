#ifndef BABSTRACTTERMINALDRIVER_H
#define BABSTRACTTERMINALDRIVER_H

class QString;

#include <BeQtCore/BeQtGlobal>

#include <QObject>

class B_WIDGETS_EXPORT BAbstractTerminalDriver : public QObject
{
    Q_OBJECT
public:
    explicit BAbstractTerminalDriver(QObject *parent = 0);
    ~BAbstractTerminalDriver();
    //
    virtual QString prompt() const = 0;
    virtual bool isActive() const = 0;
    virtual QString read() = 0;
    virtual bool applyCommand(const QString &command) = 0;
    virtual void close() = 0;
    virtual void terminate() = 0;
    virtual void kill() = 0;
signals:
    void readyRead();
    void finished(int exitCode);
protected slots:
    void emitReadyRead();
    void emitFinished(int exitCode);
private:
    Q_DISABLE_COPY(BAbstractTerminalDriver)
};

#endif // BABSTRACTTERMINALDRIVER_H
