#ifndef BLOCALTERMINALDRIVER_P_H
#define BLOCALTERMINALDRIVER_P_H

class BLocalTerminalDriverPrivate;

class QProcess;
class QString;

#include "blocalterminaldriver.h"

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QMap>

class B_WIDGETS_EXPORT BLocalTerminalDriverPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BLocalTerminalDriver)
    Q_OBJECT
public:
    explicit BLocalTerminalDriverPrivateObject(BLocalTerminalDriverPrivate *p);
    ~BLocalTerminalDriverPrivateObject();
public slots:
    void finished(int exitCode);
    void readyRead();
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivateObject)
};

class B_WIDGETS_EXPORT BLocalTerminalDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocalTerminalDriver)
    B_DECLARE_OBJECT(BLocalTerminalDriver)
public:
    explicit BLocalTerminalDriverPrivate(BLocalTerminalDriver *q);
    ~BLocalTerminalDriverPrivate();
    //
    void finished(int exitCode);
    void readyRead();
    //
    QProcess *process;
    QString workingDirectory;
protected:
    BLocalTerminalDriverPrivate(BLocalTerminalDriver &q, BLocalTerminalDriverPrivateObject &o);
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivate)
};

#endif // BLOCALTERMINALDRIVER_P_H
