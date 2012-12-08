#ifndef BTERMINALWIDGET_H
#define BTERMINALWIDGET_H

class BTerminalWidgetPrivate;
class BAbstractTerminalDriver;

class QWidget;
class QStringList;

#include <BeQtCore/BeQtGlobal>
#include <BeQtCore/BBase>

#include <QWidget>
#include <QString>

/*============================================================================
================================ BTerminalWidget =============================
============================================================================*/

class B_WIDGETS_EXPORT BTerminalWidget : public QWidget, public BBase
{
    Q_OBJECT
    B_DECLARE_PRIVATE(BTerminalWidget)
public:
    enum TerminalMode
    {
        NormalMode,
        ProgrammaticMode
    };
public:
    explicit BTerminalWidget(TerminalMode mode, QWidget *parent = 0);
    explicit BTerminalWidget(TerminalMode mode, BAbstractTerminalDriver *driver, QWidget *parent = 0);
    ~BTerminalWidget();
protected:
    explicit BTerminalWidget(BTerminalWidgetPrivate &d, QWidget *parent = 0);
public:
    void setDriver(BAbstractTerminalDriver *driver);
    void setTerminatingSequence( int key, int modifiers, const QString &displayedSymbols = QString() );
    void setWorkingDirectory(const QString &path);
    TerminalMode mode() const;
    BAbstractTerminalDriver *driver() const;
    QString workingDirectory() const;
    bool isValid() const;
    bool isActive() const;
public slots:
    void terminalCommand(const QString &command);
    void terminalCommand(const QString &command, const QStringList &arguments);
    void processCommand(const QString &command);
    void processCommand(const QString &command, const QStringList &arguments);
    void emulateUserInput(const QString &command);
    void close();
    void terminate();
    void kill();
    void clearEdit();
signals:
    void finished(int exitCode);
private:
    Q_DISABLE_COPY(BTerminalWidget)
};

#endif // BTERMINALWIDGET_H
