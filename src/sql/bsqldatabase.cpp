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

#include "bsqldatabase.h"

#include "bsqlquery.h"
#include "bsqlresult.h"

#include <BeQtCore/BDirTools>
#include <BeQtCore/BeQt>
#include <BeQtCore/private/bbase_p.h>

#include <QDebug>
#include <QLatin1String>
#include <QList>
#include <QObject>
#include <QRegExp>
#include <QScopedPointer>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

/*============================================================================
================================ BSqlDatabasePrivate =========================
============================================================================*/

class BSqlDatabasePrivate : public BBasePrivate
{
    B_DECLARE_PUBLIC(BSqlDatabase)
public:
    bool closeAfterTransaction;
    QSqlDatabase *db;
    QList<BSqlQuery> onOpen;
    bool openOnDemand;
    bool rollback;
public:
    explicit BSqlDatabasePrivate(BSqlDatabase *q);
    ~BSqlDatabasePrivate();
public:
    bool handleOnOpen();
    bool handleOpenOnDemand();
    void init();
private:
    Q_DISABLE_COPY(BSqlDatabasePrivate)
};

/*============================================================================
================================ BSqlDatabasePrivate =========================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlDatabasePrivate::BSqlDatabasePrivate(BSqlDatabase *q) :
    BBasePrivate(q)
{
    //
}

BSqlDatabasePrivate::~BSqlDatabasePrivate()
{
    QString cn = db->connectionName();
    delete db;
    QSqlDatabase::removeDatabase(cn);
}

/*============================== Public methods ============================*/

bool BSqlDatabasePrivate::handleOnOpen()
{
    foreach (const BSqlQuery &q, onOpen) {
        if (!q_func()->exec(q))
            return false;
    }
    return true;
}

bool BSqlDatabasePrivate::handleOpenOnDemand()
{
    return db->isOpen() || (openOnDemand && q_func()->open());
}

void BSqlDatabasePrivate::init()
{
    db = 0;
    openOnDemand = true;
    closeAfterTransaction = false;
    rollback = true;
}

/*============================================================================
================================ BSqlDatabase ================================
============================================================================*/

/*============================== Public constructors =======================*/

BSqlDatabase::BSqlDatabase(const QString &type, const QString &connectionName) :
    BBase(*new BSqlDatabasePrivate(this))
{
    d_func()->init();
    d_func()->db = new QSqlDatabase(QSqlDatabase::addDatabase(type, connectionName));
}

BSqlDatabase::BSqlDatabase(QSqlDriver *driver, const QString &connectionName) :
    BBase(*new BSqlDatabasePrivate(this))
{
    d_func()->init();
    d_func()->db = new QSqlDatabase(QSqlDatabase::addDatabase(driver, connectionName));
}

BSqlDatabase::~BSqlDatabase()
{
    //
}

/*============================== Static public methods =====================*/

QStringList BSqlDatabase::schemaFromFile(const QString &fileName, QTextCodec *codec)
{
    return schemaFromText(BDirTools::readTextFile(fileName, codec));
}

QStringList BSqlDatabase::schemaFromFile(const QString &fileName, const QString &codecName)
{
    return schemaFromFile(fileName, BeQt::codec(codecName));
}

QStringList BSqlDatabase::schemaFromText(const QString &text)
{
    QStringList sl = text.split('\n');
    foreach (int i, bRangeD(0, sl.size() - 1))
        sl[i].remove(QRegExp("\\s*\\-\\-.*$")); //TODO: Not very safe, consider: ... '--'
    QStringList list = sl.join("\n").split(";\n");
    foreach (int i, bRangeD(0, list.size() - 1)) {
        list[i].replace('\n', ' ');
        list[i].replace(QRegExp("\\s+"), " ");
    }
    list.removeAll("");
    list.removeDuplicates();
    return list;
}

/*============================== Public methods ============================*/

void BSqlDatabase::close()
{
    d_func()->db->close();
}

bool BSqlDatabase::closeAfterTransaction() const
{
    return d_func()->closeAfterTransaction;
}

bool BSqlDatabase::commit()
{
    B_D(BSqlDatabase);
    bool b = d->db->commit();
    if (!b)
        rollback();
    if (d->closeAfterTransaction)
        d->db->close();
    return b;
}

QString BSqlDatabase::connectionName() const
{
    return d_func()->db->connectionName();
}

