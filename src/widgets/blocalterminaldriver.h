#ifndef BLOCALTERMINALDRIVER_H
#define BLOCALTERMINALDRIVER_H

class BLocalTerminalDriverPrivate;

class QString;
class QTextCodec;

#include "babstractterminaldriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

/*============================================================================
================================ BLocalTerminalDriver ========================
============================================================================*/

class B_WIDGETS_EXPORT BLocalTerminalDriver : public BAbstractTerminalDriver, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BLocalTerminalDriver)
public:
    explicit BLocalTerminalDriver(QObject *parent = 0);
    ~BLocalTerminalDriver();
protected:
    explicit BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent = 0);
public:
    bool isActive() const;
    QString read();
    void close();
    void terminate();
    void kill();
    QString prompt() const;
    bool terminalCommand(const QString &command, const QStringList &arguments, QString &error);
    bool processCommand(const QString &command, const QStringList &arguments, QString &error);
    void setWorkingDirectory(const QString &path);
    QString workingDirectory() const;
    void setCodec(QTextCodec *codec);
    void setCodec(const QString &codecName);
    QTextCodec *codec() const;
    QString codecName() const;
private:
    Q_DISABLE_COPY(BLocalTerminalDriver)
};

#endif // BLOCALTERMINALDRIVER_H
