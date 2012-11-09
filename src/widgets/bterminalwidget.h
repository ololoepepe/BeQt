#ifndef BTERMINALWIDGET_H
#define BTERMINALWIDGET_H

class BTerminalWidgetPrivate;
class BAbstractTerminalDriver;

class QWidget;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

class B_WIDGETS_EXPORT BTerminalWidget : public QPlainTextEdit, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalWidget)
public:
    explicit BTerminalWidget(QWidget *parent = 0);
    explicit BTerminalWidget(BAbstractTerminalDriver *driver, QWidget *parent = 0);
    ~BTerminalWidget();
    //
    void setDriver(BAbstractTerminalDriver *driver);
    BAbstractTerminalDriver *driver() const;
    bool isValid() const;
    bool isActive() const;
public slots:
    void close();
    void terminate();
signals:
    void finished(int exitCode);
protected:
    BTerminalWidget(BTerminalWidgetPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BTerminalWidget)
};

#endif // BTERMINALWIDGET_H