QString BSqlDatabase::connectOptions() const
{
    return d_func()->db->connectOptions();
}

QString BSqlDatabase::databaseName() const
{
    return d_func()->db->databaseName();
}

BSqlResult BSqlDatabase::deleteFrom(const QString &table, const BSqlWhere &where)
{
    if (table.isEmpty())
        return BSqlResult(tr("Invalid parameters", "errorString"));
    QString qs = "DELETE FROM " + table;
    if (where.isValid())
        qs += " WHERE " + where.string();
    return exec(qs, where.boundValues());
}

bool BSqlDatabase::endTransaction(bool success)
{
    return success ? commit() : rollback();
}

QSqlDriver *BSqlDatabase::driver() const
{
    return d_func()->db->driver();
}

QString BSqlDatabase::driverName() const
{
    return d_func()->db->driverName();
}

BSqlResult BSqlDatabase::exec(const BSqlQuery &q)
{
    if (!q.isValid())
        return BSqlResult(tr("Invalid parameters", "errorString"));
    if (!d_func()->handleOpenOnDemand())
        return BSqlResult(tr("Database is not opened", "errorString"));
    QScopedPointer<QSqlQuery> qq(q.createQuery(*d_func()->db));
    if (qq.isNull())
        return BSqlResult(tr("Failed to create query", "errorString"));
    if (!qq->exec())
        return BSqlResult(qq->lastError());
    BSqlResult::VariantMapList list;
    while (qq->next()) {
        QSqlRecord r = qq->record();
        QVariantMap m;
        foreach (int i, bRangeD(0, r.count() - 1))
            m.insert(r.fieldName(i), r.value(i));
        if (!m.isEmpty())
            list << m;
    }
    BSqlResult sr;
    sr.setSuccess(true);
    sr.setValues(list);
    sr.setLastInsertId(qq->lastInsertId());
    return sr;
}

BSqlResult BSqlDatabase::exec(const QString &qs, const QString &placeholder1,const QVariant &boundValue1,
                              const QString &placeholder2, const QVariant &boundValue2)
{
    QVariantMap m;
    if (!placeholder1.isEmpty() && boundValue1.isValid())
        m.insert(placeholder1, boundValue1);
    if (!placeholder2.isEmpty() && boundValue2.isValid())
        m.insert(placeholder2, boundValue2);
    return exec(qs, m);
}

BSqlResult BSqlDatabase::exec(const QString &qs, const QVariant &boundValue1, const QVariant &boundValue2)
{
    return exec(qs, QVariantList() << boundValue1 << boundValue2);
}

BSqlResult BSqlDatabase::exec(const QString &qs, const QVariantList &boundValues)
{
    return exec(BSqlQuery(qs, boundValues));
}

BSqlResult BSqlDatabase::exec(const QString &qs, const QVariantMap &boundValues)
{
    return exec(BSqlQuery(qs, boundValues));
}

QString BSqlDatabase::hostName() const
{
    return d_func()->db->hostName();
}

bool BSqlDatabase::initializeFromSchema(const QString &schemaText, bool transaction)
{
    QStringList list = schemaFromText(schemaText);
    if (list.isEmpty())
        return false;
    if (!isOpen() && !open())
        return false;
    if (transaction && !this->transaction())
        return false;
    foreach (const QString &qs, list) {
        if (!exec(qs)) {
            if (transaction)
                rollback();
            return false;
        }
    }
    if (transaction && !commit()) {
        rollback();
        return false;
    }
    return true;
}

bool BSqlDatabase::initializeFromSchema(const QStringList &schema, bool transaction)
{
    return initializeFromSchema(schema.join(";\n"), transaction);
}

bool BSqlDatabase::initializeFromSchemaFile(const QString &fileName, QTextCodec *codec, bool transaction)
{
    return initializeFromSchema(schemaFromFile(fileName, codec), transaction);
}

bool BSqlDatabase::initializeFromSchemaFile(const QString &fileName, const QString &codecName, bool transaction)
{
    return initializeFromSchema(schemaFromFile(fileName, codecName), transaction);
}

