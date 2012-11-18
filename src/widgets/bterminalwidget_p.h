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

class B_WIDGETS_EXPORT BTerminalWidgetPrivateObject : public BBasePrivateObject
{
    B_DECLARE_PRIVATE_O(BTerminalWidget)
    Q_OBJECT
public:
    explicit BTerminalWidgetPrivateObject(BTerminalWidgetPrivate *p);
    ~BTerminalWidgetPrivateObject();
    //
    bool eventFilter(QObject *object, QEvent *event);
public slots:
    void readyRead();
    void finished(int exitCode);
    void blockTerminal();
    void unblockTerminal();
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivateObject)
};

class B_WIDGETS_EXPORT BTerminalWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminalWidget)
    B_DECLARE_OBJECT(BTerminalWidget)
    Q_DECLARE_TR_FUNCTIONS(BTerminalWidget)
public:
    explicit BTerminalWidgetPrivate(BTerminalWidget *q, bool nmode);
    ~BTerminalWidgetPrivate();
    //
    void setDriver(BAbstractTerminalDriver *drv);
    bool handleKeyPress(int key, int modifiers);
    void scrollDown();
    void appendText(const QString &text);
    void appendLine( const QString &text = QString() );
    void read();
    void finished(int exitCode);
    void blockTerminal();
    void unblockTerminal();
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
protected:
    BTerminalWidgetPrivate(BTerminalWidget &q, BTerminalWidgetPrivateObject &o, bool mode);
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivate)
};

#endif // BTERMINALWIDGET_P_H
