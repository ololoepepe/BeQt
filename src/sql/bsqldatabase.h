/****************************************************************************
**
** Copyright (C) 2012-2014 Andrey Bogdanov
**
** This file is part of the BeQtSql module of the BeQt library.
**
** BeQt is free software: you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** BeQt is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with BeQt.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef BSQLDATABASE_H
#define BSQLDATABASE_H

class BSqlDatabasePrivate;

class BSqlResult;

class QSqlDatabase;
class QSqlDriver;
class QSqlError;
class QTextCodec;

#include "bsqlquery.h"
#include "bsqlwhere.h"

#include <BeQtCore/BBase>

#include <QLatin1String>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

/*============================================================================
================================ BSqlDatabase ================================
============================================================================*/

class B_SQL_EXPORT BSqlDatabase : public BBase
{
    B_DECLARE_PRIVATE(BSqlDatabase)
    Q_DECLARE_TR_FUNCTIONS(BSqlDatabase)
public:
    explicit BSqlDatabase(const QString &type, const QString &connectionName = QLatin1String("defaultConnection"));
    explicit BSqlDatabase(QSqlDriver *driver, const QString &connectionName = QLatin1String("defaultConnection"));
    ~BSqlDatabase();
public:
    static QStringList schemaFromFile(const QString &fileName, QTextCodec *codec = 0);
    static QStringList schemaFromFile(const QString &fileName, const QString &codecName);
    static QStringList schemaFromText(const QString &text);
public:
    void close();
    bool closeAfterTransaction() const;
    bool commit();
    QString connectionName() const;
    QString connectOptions() const;
    QString databaseName() const;
    BSqlResult deleteFrom(const QString &table, const BSqlWhere &where = BSqlWhere());
    QSqlDriver *driver() const;
    QString driverName() const;
    bool endTransaction(bool success = true);
    BSqlResult exec(const BSqlQuery &q);
    BSqlResult exec(const QString &qs, const QString &placeholder1 = QString(),
                    const QVariant &boundValue1 = QVariant(), const QString &placeholder2 = QString(),
                    const QVariant &boundValue2 = QVariant());
    BSqlResult exec(const QString &qs, const QVariant &boundValue1, const QVariant &boundValue2 = QVariant());
    BSqlResult exec(const QString &qs, const QVariantList &boundValues);
    BSqlResult exec(const QString &qs, const QVariantMap &boundValues);
    QString hostName() const;
    bool initializeFromSchema(const QString &schemaText, bool transaction = true);
    bool initializeFromSchema(const QStringList &schema, bool transaction = true);
    bool initializeFromSchemaFile(const QString &fileName, QTextCodec *codec = 0, bool transaction = true);
    bool initializeFromSchemaFile(const QString &fileName, const QString &codecName, bool transaction = true);
    BSqlResult insert(const QString &table, const QVariantMap &values, const BSqlWhere &where = BSqlWhere());
    BSqlResult insert(const QString &table, const QString &field, const QVariant &value,
                      const BSqlWhere &where = BSqlWhere());
    BSqlResult insert(const QString &table, const QString &field1, const QVariant &value1,
                      const QString &field2, const QVariant &value2,
                      const BSqlWhere &where = BSqlWhere());
    BSqlResult insertOrReplace(const QString &table, const QVariantMap &values, const BSqlWhere &where = BSqlWhere());
    BSqlResult insertOrReplace(const QString &table, const QString &field, const QVariant &value,
                               const BSqlWhere &where = BSqlWhere());
    BSqlResult insertOrReplace(const QString &table, const QString &field1, const QVariant &value1,
                               const QString &field2, const QVariant &value2,
                               const BSqlWhere &where = BSqlWhere());
    bool isOpen() const;
    bool isOpenError() const;
    bool isValid() const;
    QSqlError lastError() const;
    QSql::NumericalPrecisionPolicy numericalPrecisionPolicy() const;
    QList<BSqlQuery> onOpenQueries() const;
    BSqlQuery onOpenQuery() const;
    bool open();
    bool open(const QString &user, const QString &password);
    bool openOnDemand() const;
    QString password() const;
    int port() const;
    bool rollback();
    bool rollbackOnCommitFail() const;
    BSqlResult select(const QString &table, const QStringList &fields, const BSqlWhere &where = BSqlWhere());
    BSqlResult select(const QString &table, const QString &field, const BSqlWhere &where = BSqlWhere());
    void setCloseAfterTransaction(bool b);
    void setConnectOptions(const QString &options = QString());
    void setDatabaseName(const QString &name);
    void setHostName(const QString &host);
    void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy);
    void setOnOpenQueries(const QList<BSqlQuery> &list);
    void setOnOpenQueries(const QStringList &list);
    void setOnOpenQuery(const BSqlQuery &q);
    void setOnOpenQuery(const QString &qs, const QString &placeholder1 = QString(),
                        const QVariant &boundValue1 = QVariant(), const QString &placeholder2 = QString(),
                        const QVariant &boundValue2 = QVariant());
    void setOnOpenQuery(const QString &qs, const QVariantMap &boundValues);
    void setOnOpenQuery(const QString &qs, const QVariant &boundValue1, const QVariant &boundValue2 = QVariant());
    void setOnOpenQuery(const QString &qs, const QVariantList &boundValues);
    void setOpenOnDemand(bool b);
    void setPassword(const QString &password);
    void setPort(int port);
    void setRollbackOnCommitFail(bool b);
    void setUserName(const QString &name);
    bool tableExists(const QString &table, QSql::TableType type = QSql::Tables) const;
    bool tableExists(const QString &table, QSql::TableType type = QSql::Tables);
    QStringList tables(QSql::TableType type = QSql::Tables) const;
    QStringList tables(QSql::TableType type = QSql::Tables);
    bool transaction();
    BSqlResult update(const QString &table, const QVariantMap &values, const BSqlWhere &where = BSqlWhere());
    BSqlResult update(const QString &table, const QString &field, const QVariant &value,
                      const BSqlWhere &where = BSqlWhere());
    BSqlResult update(const QString &table, const QString &field1, const QVariant &value1, const QString &field2,
                      const QVariant &value2, const BSqlWhere &where = BSqlWhere());
    QString userName() const;
protected:
    QSqlDatabase *innerDatabase() const;
private:
    Q_DISABLE_COPY(BSqlDatabase)
};

#endif // BSQLDATABASE_H
