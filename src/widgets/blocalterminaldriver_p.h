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

/*============================================================================
================================ Local Terminal Driver Private
============================================================================*/

class B_WIDGETS_EXPORT BLocalTerminalDriverPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BLocalTerminalDriver)
    Q_OBJECT
public:
    explicit BLocalTerminalDriverPrivate(BLocalTerminalDriver *q);
    ~BLocalTerminalDriverPrivate();
    //
    void init();
    //
    QProcess *process;
    QString workingDirectory;
public slots:
    void finished(int exitCode);
    void readyRead();
private:
    Q_DISABLE_COPY(BLocalTerminalDriverPrivate)
};

#endif // BLOCALTERMINALDRIVER_P_H
