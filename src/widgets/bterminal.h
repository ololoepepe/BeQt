#ifndef BTERMINAL_H
#define BTERMINAL_H

class BTerminalPrivate;
class BAbstractTerminalDriver;

class QWidget;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QPlainTextEdit>

class B_WIDGETS_EXPORT BTerminal : public QPlainTextEdit, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminal)
public:
    explicit BTerminal(QWidget *parent = 0);
    explicit BTerminal(BAbstractTerminalDriver *driver, QWidget *parent = 0);
    ~BTerminal();
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
    BTerminal(BTerminalPrivate &d, QWidget *parent = 0);
private:
    Q_DISABLE_COPY(BTerminal)
};

#endif // BTERMINAL_H
