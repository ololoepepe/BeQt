#ifndef BSQLDATABASE_H
#define BSQLDATABASE_H

class BSqlDatabasePrivate;
class BSqlResult;

class QSqlDriver;
class QSqlError;
class QTextCodec;

#include "bsqlquery.h"
#include "bsqlwhere.h"

#include <BeQtCore/BeQt>
#include <BeQtCore/BBase>

#include <QString>
#include <QSqlDatabase>
#include <QLatin1String>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QList>

/*============================================================================
================================ BSqlDatabase ================================
============================================================================*/

class B_SQL_EXPORT BSqlDatabase : public BBase
{
    B_DECLARE_PRIVATE(BSqlDatabase)
    Q_DECLARE_TR_FUNCTIONS(BSqlDatabase)
public:
    static QStringList schemaFromText(const QString &text);
    static QStringList schemaFromFile(const QString &fileName, QTextCodec *codec = 0);
    static QStringList schemaFromFile(const QString &fileName, const QString &codecName);
public:
    explicit BSqlDatabase(const QString &type, const QString &connectionName = QLatin1String("defaultConnection"));
    explicit BSqlDatabase(QSqlDriver *driver, const QString &connectionName = QLatin1String("defaultConnection"));
    ~BSqlDatabase();
public:
    void setConnectOptions(const QString &options = QString());
    void setDatabaseName(const QString &name);
    void setHostName(const QString &host);
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);
    void setPassword(const QString &password);
    void setPort(int port);
    void setUserName(const QString &name);
    void setOpenOnDemand(bool b);
    void setCloseAfterTransaction(bool b);
    void setOnOpenQueries(const QList<BSqlQuery> &list);
    void setOnOpenQueries(const QStringList &list);
    void setOnOpenQuery(const BSqlQuery &q);
    void setOnOpenQuery(const QString &qs, const QString &placeholder1 = QString(),
                        const QVariant &boundValue1 = QVariant(), const QString &placeholder2 = QString(),
                        const QVariant &boundValue2 = QVariant());
    void setOnOpenQuery(const QString &qs, const QVariantMap &boundValues);
    void setOnOpenQuery(const QString &qs, const QVariant &boundValue1, const QVariant &boundValue2 = QVariant());
    void setOnOpenQuery(const QString &qs, const QVariantList &boundValues);
    QSqlDriver *driver() const;
    QString driverName() const;
    QString connectionName() const;
    QString connectOptions() const;
    QString databaseName() const;
    QString hostName() const;
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
    QString password() const;
    int port() const;
    QString userName() const;
    bool openOnDemand() const;
    bool closeAfterTransaction() const;
    QList<BSqlQuery> onOpenQueries() const;
    BSqlQuery onOpenQuery() const;
    bool open();
    bool open(const QString &user, const QString &password);
    void close();
    bool transaction();
    bool commit();
    bool rollback();
    bool endTransaction(bool success = true);
    bool isOpen() const;
    bool isOpenError() const;
    bool isValid() const;
    QSqlError lastError() const;
    QStringList tables(QSql::TableType type = QSql::Tables) const;
    QStringList tables(QSql::TableType type = QSql::Tables);
    bool tableExists(const QString &table, QSql::TableType type = QSql::Tables) const;
    bool tableExists(const QString &table, QSql::TableType type = QSql::Tables);
    BSqlResult exec(const BSqlQuery &q);
    BSqlResult exec(const QString &qs, const QString &placeholder1 = QString(),
                    const QVariant &boundValue1 = QVariant(), const QString &placeholder2 = QString(),
                    const QVariant &boundValue2 = QVariant());
    BSqlResult exec(const QString &qs, const QVariant &boundValue1, const QVariant &boundValue2 = QVariant());
    BSqlResult exec(const QString &qs, const QVariantList &boundValues);
    BSqlResult exec(const QString &qs, const QVariantMap &boundValues);
    BSqlResult select(const QString &table, const QStringList &fields, const BSqlWhere &where = BSqlWhere());
    BSqlResult select(const QString &table, const QString &field, const BSqlWhere &where = BSqlWhere());
    BSqlResult insert(const QString &table, const QVariantMap &values, const BSqlWhere &where = BSqlWhere());
    BSqlResult insert(const QString &table, const QString &field1, const QVariant &value1,
                      const QString &field2 = QString(), const QVariant &value2 = QVariant(),
                      const BSqlWhere &where = BSqlWhere());
    BSqlResult update(const QString &table, const QVariantMap &values, const BSqlWhere &where = BSqlWhere());
    BSqlResult update(const QString &table, const QString &field1, const QVariant &value1,
                      const QString &field2 = QString(), const QVariant &value2 = QVariant(),
                      const BSqlWhere &where = BSqlWhere());
    BSqlResult deleteFrom(const QString &table, const BSqlWhere &where = BSqlWhere());
    bool initializeFromSchema(const QString &schemaText);
    bool initializeFromSchema(const QStringList &schema);
    bool initializeFromSchemaFile(const QString &fileName, QTextCodec *codec = 0);
    bool initializeFromSchemaFile(const QString &fileName, const QString &codecName);
protected:
    QSqlDatabase *innerDatabase() const;
private:
    Q_DISABLE_COPY(BSqlDatabase)
};

#endif // BSQLDATABASE_H
