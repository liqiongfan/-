#ifndef DB_H
#define DB_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVector>
#include <QVariantMap>
#include <QVariantList>
#include <QtDebug>
#include <iostream>

namespace db {

class orm
{
    QSqlDatabase _db{};
    QString _tableName{}, _tableAlias{};
    QVariantMap _wheres{}, _sets{}, _orWheres{};
    QVector<QString> _fields{ "*" };
    QVector<QString> _groups{};
    QVector<QString> _orders{};
    int _limits{ -1 };
    int _offsets{ -1 };
    int numRowsAffected{ -1 };
    QVariant lastInsertId{};

    // Sql compile result
    QString _fieldsSql{}, _tableSql{}, _whereSql{},
            _groupSql{},  _orderSql{}, _limitSql{},
            _offsetSql{}, _setSql{}, _joinSql{};

    // All bind values
    QVariantList _binds{}, _insertVals{};

    QString _compileVector( const QVector<QString> &v, const QString &word = "," ) {
        Q_UNUSED(word)
        Q_UNUSED(v)

        QString str{};
        int size = v.size();
        for ( int i = 0; i < size; i++ ) {
            str.append(v.at(i));
            if ( i != size - 1 ) {
                str.append( word );
            }
        }
        return str;
    }

    QString _compileQVariantMap( const QVariantMap &v, const QString &word = "AND" ) {
        Q_UNUSED(v)
        Q_UNUSED(word)
        QString str{};
        int sizeCount = v.size() - 1;
        for ( auto iter = v.begin(); iter != v.end(); iter++ ) {
            QVariantMap val = iter.value().toMap();
            str.append(QString("`%1` %2 ?").arg(iter.key()).arg(val.firstKey()));
            this->_binds.push_back( val.first() );
            if ( sizeCount-- != 0 ) {
               str.append(QString(" %1 ").arg(word));
            }
        }
        return str;
    }

    QString _compileInsertMap( const QVariantList &v ) {
        QString str{};
        QVariantMap fields = v.first().toMap();
        int sizeCount = v.size(), fieldCount = fields.size();
        str.append("(");
        for ( auto fieldName = fields.begin(); fieldName != fields.end(); fieldName++ ) {
            str.append(QString("`%1`").arg(fieldName.key()));
            if ( --fieldCount != 0 ) {
                str.append(",");
            }
        }
        str.append(") VALUES ");
        for ( auto iter = v.begin(); iter != v.end(); iter++ ) {

            QVariantMap map = iter->toMap();
            int mapCount = map.size();
            str.append("(");
            for ( auto data: map) {
                str.append("?");
                this->_binds.push_back(data);
                if ( --mapCount != 0 ) {
                    str.append(", ");
                }
            }
            str.append(")");
            if ( --sizeCount != 0 ) {
                str.append(",");
            }
        }
        return str;
    }

    void _compileFields() {
        this->_fieldsSql = this->_compileVector( this->_fields );
    }

    void _compileTable() {
        if ( this->_tableAlias.isEmpty() ) {
            this->_tableSql = this->getTableName();
        } else {
            this->_tableSql = QString("`%1` AS %2")
                    .arg(this->getTableName())
                    .arg(this->_tableAlias)
            ;
        }
    }

    void _compileWhere() {
        if ( this->_wheres.size() ) {
            this->_whereSql = QString(" WHERE %1 ").arg( this->_compileQVariantMap( this->_wheres ) );
        }
        if ( this->_orWheres.size() ) {
            if ( this->_wheres.size() )
                this->_whereSql.append( QString(" OR %1 ").arg( this->_compileQVariantMap( this->_orWheres ) ) );
            else {
                this->_whereSql = QString(" WHERE %1 ").arg( this->_compileQVariantMap( this->_orWheres ) );
            }
        }
        if ( !this->_wheres.size() && !this->_orWheres.size() ) this->_whereSql.clear();
    }

    void _compileGroupBy() {
        if ( this->_groups.size() )
            this->_groupSql = QString(" GROUP BY %1 ").arg( this->_compileVector( this->_groups ) );
        else this->_groupSql.clear();
    }

    void _compileOrderBy() {
        if ( this->_orders.size() )
            this->_orderSql = QString(" ORDER BY %1 ").arg( this->_compileVector( this->_orders ) );
        else this->_orderSql.clear();
    }

    void _compileLimit() {
        if ( this->_limits != -1 ) {
            this->_binds.push_back( this->_limits );
            this->_limitSql = QString(" LIMIT ? ");
        } else this->_limitSql.clear();
    }

    void _compileOffset() {
        if ( this->_offsets != -1 ) {
            this->_binds.push_back( this->_offsets );
            this->_offsetSql = QString(" OFFSET ? ");
        } else this->_offsetSql.clear();
    }

    void _compileSet() {
        this->_setSql = this->_compileQVariantMap( this->_sets, "," );
    }

