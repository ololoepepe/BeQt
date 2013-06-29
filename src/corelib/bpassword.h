#ifndef BPASSWORD_H
#define BPASSWORD_H

class BPasswordPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QCryptographicHash>

/*============================================================================
================================ BPassword ===================================
============================================================================*/

class B_CORE_EXPORT BPassword : public BBase
{
    B_DECLARE_PRIVATE(BPassword)
public:
    explicit BPassword(const QString &s = QString());
    explicit BPassword(QCryptographicHash::Algorithm a, const QByteArray &ba = QByteArray(), int charCountHint = 0);
    BPassword(const BPassword &other);
    ~BPassword();
protected:
    explicit BPassword(BPasswordPrivate &d);
public:
    void setPassword(const QString &s);
    void setEncryptedPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint = 0);
    void encrypt(QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    void clear();
    QString password() const;
    QByteArray encryptedPassword() const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm a, int *charCountHint = 0) const;
    int charCountHint() const;
    QCryptographicHash::Algorithm algorithm() const;
    bool isEncrypted() const;
public:
    BPassword &operator =(const BPassword &other);
    bool operator ==(const BPassword &other) const;
};

#endif // BPASSWORD_H
