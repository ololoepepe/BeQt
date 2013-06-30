#ifndef BSQLWHERE_H
#define BSQLWHERE_H

class BSqlWherePrivate;

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QString>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BSqlWhere ===================================
============================================================================*/

class B_SQL_EXPORT BSqlWhere : public BBase
{
    B_DECLARE_PRIVATE(BSqlWhere)
public:
    explicit BSqlWhere(const QString &string, const QVariantMap &values);
    explicit BSqlWhere(const QString &string = QString(), const QString &placeholder1 = QString(),
                       const QVariant &boundValue1 = QVariant(), const QString &placeholder2 = QString(),
                       const QVariant &boundValue2 = QVariant());
    BSqlWhere(const BSqlWhere &other);
    ~BSqlWhere();
public:
    void setString(const QString &s);
    void setBoundValues(const QVariantMap &values);
    void setBoundValues(const QString &placeholder1, const QVariant &boundValue1,
                        const QString &placeholder2 = QString(), const QVariant &boundValue2 = QVariant());
    QString string() const;
    QVariantMap boundValues() const;
    bool isValid() const;
public:
    BSqlWhere &operator =(const BSqlWhere &other);
    bool operator ==(const BSqlWhere &other) const;
};

#endif // BSQLWHERE_H
