#ifndef BSQLRESULT_H
#define BSQLRESULT_H

class BSqlResultPrivate;

class QString;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QVariant>
#include <QVariantMap>
#include <QSqlError>
#include <QList>

/*============================================================================
================================ BSqlResult ==================================
============================================================================*/

class B_SQL_EXPORT BSqlResult : public BBase
{
    B_DECLARE_PRIVATE(BSqlResult)
public:
    typedef QList<QVariantMap> BVariantMapList;
public:
    explicit BSqlResult(bool success = false, const QSqlError &error = QSqlError());
    explicit BSqlResult(const QSqlError &error);
    BSqlResult(const BSqlResult &other);
    ~BSqlResult();
public:
    void setSuccess(bool b);
    void setValues(const BVariantMapList &list);
    void setLastInsertId(const QVariant &id);
    void setLastError(const QSqlError &e);
    bool success() const;
    BVariantMapList values() const;
    QVariantMap value() const;
    QVariant value(const QString &key, const QVariant &defValue = QVariant()) const;
    QVariant lastInsertId(QVariant::Type t = QVariant::Invalid) const;
    QSqlError lastError() const;
public:
    BSqlResult &operator =(const BSqlResult &other);
    bool operator ==(const BSqlResult &other) const;
    operator bool() const;
};

#endif // BSQLRESULT_H