    QString _compileQuery() {
        this->_compileFields();
        this->_compileTable();
        this->_compileWhere();
        this->_compileGroupBy();
        this->_compileOrderBy();
        this->_compileLimit();
        this->_compileOffset();

        return QString("SELECT %1 FROM %2%3%4%5%6%7%8")
                .arg( this->_fieldsSql )
                .arg( this->_tableSql )
                .arg( this->_joinSql )
                .arg( this->_whereSql )
                .arg( this->_groupSql )
                .arg( this->_orderSql )
                .arg( this->_limitSql )
                .arg( this->_offsetSql )
        ;
    }

    QString _compileUpdate() {
        this->_compileTable();
        this->_compileSet();
        this->_compileWhere();
        return QString("UPDATE %1 SET %2%3")
                .arg( this->_tableSql )
                .arg( this->_setSql )
                .arg( this->_whereSql )
        ;
    }

    QString _compileDelete() {

        this->_compileTable();
        this->_compileWhere();

        return QString("DELETE FROM %1%2")
                .arg( this->_tableSql )
                .arg( this->_whereSql )
        ;
    }

    QString _compileInsert() {
        this->_compileTable();
        return QString("INSERT INTO %1 %2")
                .arg(this->_tableSql)
                .arg(this->_compileInsertMap( this->_insertVals));
    }


protected:
    virtual QString getTableName() = 0;

public:

    void flush() {
        this->_wheres.clear();
        this->_sets.clear();
        this->_fields.clear();
        this->_fields.push_back("*");
        this->_groups.clear();
        this->_orders.clear();
        this->_binds.clear();
        this->_insertVals.clear();
    }

    orm* as( const QString &alias ) {
        this->_tableAlias = alias;
        return this;
    }

    orm* field( const QVector<QString> &field={"*"} ) {
        this->_fields = field;
        return this;
    }

    orm* where( const QString &key, const QString &ope, const QVariant &v ) {
        this->_wheres[key] = QVariantMap({{ope, v}});
        return this;
    }

    orm* orWhere( const QString &key, const QString &ope, const QVariant &v ) {
        this->_orWheres[key] = QVariantMap({{ope, v}});
        return this;
    }

    orm* groupBy( const QString &by ) {
        this->_groups.push_back( by );
        return this;
    }

    orm* orderBy( const QString &by ) {
        this->_orders.push_back( by );
        return this;
    }

    orm* orderByDesc( const QString &by ) {
        this->_orders.push_back( QString("%1 DESC").arg( by ) );
        return this;
    }

    orm* limit( int limit ) {
        this->_limits = limit;
        return this;
    }

    orm* offset( int offset ) {
        this->_offsets = offset;
        return this;
    }

    orm* join( const QString &table, const QString &first, const QString &cond, const QString &second, const QString &concat = "INNER" ) {
        Q_UNUSED(table)
        Q_UNUSED(first)
        Q_UNUSED(cond)
        Q_UNUSED(concat)
        this->_joinSql.append(
                QString(" %1 JOIN %2 ON %3 %4 %5 ")
                .arg( concat.toUpper() )
                .arg(table)
                .arg(first)
                .arg(cond)
                .arg(second))
        ;
        return this;
    }

    QVariantList all() {
        QSqlQuery sqlQuery;
        sqlQuery.prepare( this->_compileQuery() );
        for ( auto bind : this->_binds ) {
            sqlQuery.addBindValue( bind );
        }
        sqlQuery.exec();

        QSqlRecord record = sqlQuery.record();
        int fieldCount = record.count();
        QVector<QString> fieldNames{};
        for ( int i = 0; i < fieldCount; i++ ) {
            fieldNames.push_back( record.fieldName(i) );
        }

        QVariantList ret{};
        while(sqlQuery.next())
        {
            QVariantMap res{};
            for ( auto key: fieldNames ) {
                res[key] = sqlQuery.value( key );
            }
            ret.push_back(res);
        }

        this->flush();
        return ret;
    }

    int count() {
        this->field({"COUNT(*) AS dbCount"});
        QVariantMap v = this->first();
        return v.value("dbCount").toInt();
    }

    QVariantMap first() {
        this->_limits = 1;

        QSqlQuery sqlQuery;
        sqlQuery.prepare( this->_compileQuery() );
        for ( auto bind : this->_binds ) {
            sqlQuery.addBindValue( bind );
        }

        sqlQuery.exec();

        QSqlRecord record = sqlQuery.record();
        int fieldCount = record.count();
        QVector<QString> fieldNames{};
        for ( int i = 0; i < fieldCount; i++ ) {
            fieldNames.push_back( record.fieldName(i) );
        }

        QVariantMap res{};
        if (sqlQuery.next())
        {
            for ( auto key: fieldNames ) {
                res[key] = sqlQuery.value( key );
            }
        }

        this->flush();
        return res;
    }

    bool update(QVariantMap sets) {
        this->_sets =sets;

        QSqlQuery sqlQuery;
        sqlQuery.prepare( this->_compileUpdate() );
        for ( auto bind : this->_binds ) {
            sqlQuery.addBindValue( bind );
        }
        this->flush();
        bool ret = sqlQuery.exec();
        this->numRowsAffected = sqlQuery.numRowsAffected();
        this->lastInsertId = sqlQuery.lastInsertId();
        return ret;
    }

