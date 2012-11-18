#ifndef BLOCALTERMINALDRIVER_H
#define BLOCALTERMINALDRIVER_H

class BLocalTerminalDriverPrivate;

class QString;

#include "babstractterminaldriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ Local Terminal Driver
============================================================================*/

class B_WIDGETS_EXPORT BLocalTerminalDriver : public BAbstractTerminalDriver, public BBase
{
    B_DECLARE_PRIVATE(BLocalTerminalDriver)
    Q_OBJECT
public:
    explicit BLocalTerminalDriver(QObject *parent = 0);
    ~BLocalTerminalDriver();
    //
    QString processCommand(const QString &command, const QStringList &arguments);
    bool isActive() const;
    QString read();
    void close();
    void terminate();
    void kill();
    QString prompt() const;
    QString terminalCommand(const QString &command, const QStringList &arguments,
                            bool *finished = 0, int *exitCode = 0);
    void setWorkingDirectory(const QString &path);
    QString workingDirectory() const;
protected:
    BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent = 0);
private:
    Q_DISABLE_COPY(BLocalTerminalDriver)
};

#endif // BLOCALTERMINALDRIVER_H
