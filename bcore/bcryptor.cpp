#include "bcryptor.h"

#include <QObject>
#include <QByteArray>
#include <QString>

QByteArray BCryptor::encryptDataStatic(const QByteArray &data, const QByteArray &key, Method method)
{
    switch (method)
    {
    case XorMethod:
        return _m_encryptXor(data, key);
    default:
        return data;
    }
}

QByteArray BCryptor::encryptDataStatic(const QString &data, const QString &key, Method method)
{
    return encryptDataStatic(data.toAscii(), key.toAscii(), method);
}

QByteArray BCryptor::decryptDataStatic(const QByteArray &data, const QByteArray &key, Method method)
{
    switch (method)
    {
    case XorMethod:
        return encryptDataStatic(data, key);
    default:
        return data;
    }
}

QString BCryptor::decryptDataStatic(const QByteArray &data, const QString &key, Method method)
{
    return QString::fromAscii( decryptDataStatic(data, key.toAscii(), method).data() );
}

//

BCryptor::BCryptor(QObject *parent) :
    QObject(parent)
{
    _m_isCrypting = false;
}

//

void BCryptor::encryptData(const QByteArray &data, const QByteArray &key, Method method)
{
    if (_m_isCrypting)
        return;
    _m_isCrypting = true;
    emit dataEncrypted( encryptDataStatic(data, key, method) );
    _m_isCrypting = false;
}

void BCryptor::decryptData(const QByteArray &data, const QByteArray &key, Method method)
{
    if (_m_isCrypting)
        return;
    _m_isCrypting = true;
    emit dataDecrypted( decryptDataStatic(data, key, method) );
    _m_isCrypting = false;
}

//

QByteArray BCryptor::_m_encryptXor(const QByteArray &data, const QByteArray &key)
{
    QByteArray ba = data;
    for (int i = 0; i < ba.size(); ++i)
    {
        char dc = ba.at(i);
        for (int j = 0; j < key.size(); ++j)
            dc ^= key.at(j);
        ba[i] = dc;
    }
    return ba;
}