BSqlResult BSqlDatabase::insert(const QString &table, const QVariantMap &values, const BSqlWhere &where)
{
    if (table.isEmpty() || values.isEmpty())
        return BSqlResult(tr("Invalid parameters", "errorString"));
    QString qs = "INSERT INTO " + table + " (";
    QStringList keys = values.keys();
    qs += keys.join(", ");
    qs += ") VALUES (:";
    qs += keys.join(", :");
    qs += ")";
    if (where.isValid())
        qs += " WHERE " + where.string();
    BSqlQuery q(qs);
    foreach (const QString &k, keys)
        q.bindValue(":" + k, values.value(k));
    if (where.isValid())
        q.addBoundValues(where.boundValues());
    return exec(q);
}

BSqlResult BSqlDatabase::insert(const QString &table, const QString &field, const QVariant &value,
                                const BSqlWhere &where)
{
    QVariantMap m;
    if (!field.isEmpty() && value.isValid())
        m.insert(field, value);
    return insert(table, m, where);
}

BSqlResult BSqlDatabase::insert(const QString &table, const QString &field1, const QVariant &value1,
                                const QString &field2, const QVariant &value2, const BSqlWhere &where)
{
    QVariantMap m;
    if (!field1.isEmpty() && value1.isValid())
        m.insert(field1, value1);
    if (!field2.isEmpty() && value2.isValid())
        m.insert(field2, value2);
    return insert(table, m, where);
}

bool BSqlDatabase::isOpen() const
{
    return d_func()->db->isOpen();
}

bool BSqlDatabase::isOpenError() const
{
    return d_func()->db->isOpenError();
}

bool BSqlDatabase::isValid() const
{
    return d_func()->db->isValid();
}

QSqlError BSqlDatabase::lastError() const
{
    return d_func()->db->lastError();
}

QSql::NumericalPrecisionPolicy BSqlDatabase::numericalPrecisionPolicy() const
{
    return d_func()->db->numericalPrecisionPolicy();
}

QList<BSqlQuery> BSqlDatabase::onOpenQueries() const
{
    return d_func()->onOpen;
}

BSqlQuery BSqlDatabase::onOpenQuery() const
{
    return !d_func()->onOpen.isEmpty() ? d_func()->onOpen.first() : BSqlQuery();
}

bool BSqlDatabase::open()
{
    return d_func()->db->open() && d_func()->handleOnOpen();
}

bool BSqlDatabase::open(const QString &user, const QString &password)
{
    return d_func()->db->open(user, password) && d_func()->handleOnOpen();
}

bool BSqlDatabase::openOnDemand() const
{
    return d_func()->openOnDemand;
}

QString BSqlDatabase::password() const
{
    return d_func()->db->password();
}

int BSqlDatabase::port() const
{
    return d_func()->db->port();
}

bool BSqlDatabase::rollback()
{
    B_D(BSqlDatabase);
    bool b = d->db->rollback();
    if (d->closeAfterTransaction)
        d->db->close();
    return b;
}

bool BSqlDatabase::rollbackOnCommitFail() const
{
    return d_func()->rollback;
}

BSqlResult BSqlDatabase::select(const QString &table, const QStringList &fields, const BSqlWhere &where)
{
    if (table.isEmpty() || fields.isEmpty())
        return BSqlResult(tr("Invalid parameters", "errorString"));
    QString qs = "SELECT ";
    qs += fields.join(", ");
    qs += " FROM " + table;
    if (where.isValid())
        qs += " WHERE " + where.string();
    return exec(qs, where.boundValues());
}

BSqlResult BSqlDatabase::select(const QString &table, const QString &field, const BSqlWhere &where)
{
    return select(table, QStringList() << field, where);
}

void BSqlDatabase::setCloseAfterTransaction(bool b)
{
    d_func()->closeAfterTransaction = b;
}

void BSqlDatabase::setConnectOptions(const QString &options)
{
    d_func()->db->setConnectOptions(options);
}

void BSqlDatabase::setDatabaseName(const QString &name)
{
    d_func()->db->setDatabaseName(name);
}

void BSqlDatabase::setHostName(const QString &host)
{
    d_func()->db->setHostName(host);
}

void BSqlDatabase::setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy)
{
    d_func()->db->setNumericalPrecisionPolicy(precisionPolicy);
}

void BSqlDatabase::setOnOpenQueries(const QList<BSqlQuery> &list)
{
    d_func()->onOpen = list;
}

