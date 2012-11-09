#ifndef BTERMINAL_P_H
#define BTERMINAL_P_H

class BTerminalPrivate;
class BAbstractTerminalDriver;

class QEvent;

#include "bterminal.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

class B_WIDGETS_EXPORT BTerminalPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BTerminalPrivateObject(BTerminalPrivate *p);
    ~BTerminalPrivateObject();
    //
    bool eventFilter(QObject *object, QEvent *event);
    //
    BTerminalPrivate *const _m_p;
public slots:
    void readyRead();
    void finished(int exitCode);
private:
    Q_DISABLE_COPY(BTerminalPrivateObject)
};

class B_WIDGETS_EXPORT BTerminalPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminal)
    Q_DECLARE_TR_FUNCTIONS(BTerminal)
public:
    explicit BTerminalPrivate(BTerminal *q);
    ~BTerminalPrivate();
    //
    void setDriver(BAbstractTerminalDriver *drv);
    bool handleKeyPress(int key, int modifiers);
    void scrollDown();
    void read();
    void finished(int exitCode);
    void appendText(const QString &text);
    void appendLine( const QString &text = QString() );
    //
    BTerminalPrivateObject *const _m_o;
    //
    BAbstractTerminalDriver *driver;
    int len;
private:
    Q_DISABLE_COPY(BTerminalPrivate)
    //
    friend class BTerminalPrivateObject;
};

#endif // BTERMINAL_P_H
