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

class B_WIDGETS_EXPORT BLocalTerminalDriverPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BLocalTerminalDriverPrivateObject(BLocalTerminalDriverPrivate *p);
    ~BLocalTerminalDriverPrivateObject();
    //
    BLocalTerminalDriverPrivate *const _m_p;
public slots:
    void finished(int exitCode);
    void readyRead();
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivateObject)
};

class B_WIDGETS_EXPORT BLocalTerminalDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocalTerminalDriver)
public:
    explicit BLocalTerminalDriverPrivate(BLocalTerminalDriver *q);
    ~BLocalTerminalDriverPrivate();
    //
    void finished(int exitCode);
    void readyRead();
    //
    BLocalTerminalDriverPrivateObject *const _m_o;
    //
    QProcess *process;
    QString workingDirectory;
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivate)
    //
    friend class BLocalTerminalDriverPrivateObject;
};

#endif // BLOCALTERMINALDRIVER_P_H
