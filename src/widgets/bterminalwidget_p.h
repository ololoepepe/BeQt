#ifndef BTERMINALWIDGET_P_H
#define BTERMINALWIDGET_P_H

class BTerminalWidgetPrivate;
class BAbstractTerminalDriver;

class QEvent;
class QString;

#include "bterminalwidget.h"

#include <BeQtCore/private/bbase_p.h>

#include <QObject>
#include <QCoreApplication>

class B_WIDGETS_EXPORT BTerminalWidgetPrivateObject : public QObject
{
    Q_OBJECT
public:
    explicit BTerminalWidgetPrivateObject(BTerminalWidgetPrivate *p);
    ~BTerminalWidgetPrivateObject();
    //
    bool eventFilter(QObject *object, QEvent *event);
    //
    BTerminalWidgetPrivate *const _m_p;
public slots:
    void readyRead();
    void finished(int exitCode);
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivateObject)
};

class B_WIDGETS_EXPORT BTerminalWidgetPrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BTerminalWidget)
    Q_DECLARE_TR_FUNCTIONS(BTerminalWidget)
public:
    explicit BTerminalWidgetPrivate(BTerminalWidget *q);
    ~BTerminalWidgetPrivate();
    //
    void setDriver(BAbstractTerminalDriver *drv);
    bool handleKeyPress(int key, int modifiers);
    void scrollDown();
    void read();
    void finished(int exitCode);
    void appendText(const QString &text);
    void appendLine( const QString &text = QString() );
    //
    BTerminalWidgetPrivateObject *const _m_o;
    //
    BAbstractTerminalDriver *driver;
    bool userCommands;
    int terminatingKey;
    int terminatingModifiers;
    QString terminatingSymbols;
    int len;
private:
    Q_DISABLE_COPY(BTerminalWidgetPrivate)
    //
    friend class BTerminalWidgetPrivateObject;
};

#endif // BTERMINALWIDGET_P_H
