#ifndef BCRYPTOR_H
#define BCRYPTOR_H

#include <QObject>
#include <QByteArray>
#include <QString>

#if defined(BCORE_LIBRARY)
#  define BCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define BCORESHARED_EXPORT Q_DECL_IMPORT
#endif

class BCORESHARED_EXPORT BCryptor : public QObject
{
    Q_OBJECT
public:
    enum Method
    {
        NoMethod,
        XorMethod
    };
    //
    static QByteArray encryptDataStatic(const QByteArray &data, const QByteArray &key, Method method = XorMethod);
    static QByteArray encryptDataStatic(const QString &data, const QString &key, Method method = XorMethod);
    static QByteArray decryptDataStatic(const QByteArray &data, const QByteArray &key, Method method = XorMethod);
    static QString decryptDataStatic(const QByteArray &data, const QString &key, Method method = XorMethod);
    //
    explicit BCryptor(QObject *parent = 0);
public slots:
    void encryptData(const QByteArray &data, const QByteArray &key, Method method = XorMethod);
    void decryptData(const QByteArray &data, const QByteArray &key, Method method = XorMethod);
private:
    static QByteArray _m_encryptXor(const QByteArray &data, const QByteArray &key);
    //
    bool _m_isCrypting;
signals:
    void dataEncrypted(const QByteArray &data);
    void dataDecrypted(const QByteArray &data);
};

#endif // MCRYPTOR_H