void BSqlDatabase::setOnOpenQueries(const QStringList &list)
{
    QList<BSqlQuery> ql;
    foreach (const QString &qs, list) {
        BSqlQuery q(qs);
        if (q.isValid())
            ql << q;
    }
    setOnOpenQueries(ql);
}

void BSqlDatabase::setOnOpenQuery(const BSqlQuery &q)
{
    setOnOpenQueries(QList<BSqlQuery>() << q);
}

void BSqlDatabase::setOnOpenQuery(const QString &qs, const QString &placeholder1, const QVariant &boundValue1,
                                  const QString &placeholder2, const QVariant &boundValue2)
{
    QVariantMap m;
    if (!placeholder1.isEmpty() && boundValue1.isValid())
        m.insert(placeholder1, boundValue1);
    if (!placeholder2.isEmpty() && boundValue2.isValid())
        m.insert(placeholder2, boundValue2);
    setOnOpenQuery(qs, m);
}

void BSqlDatabase::setOnOpenQuery(const QString &qs, const QVariantMap &boundValues)
{
    setOnOpenQuery(BSqlQuery(qs, boundValues));
}

void BSqlDatabase::setOnOpenQuery(const QString &qs, const QVariant &boundValue1, const QVariant &boundValue2)
{
    setOnOpenQuery(qs, QVariantList() << boundValue1 << boundValue2);
}

void BSqlDatabase::setOnOpenQuery(const QString &qs, const QVariantList &boundValues)
{
    setOnOpenQuery(BSqlQuery(qs, boundValues));
}

void BSqlDatabase::setOpenOnDemand(bool b)
{
    d_func()->openOnDemand = b;
}

void BSqlDatabase::setPassword(const QString &password)
{
    d_func()->db->setPassword(password);
}

void BSqlDatabase::setPort(int port)
{
    d_func()->db->setPort(port);
}

void BSqlDatabase::setRollbackOnCommitFail(bool b)
{
    d_func()->rollback = b;
}

void BSqlDatabase::setUserName(const QString &name)
{
    d_func()->db->setUserName(name);
}

bool BSqlDatabase::tableExists(const QString &table, QSql::TableType type) const
{
    return tables(type).contains(table, Qt::CaseInsensitive);
}

bool BSqlDatabase::tableExists(const QString &table, QSql::TableType type)
{
    return tables(type).contains(table, Qt::CaseInsensitive);
}

QStringList BSqlDatabase::tables(QSql::TableType type) const
{
    return d_func()->db->tables(type);
}

QStringList BSqlDatabase::tables(QSql::TableType type)
{
    return d_func()->handleOpenOnDemand() ? d_func()->db->tables(type) : QStringList();
}

bool BSqlDatabase::transaction()
{
    return d_func()->handleOpenOnDemand() && d_func()->db->transaction();
}

BSqlResult BSqlDatabase::update(const QString &table, const QVariantMap &values, const BSqlWhere &where)
{
    if (table.isEmpty() || values.isEmpty())
        return BSqlResult(tr("Invalid parameters", "errorString"));
    QString qs = "UPDATE " + table + " SET ";
    QStringList keys = values.keys();
    QStringList pairs;
    foreach (const QString &k, keys)
        pairs << (k + " = :" + k);
    qs += pairs.join(", ");
    if (where.isValid())
        qs += " WHERE " + where.string();
    BSqlQuery q(qs);
    foreach (const QString &k, keys)
        q.bindValue(":" + k, values.value(k));
    if (where.isValid())
        q.addBoundValues(where.boundValues());
    return exec(q);
}

BSqlResult BSqlDatabase::update(const QString &table, const QString &field, const QVariant &value,
                                const BSqlWhere &where)
{
    QVariantMap m;
    if (!field.isEmpty() && value.isValid())
        m.insert(field, value);
    return update(table, m, where);
}

BSqlResult BSqlDatabase::update(const QString &table, const QString &field1, const QVariant &value1,
                                const QString &field2, const QVariant &value2, const BSqlWhere &where)
{
    QVariantMap m;
    if (!field1.isEmpty() && value1.isValid())
        m.insert(field1, value1);
    if (!field2.isEmpty() && value2.isValid())
        m.insert(field2, value2);
    return update(table, m, where);
}

QString BSqlDatabase::userName() const
{
    return d_func()->db->userName();
}

/*============================== Protected methods =========================*/

QSqlDatabase *BSqlDatabase::innerDatabase() const
{
    return d_func()->db;
}
