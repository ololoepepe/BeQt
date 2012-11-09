#ifndef BLOCALTERMINALDRIVER_H
#define BLOCALTERMINALDRIVER_H

class BLocalTerminalDriverPrivate;

class QString;

#include "babstractterminaldriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QObject>

class B_WIDGETS_EXPORT BLocalTerminalDriver : public BAbstractTerminalDriver, public BBase
{
    B_DECLARE_PRIVATE(BLocalTerminalDriver)
    Q_OBJECT
public:
    explicit BLocalTerminalDriver(QObject *parent = 0);
    ~BLocalTerminalDriver();
    //
    QString prompt() const;
    QString invalidCommandMessage() const;
    bool isActive() const;
    QString read();
    bool applyCommand(const QString &command);
    void close();
    void terminate();
    void kill();
protected:
    BLocalTerminalDriver(BLocalTerminalDriverPrivate &d, QObject *parent = 0);
private:
    Q_DISABLE_COPY(BLocalTerminalDriver)
};

#endif // BLOCALTERMINALDRIVER_H
