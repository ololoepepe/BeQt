#ifndef BSTDIO_H
#define BSTDIO_H

#include <QThread>
#include <QString>
#include <QTextStream>
#include <QStringList>

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

class BCORESHARED_EXPORT BStdIO : public QThread
{
    Q_OBJECT
public:
    static BStdIO *instance();
    static QString readLine();
    static void write(const QString &text);
    static void writeLine(const QString &text);
    static QStringList splitCommand(const QString &command);
    static void setStdinEchoEnabled(bool enabled = true);
public slots:
    void writeSlot(const QString &text);
protected:
    BStdIO();
    //
    void run();
private:
    static BStdIO *_m_instance;
signals:
    void read(const QString &text);
};

#endif // BSTDIO_H