    bool remove() {

        QSqlQuery sqlQuery;
        sqlQuery.prepare( this->_compileDelete() );
        for ( auto bind : this->_binds ) {
            sqlQuery.addBindValue( bind );
        }
        this->flush();
        bool ret = sqlQuery.exec();
        this->numRowsAffected = sqlQuery.numRowsAffected();
        this->lastInsertId = sqlQuery.lastInsertId();
        return ret;
    }

    bool create(const QVariant &v) {
        Q_UNUSED(v)
        switch (v.type() ) {
        case QVariant::Type::Map:
            this->_insertVals.push_back(v);
            break;
        case QVariant::Type::List:
            this->_insertVals = v.toList();
            break;
        default:return false;
        }

        QSqlQuery sqlQuery;
        sqlQuery.prepare( this->_compileInsert() );
        for ( auto bind : this->_binds ) {
            sqlQuery.addBindValue( bind );
        }
        this->flush();
        bool ret = sqlQuery.exec();
        this->numRowsAffected = sqlQuery.numRowsAffected();
        this->lastInsertId = sqlQuery.lastInsertId();
        return ret;
    }

    int getAffectedRows() {
        return this->numRowsAffected;
    }

    QVariant getLastInserId() {
        return this->lastInsertId;
    }
};

class transaction
{
private:
    QSqlDatabase _db;
    QString _connectionName{};
    bool _inTransaction{};

    QString getRandomString() const
    {
       const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
       const int randomStringLength = 12;

       QString randomString{};
       for(int i=0; i<randomStringLength; ++i)
       {
           int index = qrand() % possibleCharacters.length();
           QChar nextChar = possibleCharacters.at(index);
           randomString.append(nextChar);
       }
       return randomString;
    }

public:
    transaction( QSqlDatabase db, bool clone = false ) {
        this->_connectionName = this->getRandomString();
        if ( clone ) {
            this->_db = QSqlDatabase::cloneDatabase(db, this->_connectionName );
            this->_db.open();
        } else
            this->_db = db;
        this->_inTransaction = this->_db.transaction();
    }

    void reset() {
        this->db().removeDatabase(this->db().databaseName());
        if ( this->_inTransaction ) this->_db.rollback();
    }

    QSqlDatabase db() {
        return this->_db;
    }

    QString db_name() {
        return this->_db.databaseName();
    }

    bool commit() {
        return this->_db.commit();
    }

    bool rollback() {
        return this->db().rollback();
    }

    bool isOpen() {
        return this->_db.isOpen();
    }

    ~transaction() {
        if ( this->_inTransaction )
            this->_db.rollback();
    }
};

namespace sqlite{

class database
{
private:
    QSqlDatabase db;
    int numRowsAffected{ -1 };
    QVariant lastInsertId{};

public:
    database( const QString &db_name, const QString &user_name = "", const QString &password = "" ) {
        Q_UNUSED(db_name)
        Q_UNUSED(user_name)
        Q_UNUSED(password)

        QSqlDatabase database;
        if ( QSqlDatabase::contains() ) {
            database = QSqlDatabase::database();
        } else {
            database = QSqlDatabase::addDatabase("QSQLITE");
            database.setDatabaseName(db_name);
            if ( !user_name.isEmpty() )
                database.setUserName(user_name);
            if ( !password.isEmpty() )
                database.setPassword(password);
        }
        if ( !user_name.isEmpty() ) {
            database.open(user_name, password);
        } else {
            database.open();
        }

        this->db = database;
    }

    QSqlDatabase getDb() { return this->db; }

    ~database() {
        if ( this->db.isOpen() ) {
            this->db.close();
        }
    }

    QVariantList executeQuery( const QString &querySql, const QVariantList &binds = {} ) {
        QSqlQuery sqlQuery;
        sqlQuery.prepare( querySql );
        for ( auto bind : binds ) {
            sqlQuery.addBindValue( bind );
        }
        sqlQuery.exec();

        QSqlRecord record = sqlQuery.record();
        int fieldCount = record.count();
        QVector<QString> fieldNames{};
        for ( int i = 0; i < fieldCount; i++ ) {
            fieldNames.push_back( record.fieldName(i) );
        }

        QVariantList ret{};
        while(sqlQuery.next())
        {
            QVariantMap res{};
            for ( auto key: fieldNames ) {
                res[key] = sqlQuery.value( key );
            }
            ret.push_back(res);
        }
        return ret;
    }

    int getAffectedRows() {
        return this->numRowsAffected;
    }

    QVariant getLastInserId() {
        return this->lastInsertId;
    }

    bool executeSql( const QString &sql, const QVariantList &binds = { }) {
        QSqlQuery sqlQuery;
        sqlQuery.prepare( sql );
        for ( auto bind : binds ) {
            sqlQuery.addBindValue( bind );
        }
        bool ret = sqlQuery.exec();
        this->lastInsertId = sqlQuery.lastInsertId();
        this->numRowsAffected = sqlQuery.numRowsAffected();
        return ret;
    }

};

}}

#endif // DB_H
