#ifndef BTERMINALWIDGET_P_H
#define BTERMINALWIDGET_P_H

class BTerminalWidgetPrivate;
class BAbstractTerminalDriver;
class BPlainTextEdit;

class QEvent;
class QString;
class QVBoxLayout;

#include "bterminalwidget.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

/*============================================================================
================================ Terminal Widget Private
============================================================================*/

class B_WIDGETS_EXPORT BTerminalWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminalWidget)
    Q_OBJECT
public:
    explicit BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode);
    ~BTerminalWidgetPrivate();
    //
    bool eventFilter(QObject *object, QEvent *event);
    void setDriver(BAbstractTerminalDriver *drv);
    bool handleKeyPress(int key, int modifiers);
    void scrollDown();
    void appendText(const QString &text);
    void appendLine( const QString &text = QString() );
    //
    const bool NormalMode;
    //
    BAbstractTerminalDriver *driver;
    int terminatingKey;
    int terminatingModifiers;
    QString terminatingSymbols;
    int len;
    //
    QVBoxLayout *vlt;
      BPlainTextEdit *ptedt;
public slots:
    void read();
    void finished(int exitCode);
    void blockTerminal();
    void unblockTerminal();
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivate)
};

#endif // BTERMINALWIDGET_P_H
