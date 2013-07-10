#ifndef BPASSWORD_H
#define BPASSWORD_H

class BPasswordPrivate;

#include "bglobal.h"
#include "bbase.h"

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

/*============================================================================
================================ BPassword ===================================
============================================================================*/

class B_CORE_EXPORT BPassword : public BBase
{
    B_DECLARE_PRIVATE(BPassword)
public:
    enum Mode
    {
        FlexibleMode = 0,
        AlwaysOpenMode,
        AlwaysEncryptedMode
    };
public:
    explicit BPassword(Mode m = FlexibleMode);
    explicit BPassword(const QString &s);
    explicit BPassword(QCryptographicHash::Algorithm a, const QByteArray &ba = QByteArray(), int charCountHint = 0);
    BPassword(const BPassword &other);
    ~BPassword();
protected:
    explicit BPassword(BPasswordPrivate &d);
public:
    void setMode(Mode m);
    void setPassword(const QString &s, QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    void setPassword(QCryptographicHash::Algorithm a, const QByteArray &ba, int charCountHint = 0);
    void encrypt(QCryptographicHash::Algorithm a = QCryptographicHash::Sha1);
    void clear();
    void restore(const QByteArray &data);
    Mode mode() const;
    QString openPassword() const;
    QByteArray encryptedPassword(int *hint = 0) const;
    QByteArray encryptedPassword(QCryptographicHash::Algorithm a, int *hint = 0) const;
    int charCountHint(Mode m = FlexibleMode) const;
    QCryptographicHash::Algorithm algorithm() const;
    QByteArray save(Mode m = FlexibleMode) const;
    bool isEncrypted() const;
public:
    BPassword &operator =(const BPassword &other);
    bool operator ==(const BPassword &other) const;
};

#endif // BPASSWORD_H
