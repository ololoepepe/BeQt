#ifndef BTERMINAL_H
#define BTERMINAL_H

class QPlainTextEdit;
class QProcess;
class QEvent;

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextCharFormat>

#if defined(BGUI_LIBRARY)
#  define BGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGUISHARED_EXPORT Q_DECL_IMPORT
#endif

class BGUISHARED_EXPORT BTerminal : public QObject
{
    Q_OBJECT
public:
    explicit BTerminal(QObject *parent = 0);
    //
    bool eventFilter(QObject *object, QEvent *event);
    bool start(const QString &program, const QStringList &arguments = QStringList(),
               const QString &workingDir = QString(), bool timeout = 30000);
    bool waitForFinished(int timeout = 30000);
    void terminate();
    void kill();
    void appendText( const QString &text, const QTextCharFormat &format = QTextCharFormat () );
    void appendLine( const QString &text, const QTextCharFormat &format = QTextCharFormat () );
    bool isStarted() const;
    const QString &program() const;
    const QStringList &arguments() const;
    const QString &workingDir() const;
    QWidget *widget() const;
public slots:
    void clearEdit();
private:
    QProcess *_m_proc;
    QPlainTextEdit *_m_edit;
    bool _m_started;
    QString _m_prog;
    QStringList _m_args;
    QString _m_wdir;
    int _m_len;
    //
    void _m_scrollDown();
    void _m_command(const QString &command);
    void _m_endOfFile();
private slots:
    void _m_readFromProcess();
    void _m_finished(int exitCode);
signals:
    void finished(int exitCode);
};

#endif // BTERMINAL_H
